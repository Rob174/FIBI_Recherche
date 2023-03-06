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

    def __init__(self) -> None:
        self.name = SignTest.name
        super().__init__()

    def __call__(self, diff: np.ndarray):
        assert len(diff) >= 10, f"Warning: sample size too small {len(diff)}"
        # Source: https://openpress.usask.ca/introtoappliedstatsforpsych/chapter/16-2-median-sign-test/
        n = np.count_nonzero(diff)
        if n == 0:
            return dict(
                X='x',
                statistic='x',
                pvalue='x',
                r_plus='x',
                r_minus='x',
                n=0,
                effect_size='x',
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
        n = dico_res["n"]
        r_plus = dico_res["r_plus"] if not isinstance(dico_res["r_plus"], str) else np.nan
        r_minus = dico_res["r_minus"] if not isinstance(dico_res["r_minus"], str) else np.nan
        X = dico_res["X"] if not isinstance(dico_res["X"], str) else np.nan
        Z = dico_res["statistic"] if not isinstance(dico_res["statistic"], str) else np.nan
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
        else:
            if value < 0.15:
                return "es-small"
            elif 0.15 <= value < 0.25:
                return "es-medium"
            else:
                return "es-big"
