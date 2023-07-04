from FIBI.analyse_results.data_extractor.types import DfExtract
from FIBI.analyse_results.utils.conversions import DicoPathConverter
from FIBI.analyse_results.visualization.legend import Legend
from FIBI.analyse_results.visualization.local_analysis.__init__ import *
from FIBI.analyse_results.data_extractor.data_extractor import MultiInstanceVisualization
from FIBI.analyse_results.utils.data_struct import Hierarchy, Table, same_dict, contains_dict
from FIBI.analyse_results.visualization.types import AggregatedData, BufferDataAggr

class Aggregator(abc.ABC):
    @abc.abstractmethod
    def aggregate(self, keys: dict, dfs: List[DfExtract]) -> List[AggregatedData]:
        pass

class PageMultiInstance(MultiInstanceVisualization):
    def __init__(self, out_path: Path, fixed_attrs: List[str], rows: List[str], cols: List[str], aggregators: List[Aggregator], 
                 query_to_path: DicoPathConverter,
                 mappings_attr_names: Dict[str,str], legend: Optional[Legend] = None):
        self.buffer: List[BufferDataAggr] = []
        super(PageMultiInstance, self).__init__(fixed_attrs,['FI_BI'])
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
        ## Build the table
        rows = []
        for e in self.buffer:
            dico = {k:e['query'][k] for k in self.rows}
            rows.append({k:v for k,v in dico.items()})
        rows = pd.DataFrame(rows).drop_duplicates()
        rows = rows.sort_values(by=list(rows.columns)).to_dict('records')
        cols = []
        for e in self.buffer:
            dico = {k:e['query'][k] for k in self.cols}
            for e2 in self.buffer[0]['fields']:
                dico[' '] = e2['name']
                cols.append({k:v for k,v in dico.items()})
        cols = pd.DataFrame(cols).drop_duplicates()
        cols[' '] = pd.Categorical(cols[' '], [e['name'] for e in self.buffer[0]['fields']])
        cols = cols.sort_values(by=[c for c in cols.columns]).to_dict('records')
        
        assert len(rows) == len(pd.DataFrame(rows).drop_duplicates(inplace=False))
        h_rows = Hierarchy(rows)
        h_cols = Hierarchy(cols)
        t = Table(h_rows, h_cols, self.mappings_attr_names)
        for l in t.get_lines():
            data_l = [e for e in self.buffer if contains_dict(e['query'],l)]
            for c in t.get_cols():
                dico_without_data = {k:v for k,v in c.items() if k != ' '}
                data_l_c = [e for e in data_l if contains_dict(
                    e['query'],dico_without_data)
                              ]
                if len(data_l_c) == 1:
                    [data_l_c] = [e for e in data_l_c[0]['fields'] if e['name'] == c[' ']]
                    t[l,c] = data_l_c
                elif len(data_l_c) == 0:
                    t[l,c] = None
                else:
                    raise Exception(f'Multiple data for the same line and col with {l} and {c} and {data_l_c}')
        
        s = default_html(stylesheet="style2",script_id=1)
        if self.legend is not None:
            s.body.append(self.legend())
        html_t = HTMLTable(s)
        for l in t.iter_rows():
            html_t.new_line()
            for e in l:
                txt = e['content']
                classes = []
                link = '../stats/'+self.query_to_path.to_path(e['content']['query'])+'.html' if e['type_elem'] in ['data'] and e['content'] is not None else None
                if e['type_elem'] != 'data' and isinstance(e['content'],dict):
                    txt = str(list(txt.values())[0])
                    if 'classes' in e['content']:
                        classes.extend(e['content']['classes'])
                elif e['type_elem'] in ['header','header-row-name','header-col-name']:
                    txt = str(e['content'])
                elif e['type_elem'] in ['data']:
                    if e['content'] is None:
                        txt = ''
                    else:
                        if 'classes' in e['content']:
                            classes.extend(e['content']['classes'])
                        if e['content']['type'] == 'init_shape':
                            txt = ''
                        elif e['content']['type'] == 'instance':
                            link = e['content']['data']
                            txt = 'view'
                        else:
                            txt = str(e['content']['data'])
                else:
                    raise ValueError
                classes = [e['type_elem'],*classes ]
                if e['first_child'] == 'true':
                    classes.append("first_child")
                
                html_t.cell(txt,
                            cell_type='th' if 'header' in e['type_elem'] else 'td',
                            colspan=e['width'],
                            rowspan=e['height'],
                            link=link,
                            classes=classes)
        html_t.new_line()
        s.append(html_t.out())
        soup_to_file(s,self.out_path)