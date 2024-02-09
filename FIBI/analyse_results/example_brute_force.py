import numpy as np
from typing import *
import bs4
import scipy.special as special
from scipy.stats._stats_py import rankdata, find_repeats
from scipy.stats import distributions, norm


class PValueMapping:
    def __call__(self, value: float, test: str) -> str:
        if np.isnan(value):
            return "pvalue-nan"
        if value < 0.05:
            return "pvalue-small"
        else:
            return "pvalue-big"


class InitDistrMapping:
    def __call__(self, value: float, test: str) -> str:
        if value in ["g"]:
            return "gaussian"
        elif value in ["gh"]:
            return "gaussianHeavy"
        elif value in ["ng"]:
            return "nongaussian"
        else:
            raise Exception


class EffectSizeMapping:
    def __call__(self, value: float, test: str) -> str:
        if np.isnan(value):
            return "es-nan"
        value = abs(value)
        if test in ["ZTest", "TTest"]:
            if value < 0.2:
                return "es-small"
            if 0.2 <= value < 0.5:
                return "es-small-medium"
            elif 0.5 <= value < 0.8:
                return "es-medium-big"
            else:
                return "es-big"
        elif test in ["Wilcoxon", "SignTest"]:
            if value < 0.1:
                return "es-small"
            if 0.1 <= value < 0.3:
                return "es-small-medium"
            elif 0.3 <= value < 0.5:
                return "es-medium-big"
            else:
                return "es-big"

        else:
            raise Exception("Unknown test " + test)


class TTest:
    name = "TTest"

    def __call__(self, diff: np.ndarray):
        diff = diff.astype(np.float64)  # type: ignore
        n = diff.shape[0]
        df = n - 1
        v = np.var(diff, 0, ddof=1)
        dm = np.mean(diff, 0)
        denom = np.sqrt(v / n)
        if denom == 0.0:
            t = np.nan
        else:
            try:
                t = dm / denom
            except Exception:
                print(traceback.format_exc())
                raise Exception
        prob = float(
            special.stdtr(df, -np.abs(t))
            * 2  # stdtr Student t distribution cumulative distribution function
        )
        if v == 0:
            cohens = np.nan
        else:
            cohens = dm / v**0.5
        dico_res = dict(
            variance=v,
            n=n,
            statistic=t,
            pvalue=f"{prob:{'.2e' if prob != np.nan else 'x'}}",
            sigma_norm=denom,
            mean=dm,
            effect_size=f"{cohens:{'.2f' if cohens != np.nan else 'x'}}",
            name=self.name,
            pvalue_classes=[PValueMapping()(prob, test="ZTest")],
            es_classes=[EffectSizeMapping()(cohens, test="ZTest")],
        )
        return dico_res

    def explain_to_text(self, diff: np.ndarray) -> List[bs4.BeautifulSoup]:
        dico_res = self(diff)
        mu = dico_res["mean"]
        variance = dico_res["variance"]
        n = dico_res["n"]
        sigma_norm = dico_res["sigma_norm"]
        t = dico_res["statistic"]
        txt = []
        to_bf = lambda x: bs4.BeautifulSoup(x, "html.parser")
        txt.append(
            to_bf(
                f"There are {len(diff)} samples in the dataset with {n} non-null elements"
            )
        )
        txt.append(
            to_bf(
                r"<p>$$t = \frac{\mu}{\sqrt{\frac{\sigma^2}{n}}} = \frac{"
                + f"{mu}"
                + r"}{\sqrt{\frac{"
                + f"{variance}"
                + r"}{"
                + f"{n}"
                + r"}}} = "
                + f"{dico_res['statistic']:.2e}$$</p>"
            )
        )
        # plot a normal distribution with plotly and add the t value as a vertical line with a text
        fig = go.Figure()
        lim = abs(t) * 2
        fig.add_trace(
            go.Scatter(
                x=np.linspace(-lim, lim, 100),
                y=norm.pdf(np.linspace(-lim, lim, 100)),
                name="normal",
            )
        )
        fig.add_trace(go.Scatter(x=[t, t], y=[0, norm.pdf(t)], name="t"))
        # convert the plotly figure to html self contained
        fig = fig.to_html(full_html=False, include_plotlyjs="cdn")
        txt.append(to_bf(fig))
        txt.append(
            to_bf(
                r"<p>$$cohens= \frac{\mu}{\sigma} = \frac{"
                + f"{mu}"
                + r"}{"
                + f"{sigma_norm}"
                + r"} = "
                + f"{dico_res['effect_size']}$$</p>"
            )
        )
        return txt


class WilcoxonTest:
    name = "Wilcoxon"

    def make_dico_result(
        self,
        es,
        count,
        n_zero,
        r_plus,
        r_minus,
        T,
        mode,
        prob,
        mn=None,
        se=None,
        rep=None,
        z=None,
    ):
        return dict(
            count=count,
            n_zero=n_zero,
            r_plus=r_plus,
            r_minus=r_minus,
            statistic=T,
            pvalue=f"{prob:{'.2e' if prob != np.nan else 'x'}}",
            mode=mode,
            mn=mn,
            se=se,
            z=z,
            rep=rep,
            prob=prob,
            effect_size=f"{es:{'.2f' if es != np.nan else 'x'}}",
            name=self.name,
            pvalue_classes=[PValueMapping()(prob, test=self.name)],
            es_classes=[EffectSizeMapping()(es, test=self.name)],
            es=es,
        )

    def __call__(self, diff: np.ndarray):
        d = diff.astype(np.float64)  # type: ignore
        assert len(diff) >= 10, f"Warning: sample size too small {len(diff)}"

        mode = "approx"

        count = len(d)
        n_zero = np.sum(d == 0)
        if n_zero == len(d):
            prob = np.nan
            return self.make_dico_result(
                es=np.nan,
                count=count,
                n_zero=len(d),
                r_plus=0,
                r_minus=0,
                T=0,
                mode=mode,
                prob=np.nan,
                mn=None,
                se=None,
                z=None,
            )

        r = rankdata(abs(d))
        r_plus = np.sum((d > 0) * r)
        r_minus = np.sum((d < 0) * r)

        # return min for two-sided test, but r_plus for one-sided test
        # the literature is not consistent here
        # r_plus is more informative since r_plus + r_minus = count*(count+1)/2,
        # i.e. the sum of the ranks, so r_minus and the min can be inferred
        # (If alternative='pratt', r_plus + r_minus = count*(count+1)/2 - r_zero.)
        # [3] uses the r_plus for the one-sided test, keep min for two-sided test
        # to keep backwards compatibility

        T = min(r_plus, r_minus)
        mn = None
        se = None
        z = None
        rep = None
        mn = count * (count + 1.0) * 0.25
        se = count * (count + 1.0) * (2.0 * count + 1.0)

        replist, repnum = find_repeats(r)
        if repnum.size != 0:
            rep = repnum
            # Correction for repeated elements.
            se -= 0.5 * (repnum * (repnum * repnum - 1)).sum()

        se = np.sqrt(se / 24)

        # apply continuity correction if applicable
        d = 0

        # compute statistic and p-value using normal approximation
        z = (T - mn - d) / se
        prob = 2.0 * distributions.norm.sf(abs(z))
        es = 4 * abs(T - (r_plus - r_minus) / 2) / (count * (count + 1))
        return self.make_dico_result(
            es, count, n_zero, r_plus, r_minus, T, mode, prob, mn, se, rep, z
        )

    def explain_to_text(self, diff: np.ndarray) -> List[bs4.BeautifulSoup]:
        dico_res = self(diff)
        count = dico_res["count"]
        n_zero = dico_res["n_zero"]
        r_plus = dico_res["r_plus"]
        r_minus = dico_res["r_minus"]
        statistic = dico_res["statistic"]
        mode = dico_res["mode"]
        mn = dico_res["mn"]
        se = dico_res["se"]
        z = dico_res["z"]
        rep = dico_res["rep"]
        prob = dico_res["prob"]
        es = dico_res["es"]
        txt = []
        to_bf = lambda x: bs4.BeautifulSoup(x, "html.parser")
        txt.append(
            to_bf(
                f"There are {len(diff)} samples in the dataset with {count-n_zero} non-null elements"
            )
        )
        txt.append(
            to_bf(
                f"Mode 'approx' with n_zero={n_zero}. This mode by default as for 1000 samples the Wilcoxon exact distribution is not implemente in scipy"
            )
        )
        if count - n_zero < 10:
            txt.append(
                to_bf(
                    f"Not enough non nul values remaining, pvalue and effect size cannot be computed"
                )
            )
        txt.append(
            to_bf(
                r"<p>$$statistic = min(R_+,R_-) = min({r_plus},{r_minus}) = {dico_res['statistic']}$$</p>"
            )
        )
        if n_zero == count:
            txt.append(
                to_bf(f"Only null values, cannot compute pvalue and effect size")
            )
        elif count - n_zero < 10:
            # message already added
            pass
        else:
            txt.append(
                to_bf(
                    f"<p>$$statistic = min(R_+,R_-) = min({r_plus},{r_minus}) = {dico_res['statistic']}$$</p>"
                )
            )
            txt.append(
                to_bf(
                    r"<p>$$\mu = \frac{count \times (count+1)}{4} = "
                    + "\frac{"
                    + f"{count}"
                    + r" \times ("
                    + f"{count}+1)"
                    + r"}{4}$$</p>"
                )
            )
            eq = r"<p>$$\sigma = \sqrt{\frac{count \times (count+1) \times (2\times count+1)"
            eq_txt = (
                r"<p>$$\sigma = \sqrt{\frac{"
                + str(count)
                + r" \times ("
                + str(count)
                + r"+1) \times (2\times "
                + str(count)
                + r"+1)"
            )
            if rep is not None:
                eq += r" - \frac{\sum t \times (t^2-t)}{2}"
                eq_txt = r" - \frac{" + str((rep * (rep * rep - 1)).sum()) + r"}{2}"
            eq += r"}{24}}$$</p>"
            eq_txt += r"}{24}}$$</p>"
            txt.append(to_bf(eq))
            txt.append(to_bf(eq_txt))
            eq = (
                r"<p>$$z=\frac{T-\mu}{\sigma}=\frac{"
                + str(statistic)
                + "-"
                + str(mn)
                + r"}{"
                + str(se)
                + "}$$</p>"
            )
            txt.append(to_bf(eq))
            txt.append(
                to_bf(
                    r"<p>$$pvalue=2 \times Pr(x>|z|)= 2 \times Pr(x>"
                    + str(z)
                    + r")="
                    + str(prob)
                    + "$$</p>"
                )
            )

            # plot a normal distribution with plotly and add the t value as a vertical line with a text
            fig = go.Figure()
            lim = abs(z) * 2
            fig.add_trace(
                go.Scatter(
                    x=np.linspace(-lim, lim, 100),
                    y=norm.pdf(np.linspace(-lim, lim, 100)),
                    name="normal",
                )
            )
            fig.add_trace(go.Scatter(x=[z, z], y=[0, norm.pdf(z)], name="t"))
            # convert the plotly figure to html self contained
            fig = fig.to_html(full_html=False, include_plotlyjs="cdn")
            txt.append(to_bf(fig))
        return txt


from scipy.stats import norm, linregress, probplot

"""--------------------------------------- QQPLOT ---------------------------------------"""


def _ppoints(n, a=0.5):
    """
    Taken from https://github.com/raphaelvallat/pingouin/blob/06801b8719ea73608e958b4e2d1734b0d5d3049b/pingouin/plotting.py
    """
    a = 3 / 8 if n <= 10 else 0.5
    return (np.arange(n) + 1 - a) / (n + 1 - 2 * a)


def qqplot(x, confidence=0.95):
    if np.max(np.abs(x)) < 1e-6:
        raise Exception(
            "No need to plot qqplot for this data as there is no difference between the two distributions"
        )
    if np.std(x) < 1e-6:
        raise Exception(
            "No need to plot qqplot for this data as the two distributions have a constant difference"
        )
    # Taken from https://github.com/raphaelvallat/pingouin/blob/06801b8719ea73608e958b4e2d1734b0d5d3049b/pingouin/plotting.py
    x = x[~np.isnan(x)]
    if len(x) == 0:
        return False
    # Extract quantiles and regression
    quantiles = probplot(x, dist=norm, fit=False)  # type: ignore # type: ignore
    theor, observed = quantiles[0], quantiles[1]

    fit_params = norm.fit(x)
    loc = fit_params[-2]
    scale = fit_params[-1]
    shape = fit_params[:-2] if len(fit_params) > 2 else None

    # Observed values to observed quantiles
    if loc != 0 and scale != 1:
        observed = (np.sort(observed) - loc) / scale

    # Linear regression
    slope, intercept, r, _, _ = linregress(theor, observed)

    # Add diagonal line
    end_pts = [[np.min(theor), np.max(theor)], [np.min(observed), np.max(observed)]]
    end_pts[0] = min(end_pts[0])
    end_pts[1] = max(end_pts[1])

    fit_val = slope * theor + intercept
    # Confidence envelope
    n = x.size
    P = _ppoints(n)
    crit = norm.ppf(1 - (1 - confidence) / 2)
    pdf = norm.pdf(theor) if shape is None else norm.pdf(theor, *shape)
    se = (slope / pdf) * np.sqrt(P * (1 - P) / n)
    upper = fit_val + crit * se
    lower = fit_val - crit * se
    return theor, fit_val, observed, upper, lower


"""------------------------------------- QQPLOT END ---------------------------------------"""


def check_in_inter(theor, fit_val, upper, lower) -> Literal["ng", "gh", "g"]:
    count_out_tail = 0
    for t, v, u, l in zip(theor, fit_val, upper, lower):
        if abs(t) > 1.5 and (v > u or v < l):
            count_out_tail += 1
        elif abs(t) <= 1.5 and v > u or v < l:
            return "ng"
    if count_out_tail < 5:
        return "g"
    else:
        return "gh"


def impr_hansen(bi, fi, init):
    return (bi - fi) / fi, "(bi-fi)/fi"


def impr_bi(bi, fi, init):
    return (bi - fi) / bi, "(bi-fi)/bi"


def impr_init_cost(bi, fi, init):
    return (bi - fi) / init, "(bi-fi)/init"


def diff(bi, fi, init):
    return (bi - fi), "(bi-fi)"


def memoire_example():
    init = np.array([8, 5, 6, 8, 4])
    bi = np.array([4, 3, 4, 5, 2])
    fi = np.array([5, 3, 4, 3, 3])
    ttest = TTest()
    ttest = TTest()
    mapping = {f.__name__: f for f in [impr_hansen, impr_bi, impr_init_cost, diff]}
    print(f"Let us say that we have\ninit_costs: {init.tolist()}\nbi: {bi}\nfi: {fi}")
    print("Then we find")
    for k, fn in mapping.items():
        v = fn(bi, fi, init)[0]
        print(
            f"avg({f'{fn(bi,fi,init)[1]})': <20}= {v.mean().round(3):+.3f} because {v.round(3).tolist()}"
        )


def brute_force():
    import random

    size = 20
    while True:
        print(f"{f'{size=}':-^100}")
        for i in range(500):
            np.random.seed(i)
            random.seed(i)
            # mu = np.random.rand()*48+2
            # sigma = np.random.rand()*mu/4
            # bi = np.random.normal(mu,sigma,size=size)
            # fi = np.random.normal(mu,sigma,size=size)
            bi = np.random.rand(size) * 48 + 2  # [2,10]
            fi = np.random.rand(size) * 48 + 2  # [2,10]
            ttest = TTest()
            wtest = WilcoxonTest()
            mapping = {f.__name__: f for f in [impr_hansen, impr_bi]}
            text = f"Let us say that we have\nbi: {bi}\nfi: {fi}"
            text += "\nThen we find"
            res = {}
            for k, fn in mapping.items():
                v, expr = fn(bi, fi, [])
                theor, fit_val, observed, upper, lower = qqplot(v)
                interval = check_in_inter(theor, observed, upper, lower)
                if interval in ["ng"]:
                    dict_test = wtest(v)
                    pvalue = dict_test["pvalue"]
                else:
                    dict_test = ttest(v)
                    pvalue = dict_test["pvalue"]

                text += f"\n{f'avg({expr})': <20}= {v.mean():+f} pvalue={pvalue.strip()} because {v.tolist()}"
                res[expr] = {
                    "pvalue": pvalue,
                    "metric": v.mean(),
                    "v": v,
                    "sign": np.sign(v.mean()),
                    "pvalue": dict_test["pvalue_classes"],
                    "dict_test": dict_test,
                    "interval": interval,
                }
            if res["(bi-fi)/fi"]["sign"] != res["(bi-fi)/bi"]["sign"] and res["(bi-fi)/fi"]['interval'] == 'ng' and res["(bi-fi)/bi"]['interval'] == 'ng':
                class_pvalue_fi_denom = res["(bi-fi)/fi"]["pvalue"][0]
                class_pvalue_bi_denom = res["(bi-fi)/bi"]["pvalue"][0]
                if (class_pvalue_fi_denom != "pvalue-big") and (
                    class_pvalue_bi_denom != "pvalue-big"
                ):
                    print(f"{class_pvalue_fi_denom=}")
                    print(f"{class_pvalue_bi_denom=}")
                    print(text)
                    for k, d in res.items():
                        v = d["v"]
                        theor, fit_val, observed, upper, lower = qqplot(v)
                        interval = check_in_inter(theor, observed, upper, lower)
                    b = 0
        size += 1
        if size > 100:
            break


if __name__ == "__main__":
    brute_force()
    # memoire_example()
