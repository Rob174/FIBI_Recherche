from FIBI.analyse_results.data_extractor.types import DfExtract
from FIBI.analyse_results.visualization.types import HTMLDataOut
from FIBI.analyse_results.visualization.local_analysis.components.__init__ import *

class RegisterSeeds:
    def __init__(self):
        self.type = "register_seeds"

    def __call__(self, keys: dict, dfs: List[DfExtract]) -> HTMLDataOut:
        s = bs4.BeautifulSoup(
                "<div><div/>",
                features="html.parser",
            )
        title = s.new_tag("h3")
        title.string = "Range of SEED_GLOB"
        s.append(title)
        for df in dfs:
            p = s.new_tag("div")
            p.string = f"{df['dico_keys']['FI_BI']}"
            # on click, copy in clipboard the global seeds 
            p['onclick'] = f"navigator.clipboard.writeText('{df['df']['SEED_GLOB'].to_list()}')"
            s.append(p)

        return HTMLDataOut(
            query= keys,
            data= s,
            type= self.type,
            name= "Range of SEED_GLOB"
        )
