from FIBI.analyse_results.visualization.global_analysis.components.__init__ import *
from FIBI.analyse_results.visualization.global_analysis.page_multiinstances import (
    AggregatedData,
    Aggregator,
)
from FIBI.analyse_results.visualization.utils import default_fibi_order, dicos_fibi_diff


class AverageFIBI(Aggregator):
    def __init__(self, metric: str, name: Optional[Callable[[str],str]]=None):
        self.metric = metric
        if name is None:
            name = lambda x:x
        self.name = name

    def aggregate(self, keys: dict, dfs: List[DfExtract]) -> List[AggregatedData]:
        assert len(dfs) == 2
        bifi = dicos_fibi_diff(dfs, self.metric, False)
        return [
            AggregatedData(
                query=keys,
                data=f"{np.mean(bifi[k]):.4f}",
                type= "avg_" + self.metric,
                name= self.name(k),
                classes=[]
            ) for k in default_fibi_order()
        ]


class AverageFIBIDiff(Aggregator):
    def __init__(
        self,
        metric: str,
        attr_diff: str,
        diff_order: List[str],
        name: Optional[Callable[[str],str]]=None
    ):
        self.metric = metric
        self.attr_diff = attr_diff
        self.diff_order = diff_order
        if name is None:
            name = lambda x:x
        self.name = name

    def aggregate(self, keys: dict, dfs: List[DfExtract]) -> List[AggregatedData]:
        diff = dicos_fibi_diff(dfs, self.metric, True)["-".join(default_fibi_order())]
        data = np.mean(diff)
        class_name = "positive" if data > 0 else ("nul" if data == 0 else "negative")
        return [
            AggregatedData(
                query=keys,
                data=f"{data:.2e}",
                type="avg_diff_" + self.metric,
                name=self.name("-".join(self.diff_order)),
                classes=[class_name, "diff"]
            )
        ]
