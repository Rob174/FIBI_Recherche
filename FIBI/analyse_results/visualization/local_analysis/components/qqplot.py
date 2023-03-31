from FIBI.analyse_results.data_extractor.types import DfExtract
from FIBI.analyse_results.visualization.types import HTMLDataOut
from FIBI.analyse_results.visualization.local_analysis.components.__init__ import *
from FIBI.analyse_results.visualization.utils import default_fibi_order, dicos_fibi_diff
from FIBI.analyse_results.utils.stats import check_in_inter, qqplot


class QQPlot:
    def __init__(self, metric: str, name=""):
        self.metric = metric
        self.name = name
        self.type = "qqplot_" + self.metric

    def __call__(self, keys: dict, dfs: List[DfExtract]) -> HTMLDataOut:
        dico_fibi = dicos_fibi_diff(dfs, self.metric, diff=True)
        diff = dico_fibi["-".join(default_fibi_order())]
        try:
            theor, fit_val, observed, upper, lower = qqplot(diff)  # type: ignore
            initialization = check_in_inter(theor, observed, upper, lower)
        except Exception as e:
            return HTMLDataOut(
                query=keys,
                data=bs4.BeautifulSoup(f"<div>{e}<div/>", features="html.parser"),
                type=self.type,
                name="QQPlot of " + self.metric if self.name == "" else self.name,
            )
        s = bs4.BeautifulSoup("<div><div/>", features="html.parser")
        fig = go.Figure()
        fig.add_trace(
            go.Scatter(
                x=theor,
                y=observed,
                name="Observed values",
                mode="markers",
                marker=dict(size=5, line=dict(width=2)),
            )
        )
        fig.add_trace(
            go.Scatter(x=theor, y=upper, name="Upper bound interval 95%", mode="lines")
        )
        fig.add_trace(
            go.Scatter(x=theor, y=lower, name="Lower bound interval 95%", mode="lines")
        )
        lim_start = np.min([np.min(theor), np.min(observed)])
        lim_end = np.max([np.max(theor), np.max(observed)])
        fig.add_trace(
            go.Scatter(
                x=[lim_start, lim_end],
                y=[lim_start, lim_end],
                name="Diagonal line",
                mode="lines",
            )
        )
        fig.update_layout(
            xaxis_title="Theorical quantiles", yaxis_title="Observed quantiles"
        )
        html = bs4.BeautifulSoup(
            fig.to_html(full_html=False, include_plotlyjs="cdn"), features="html.parser"
        )
        s.div.append(html) # type: ignore
        p = s.new_tag("p")
        p.string = "Initialisation: " + initialization
        s.div.append(p) # type: ignore
        return HTMLDataOut(
            query=keys,
            data=s,
            type=self.type,
            name="QQPlot of " + self.metric if self.name == "" else self.name,
        )
