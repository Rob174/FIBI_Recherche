import FIBI.analyse_results.utils.__init__
from FIBI.analyse_results.visualization.global_analysis.components.__init__ import *
from FIBI.analyse_results.visualization.global_analysis.page_multiinstances import AggregatedData, Aggregator
from FIBI.analyse_results.visualization.utils import default_fibi_order, dicos_fibi_diff
from FIBI.analyse_results.utils.stats import check_in_inter, qqplot

class InitDistrShape(Aggregator):
    def __init__(self, metric: str):
        self.metric = metric

    def aggregate(self, keys: dict, dfs: List[DfExtract]) -> List[AggregatedData]:
        diff = dicos_fibi_diff(dfs, self.metric, True)["-".join(default_fibi_order())]
        try:
            theor, fit_val, observed, upper, lower = qqplot(diff)  # type: ignore
            initialization = check_in_inter(theor, observed, upper, lower)
        except Exception:
            initialization = 'ng'
        return [AggregatedData(
            type= "init_shape",
            data= initialization,
            query= keys,
            name= "Init distr",
            classes= [initialization],
        )]
