from FIBI.analyse_results.data_extractor.types import DfExtract
from FIBI.analyse_results.utils.conversions import DicoPathConverter
from FIBI.analyse_results.visualization.local_analysis.__init__ import *
from FIBI.analyse_results.data_extractor.data_extractor import SingleInstanceVisualization
from FIBI.analyse_results.visualization.local_analysis.components.histograms import HistoFIBI, HistoFIBIDiff
from FIBI.analyse_results.visualization.local_analysis.components.qqplot import QQPlot
from FIBI.analyse_results.visualization.local_analysis.components.tables import TableAttrFIBI
from FIBI.analyse_results.visualization.local_analysis.components.test_viewer import TestViewer


def analyse_attr(attr: str) -> List:
    return [
        TableAttrFIBI(attr),
        HistoFIBI(attr),
        HistoFIBIDiff(attr)
    ]
class PageInstance(SingleInstanceVisualization):
    def __init__(self, folder_out: Path, fixed_attrs: List[str], 
                 query_to_path: DicoPathConverter):
        self.components = [
            *analyse_attr('init_cost'),
            *analyse_attr('final_cost'),
            *analyse_attr('ratio'),
            QQPlot('ratio'),
            TestViewer('ratio')
        ]
        self.order = [c.type for c in (self.components)]
        super(PageInstance, self).__init__(fixed_attrs,['FI_BI'])
        self.folder_out = folder_out
        self.query_to_path = query_to_path
    def on_new_data(
        self, keys: dict, dfs: List[DfExtract]
    ): 
        elems: List[HTMLDataOut] = []
        for c in self.components:
            elems.append(c(keys,dfs))
        s = default_html('style3',2)
        title = s.new_tag('h1')
        title.string = 'Instance '+', '.join([f'{k}={v}' for k,v in keys.items()])
        s.html.body.append(title)
        for dico in elems:
            d = s.new_tag('div')
            title = s.new_tag('h2')
            title.string = dico['name']
            title['class'] = dico['type']
            d.append(title)
            d.append(dico['data'])
            s.html.body.append(d)
        self.folder_out.mkdir(exist_ok=True, parents=True)
        soup_to_file(s, self.folder_out / (self.query_to_path.to_path(elems[0]['query'])+'.html'))