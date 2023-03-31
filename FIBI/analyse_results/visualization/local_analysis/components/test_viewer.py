from FIBI.analyse_results.data_extractor.types import DfExtract
from FIBI.analyse_results.visualization.types import HTMLDataOut
from FIBI.analyse_results.visualization.local_analysis.components.__init__ import *
from FIBI.analyse_results.visualization.statistical_tests import SignTest, ZTest
from FIBI.analyse_results.visualization.utils import default_fibi_order, dicos_fibi_diff
from FIBI.analyse_results.utils.stats import check_in_inter, qqplot


class TestViewer:
    def __init__(self, metric: str, name=""):
        self.metric = metric
        self.name = name
        self.type = "qqplot_" + self.metric

    def __call__(self, keys: dict, dfs: List[DfExtract]) -> HTMLDataOut:
        dico_fibi = dicos_fibi_diff(dfs, self.metric, diff=True)
        diff = dico_fibi["-".join(default_fibi_order())]
        try:
            theor, fit_val, observed, upper, lower = qqplot(diff)  # type: ignore
            init = check_in_inter(theor, observed, upper, lower)
        except Exception as e:
            init = 'ng'
        if init in ["ng"]:
            first_test = SignTest()
            second_test = ZTest()
        elif init in ["g", "gh"]:
            first_test = ZTest()
            second_test = SignTest()
        else:
            raise ValueError("Unknown initialization: %s" % init)
        s = bs4.BeautifulSoup("<div><div/>", features="html.parser")
        for test,name in zip([first_test, second_test], ["used", "not used"]):
            test_res = test(diff)
            title = s.new_tag("h3")
            title.string = f"Test {name}: " + test.name
            s.append(title)
            txt = s.new_tag("p")
            for k, v in test_res.items():
                span = s.new_tag("span")
                span.string = k + ":" + str(v)
                txt.append(span)
            s.append(txt)
            div_detail = s.new_tag("div")
            title = s.new_tag("h4")
            title.string = "Details"
            div_detail.append(title)
            for e in test.explain_to_text(diff):
                div_detail.append(e)
            s.append(div_detail)
        return HTMLDataOut(
            query=keys,
            data=s,
            type=self.type,
            name="Statistiques for " + self.metric if self.name == "" else self.name,
        )
