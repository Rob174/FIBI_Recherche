from FIBI.analyse_results.visualization.global_analysis.components.__init__ import *
from FIBI.analyse_results.visualization.global_analysis.page_multiinstances import (
    Aggregator,
)
from FIBI.analyse_results.visualization.utils import default_fibi_order, dicos_fibi_diff
from FIBI.analyse_results.visualization.statistical_tests import SignTest, ZTest, WilcoxonTest, TTest
from FIBI.analyse_results.visualization.global_analysis.components.init_distr_shape import (
    InitDistrShape)


class PValueEffectSize(Aggregator):
    def __init__(self, metric: str):
        self.metric = metric

    def aggregate(
        self, keys: dict, dfs: List[DfExtract]
    ) -> List[dict]:
        init = InitDistrShape('ratio').aggregate(keys, dfs)[0]['data']
        diff = dicos_fibi_diff(dfs, self.metric, True)["-".join(default_fibi_order())]
        if init in ["ng"]:
            test = WilcoxonTest()
        elif init in ["g", "gh"]:
            test = TTest()
        else:
            raise ValueError("Unknown initialization: %s" % init)
        classes = [
            test.__class__.__name__,
        ]
        test_res = test(diff)
        return [
            {
                "type": "pvalue",
                "data": test_res["pvalue"],
                "query": keys,
                "name": "PValue",
                "classes": [*classes, *test_res["pvalue_classes"]],
            },
            {
                "type": "effect_size",
                "data": test_res["effect_size"],
                "query": keys,
                "name": "Effect size",
                "classes": [*classes, *test_res["es_classes"]],
            },
        ]
