from FIBI.analyse_results.data_extractor.types import DfExtract
from FIBI.analyse_results.utils.conversions import DicoPathConverter
from FIBI.analyse_results.visualization.global_analysis.page_multiinstances import Aggregator
from FIBI.analyse_results.visualization.legend import Legend
from FIBI.analyse_results.visualization.local_analysis.__init__ import *
from FIBI.analyse_results.data_extractor.data_extractor import MultiInstanceVisualization
from FIBI.analyse_results.utils.data_struct import Hierarchy, Table, same_dict, contains_dict
from FIBI.analyse_results.visualization.types import AggregatedData, BufferDataAggr
import plotly.express as px
import pandas as pd

class PieChartDistrib(MultiInstanceVisualization):
    def __init__(self, out_path: Path, fixed_attrs: List[str], rows: List[str], cols: List[str], aggregators: List[Aggregator], 
                 query_to_path: DicoPathConverter,
                 mappings_attr_names: Dict[str,str], legend: Optional[Legend] = None):
        self.buffer: List[BufferDataAggr] = []
        super(PieChartDistrib, self).__init__(fixed_attrs,['FI_BI'])
        self.out_path = out_path
        self.aggregators = aggregators
        self.mappings_attr_names = mappings_attr_names
        self.legend = legend
        self.query_to_path = query_to_path
        self.rows = rows
        self.cols = cols
    def on_new_data(
        self, keys: dict, dfs: List[DfExtract]
    ): 
        elems: List[AggregatedData] = []
        for c in self.aggregators:
            elems.extend(c.aggregate(keys,dfs))
        self.buffer.append(BufferDataAggr(query=keys,fields=elems))
    def on_end(self):
        mapping = {'ng':'Non-gaussian', 'g':'Gaussian', 'gh': 'Gaussian with heavy-tail'}
        Ldico = {}
        for b in self.buffer:
            for f in b['fields']:
                if f['type'] == 'test_used':
                    c = [e for e in f['classes'] if e in ['ng','g','gh']]
                    if len(c) != 1:
                        raise Exception('Error in test_used')
                    if f['query']['IMPR'] not in Ldico:
                        Ldico[f['query']['IMPR']] = []
                    Ldico[f['query']['IMPR']].append({"distribution": mapping[c[0]]})
        for k in Ldico:
            df = pd.DataFrame(Ldico[k])
            # pie chart with distribution numbers of occurences in percentage of the total
            fig = px.pie(df, values=df.index, names='distribution')
            path_html = self.out_path / f'distributions_{k}.html'
            path_img = self.out_path / f'distributions_{k}.png'
            os.system("dvc unprotect "+path_html.resolve().as_posix())
            os.system("dvc unprotect "+path_img.resolve().as_posix())
            fig.write_html(path_html)
            fig.write_image(path_img)
            os.system("dvc add "+path_html.resolve().as_posix())
            os.system("dvc add "+path_img.resolve().as_posix())
            