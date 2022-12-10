from FIBI.analyse_results.data_extractor.types import DfExtract
from FIBI.analyse_results.visualization.types import HTMLDataOut
from FIBI.analyse_results.visualization.utils import default_fibi_order, dicos_fibi_diff
from FIBI.analyse_results.visualization.local_analysis.components.__init__ import *

class HistoFIBI:
    def __init__(self, metric: str, name=""):
        self.metric = metric
        self.name = name
        self.type = "histo_fibi_" + self.metric

    def __call__(self, keys: dict, dfs: List[DfExtract]) -> HTMLDataOut:
        dico_fibi = dicos_fibi_diff(dfs, self.metric, diff=False)
        fig = go.Figure()
        for k, array in dico_fibi.items():
            fig.add_trace(go.Histogram(x=array, name=k))
        return HTMLDataOut(
            query= keys,
            data= bs4.BeautifulSoup(
                fig.to_html(full_html=False, include_plotlyjs="cdn"),
                features="html.parser",
            ),
            type= self.type,
            name= "Histogram of " + self.metric if self.name == "" else self.name,
        )


class HistoFIBIDiff:
    def __init__(self, metric: str, name=""):
        self.metric = metric
        self.name = name
        self.type = "histo_fibi_" + self.metric

    def __call__(self, keys: dict, dfs: List[DfExtract]) -> HTMLDataOut:
        dico_fibi = dicos_fibi_diff(dfs, self.metric, diff=True)
        fig = go.Figure()
        for k, array in dico_fibi.items():
            if k in default_fibi_order():
                continue
            fig.add_trace(go.Histogram(x=array, name=k))

        return HTMLDataOut(
            query= keys,
            data= bs4.BeautifulSoup(
                fig.to_html(full_html=False, include_plotlyjs="cdn"),
                features="html.parser",
            ),
            type= self.type,
            name= "Histogram of  "
            + self.metric
            + " of "
            + "-".join(default_fibi_order())
            if self.name == ""
            else self.name,
        )
