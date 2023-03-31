from FIBI.analyse_results.data_extractor.types import DfExtract
from FIBI.analyse_results.visualization.types import HTMLDataOut
from FIBI.analyse_results.visualization.local_analysis.components.__init__ import *
from FIBI.analyse_results.visualization.utils import default_fibi_order, dicos_fibi_diff

class TableAttrFIBI:
    def __init__(self, metric: str,name = ''):
        self.metric = metric
        self.name = name
        self.type = 'stats_'+self.metric

    def __call__(self, keys: dict, dfs: List[DfExtract]) -> HTMLDataOut:
        dico_stats = {k:pd.Series(v).describe().to_dict() for k, v in dicos_fibi_diff(dfs,self.metric).items()}
        s = bs4.BeautifulSoup('<div><div/>', features="html.parser")
        t = HTMLTable(s)
        t.new_line()
        for i,fibi in enumerate(["",*default_fibi_order(),'-'.join(default_fibi_order())]):
            if i == 0:
                class_names = ['header']
            else:
                class_names = ['header-col']
            t.cell(content=fibi,classes=class_names,cell_type='th')
        for k in dico_stats[default_fibi_order()[1]]:
            t.new_line()
            t.cell(content=f"{k}",cell_type='th',classes=['header-row'])
            for fibi in dico_stats:
                t.cell(content=f"{dico_stats[fibi][k]}",cell_type='td',classes=[])
        t.new_line()
        return HTMLDataOut(query= keys, data= t.out(),type=self.type,name='Statistiques '+self.metric if self.name =='' else self.name)