from typing import *
from FIBI.analyse_results.visualization.__init__ import *
from scipy.stats import distributions, norm
import traceback


class AbstractStatisticMaker(abc.ABC):
    name = None

    def __init__(self) -> None:
        super().__init__()
        if self.name is None:
            raise Exception

    @abc.abstractmethod
    def __call__(self, diff: np.ndarray):
        pass


class LegendDict(TypedDict):
    name: str
    legend: list


class AbstractClassMapping(abc.ABC):
    @abc.abstractmethod
    def __call__(self, value: float, test: str) -> str:
        pass


class SignTest(AbstractStatisticMaker):
    name = "Sign Test"

    def __call__(self, diff: np.ndarray):
        assert len(diff) >= 10, f"Warning: sample size too small {len(diff)}"
        # Source: https://openpress.usask.ca/introtoappliedstatsforpsych/chapter/16-2-median-sign-test/
        n = np.count_nonzero(diff)
        if n == 0:
            return dict(
                X="x",
                statistic="x",
                pvalue="x",
                r_plus="x",
                r_minus="x",
                n=0,
                effect_size="x",
                name=self.name,
                pvalue_classes=[PValueMapping()(np.nan, test="SignTest")],
                es_classes=[EffectSizeMapping()(np.nan, test="SignTest")],
            )
        r_plus = np.sum((diff > 0), axis=0)
        r_minus = np.sum((diff < 0), axis=0)
        X = min(r_plus, r_minus)
        if X < n / 2:
            Z = ((X + 0.5) - n / 2) / (n**0.5 / 2)
        else:
            Z = ((X - 0.5) - n / 2) / (n**0.5 / 2)
        prob = float(2.0 * distributions.norm.sf(abs(Z)))
        effect_size = abs(X / n - 0.5)
        result = dict(
            X=X,
            statistic=Z,
            pvalue=f"{prob:{'.2e' if prob != np.nan else 'x'}}",
            r_plus=r_plus,
            r_minus=r_minus,
            n=n,
            effect_size=f"{effect_size:{'.2f' if effect_size != np.nan else 'x'}}",
            name=self.name,
            pvalue_classes=[PValueMapping()(prob, test="SignTest")],
            es_classes=[EffectSizeMapping()(effect_size, test="SignTest")],
        )
        return result

    def explain_to_text(self, diff: np.ndarray) -> List[bs4.BeautifulSoup]:
        dico_res = self(diff)
        n: int = dico_res["n"]
        r_plus = (
            dico_res["r_plus"] if not isinstance(dico_res["r_plus"], str) else np.nan
        )
        r_minus = (
            dico_res["r_minus"] if not isinstance(dico_res["r_minus"], str) else np.nan
        )
        X: float = dico_res["X"] if not isinstance(dico_res["X"], str) else np.nan
        Z = (
            dico_res["statistic"]
            if not isinstance(dico_res["statistic"], str)
            else np.nan
        )
        to_bf = lambda x: bs4.BeautifulSoup(x, "html.parser")
        txt = []
        txt.append(
            to_bf(
                f"There are {len(diff)} samples in the dataset with {n} non-null elements"
            )
        )
        txt.append(
            to_bf(
                f"<p>As $$R_+ = {r_plus}$$ and $$R_- = {r_minus}$$, we have $$X = min(R_+,R_-) = min({r_plus},{r_minus})={X}$$</p>"
            )
        )
        if X < n / 2:
            txt.append(
                to_bf(
                    r"<p>Then as $$X < \frac{n}{2}$$ (X="
                    + str(X)
                    + r" and n="
                    + str(n)
                    + r"), we have $$Z = \frac{X+0.5-\frac{n}{2}}{\sqrt{\frac{n}{4}}} = \frac{"
                    + str(X)
                    + r"+0.5-\frac{"
                    + str(n)
                    + r"}{2}}{\sqrt{\frac{"
                    + str(n)
                    + r"}{4}}}="
                    + str(dico_res["statistic"])
                    + r"$$</p>"
                )
            )
        else:
            txt.append(
                to_bf(
                    r"<p>Then as $$X \geq \frac{n}{2}$$ (X="
                    + str(X)
                    + r" and n="
                    + str(n)
                    + r"), we have $$Z = \frac{X-0.5-\frac{n}{2}}{\sqrt{\frac{n}{4}}} = \frac{"
                    + str(X)
                    + r"-0.5-\frac{"
                    + str(n)
                    + r"}{2}}{\sqrt{\frac{"
                    + str(n)
                    + r"}{4}}}="
                    + str(dico_res["statistic"])
                    + r"$$</p>"
                )
            )
        # plot a normal distribution with plotly and add the Z value as a vertical line with a text
        fig = go.Figure()
        lim = abs(Z) * 2
        fig.add_trace(
            go.Scatter(
                x=np.linspace(-lim, lim, 100),
                y=norm.pdf(np.linspace(-lim, lim, 100)),
                name="normal",
            )
        )
        fig.add_trace(go.Scatter(x=[Z, Z], y=[0, norm.pdf(Z)], name="Z"))
        # convert the plotly figure to html self contained
        fig = fig.to_html(full_html=False, include_plotlyjs="cdn")
        txt.append(
            to_bf(
                f"<p>Then we have $$p = 2 \\times P(Z > |Z|) = 2 \\times P(Z > {Z:.2f}) = {dico_res['pvalue']}$$</p>"
            )
        )
        txt.append(to_bf(fig))
        # Then detail the formula for the effect size
        txt.append(
            to_bf(
                f"<p>Finally, the effect size is $$\\frac{{X}}{{n}} - \\frac{{1}}{{2}} = \\frac{{{X}}}{{{n}}} - \\frac{{1}}{{2}} = {dico_res['effect_size']}$$</p>"
            )
        )
        return txt


from numpy import compress
from scipy.stats._stats_py import rankdata, find_repeats
from scipy.stats._hypotests import _get_wilcoxon_distr


class WilcoxonTest(AbstractStatisticMaker):
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
        n = diff.shape[0]

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
                    + str(prob)+"$$</p>"
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


import scipy.special as special


class TTest(AbstractStatisticMaker):
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


class ZTest(AbstractStatisticMaker):
    name = "ZTest"

    def __init__(self) -> None:
        self.name = ZTest.name
        super().__init__()

    def __call__(self, diff: np.ndarray):
        diff = diff.astype(np.float64)  # type: ignore
        assert len(diff) >= 10, f"Warning: sample size too small {len(diff)}"
        n = diff.shape[0]
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
            distributions.norm.sf(np.abs(t)) * 2
        )  # use np.abs to get upper tail
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

def tests_wilcoxon_ttest() -> Dict[str,AbstractStatisticMaker]:
    return {"test_gauss":TTest(),"test_nongauss":WilcoxonTest()}

def tests_signtest_ztest() -> Dict[str,AbstractStatisticMaker]:
    return {"test_gauss":ZTest(),"test_nongauss":SignTest()}

class PValueMapping(AbstractClassMapping):
    def __call__(self, value: float, test: str) -> str:
        if np.isnan(value):
            return "pvalue-nan"
        if value < 1e-5:
            return "pvalue-small"
        else:
            return "pvalue-big"


class InitDistrMapping(AbstractClassMapping):
    def __call__(self, value: float, test: str) -> str:
        if value in ["g"]:
            return "gaussian"
        elif value in ["gh"]:
            return "gaussianHeavy"
        elif value in ["ng"]:
            return "nongaussian"
        else:
            raise Exception


class EffectSizeMapping(AbstractClassMapping):
    def __call__(self, value: float, test: str) -> str:
        if np.isnan(value):
            return "es-nan"
        value = abs(value)
        if test == "ZTest":
            if value < 0.5:
                return "es-small"
            elif 0.5 <= value < 0.8:
                return "es-medium"
            else:
                return "es-big"
        elif test == "Wilcoxon":
            if value < 0.3:
                return "es-small"
            elif 0.3 <= value < 0.5:
                return "es-medium"
            else:
                return "es-big"

        else:
            if value < 0.15:
                return "es-small"
            elif 0.15 <= value < 0.25:
                return "es-medium"
            else:
                return "es-big"
