import FIBI.analyse_results.utils.__init__
from FIBI.analyse_results.visualization.global_analysis.components.__init__ import *
from FIBI.analyse_results.visualization.global_analysis.page_multiinstances import AggregatedData, Aggregator
from FIBI.analyse_results.visualization.statistical_tests import AbstractStatisticMaker
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

class TestUsed(InitDistrShape):
    def __init__(self, metric: str, test_gauss: AbstractStatisticMaker, test_nongauss: AbstractStatisticMaker):
        self.test_gauss = test_gauss
        self.test_nongauss = test_nongauss
        self.metric = metric
    def aggregate(self, keys: dict, dfs: List[DfExtract]) -> List[AggregatedData]:
        [init_distr] =  super().aggregate(keys, dfs)
        if init_distr['data'] == 'ng':
            test_used = self.test_nongauss.name
            classes = [self.test_nongauss.name, init_distr['data']]
        else:
            test_used = self.test_gauss.name
            classes = [self.test_gauss.name, init_distr['data']]
        return [AggregatedData(
            type= "test_used",
            data= test_used,
            query= keys,
            name= "Stat. test",
            classes= classes,
        )]