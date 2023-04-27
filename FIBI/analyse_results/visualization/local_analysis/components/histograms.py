from FIBI.analyse_results.data_extractor.types import DfExtract
from FIBI.analyse_results.visualization.types import HTMLDataOut
from FIBI.analyse_results.visualization.utils import (
    default_fibi_order,
    dicos_fibi_diff,
    dicos_fibi_diff_with_seed_glob,
)
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
            query=keys,
            data=bs4.BeautifulSoup(
                fig.to_html(full_html=False, include_plotlyjs="cdn"),
                features="html.parser",
            ),
            type=self.type,
            name="Histogram of " + self.metric if self.name == "" else self.name,
        )


class HistoFIBIDiff:
    def __init__(self, metric: str, name="", postscript_path: Path = None):
        self.metric = metric
        self.name = name
        self.type = "histo_fibi_" + self.metric
        self.postscript_path = postscript_path

    def __call__(self, keys: dict, dfs: List[DfExtract]) -> HTMLDataOut:
        dico_fibi, dico_seeds = dicos_fibi_diff_with_seed_glob(
            dfs, self.metric, diff=True
        )
        # make a df with the difference and the seed_glob pairs
        df = pd.DataFrame(
            {
                "diff": dico_fibi["-".join(default_fibi_order())],
                "SEED_GLOB": dico_seeds["-".join(default_fibi_order())],
            }
        )
        fig = go.Figure()
        hist, bin_edges = np.histogram(df["diff"], bins=100)
        xbins = {
            "start": bin_edges[0],
            "end": bin_edges[-1],
            "size": bin_edges[1] - bin_edges[0],
        }
        # using bin_eges make the group of SEED_GLOB in the same bin using dfs[0].df['SEED_GLOB']
        groups = {}
        for binStart, binEnd in zip(bin_edges[:-1], bin_edges[1:]):
            groups[f"{binStart},{binEnd}"] = list(
                df[(df["diff"] >= binStart) & (df["diff"] < binEnd)]["SEED_GLOB"].values
            )
            groups[f"{binStart},{binEnd}"] = f"{binStart}->{binEnd}:" + ";".join(
                groups[f"{binStart},{binEnd}"][
                    : min(5, len(groups[f"{binStart},{binEnd}"]))
                ]
            )
        fig.add_trace(
            go.Histogram(
                x=dico_fibi["-".join(default_fibi_order())],
                name="-".join(default_fibi_order()),
                xbins=xbins,
                hovertext=list(groups.values()),
            )
        )
        fig.update_layout(
            clickmode="event",
        )
        with open(self.postscript_path, "r") as f:
            script = f.read()
        out = f'<div>{fig.to_html(full_html=False, include_plotlyjs="cdn", post_script=script)}</div>'

        return HTMLDataOut(
            query=keys,
            data=bs4.BeautifulSoup(out, features="html.parser"),
            type=self.type,
            name="Histogram of  "
            + self.metric
            + " of "
            + "-".join(default_fibi_order())
            if self.name == ""
            else self.name,
        )
