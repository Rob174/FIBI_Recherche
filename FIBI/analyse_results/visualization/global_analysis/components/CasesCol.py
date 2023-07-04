import FIBI.analyse_results.utils.__init__
from FIBI.analyse_results.visualization.global_analysis.components.__init__ import *
from FIBI.analyse_results.visualization.global_analysis.components.averages import AverageFIBI, AverageFIBIDiffTgt
from FIBI.analyse_results.visualization.global_analysis.components.stat_tests import PValueEffectSize
from FIBI.analyse_results.visualization.global_analysis.page_multiinstances import (
    Aggregator,
)
from FIBI.analyse_results.visualization.local_analysis.components.generic import AbstractComponentObserver, GenericComponent
from FIBI.analyse_results.visualization.utils import default_fibi_order, dicos_fibi_diff
from FIBI.analyse_results.visualization.statistical_tests import AbstractStatisticMaker, SignTest, ZTest, WilcoxonTest, TTest
from FIBI.analyse_results.visualization.global_analysis.components.init_distr_shape import (
    InitDistrShape)

from FIBI.analyse_results.visualization.global_analysis.pie_chart import get_case

class CasesCol(Aggregator):
    def __init__(self, avg_ratio: AverageFIBIDiffTgt, pvalue_es: PValueEffectSize, observable: Optional[GenericComponent] = None):
        self.avg_ratio = avg_ratio
        self.pvalue_es = pvalue_es
        self.observable = observable

    def aggregate(
        self, keys: dict, dfs: List[DfExtract]
    ) -> List[dict]:
        [agg_avg] = self.avg_ratio.aggregate(keys,dfs)
        [agg_pvalue, agg_es] = self.pvalue_es.aggregate(keys,dfs)
        pvalue = [c for c in agg_pvalue['classes'] if "pvalue-" in c][0]
        es = [c for c in agg_es['classes'] if "es-" in c][0]
        avg = None
        if "nul" in agg_avg['classes']:
            avg = "nul"
        else:
            avg = [c for c in agg_avg['classes'] if "avgConcl" in c][0]
        main_case,case_chosen,color,case_letter = get_case({
            "avg": avg,
            "signif": pvalue,
            "es": es
        }) 
        if self.observable is not None:
            self.observable.on_case_found(keys, case_letter)
        return [
            {
                "type": "case",
                "data": case_letter,
                "query": keys,
                "name": "Case",
                "classes": [case_letter],
            },
        ]
