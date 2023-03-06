from FIBI.analyse_results.data_extractor.types import DfExtract
from FIBI.analyse_results.utils.conversions import DicoPathConverter
from FIBI.analyse_results.visualization.global_analysis.page_multiinstances import Aggregator
from FIBI.analyse_results.visualization.legend import Legend
from FIBI.analyse_results.visualization.local_analysis.__init__ import *
from FIBI.analyse_results.data_extractor.data_extractor import MultiInstanceVisualization
from FIBI.analyse_results.utils.data_struct import Hierarchy, Table, same_dict, contains_dict
from FIBI.analyse_results.visualization.types import AggregatedData, BufferDataAggr
import re
import plotly.express as px
import pandas as pd
import textwrap
import FIBI.analyse_results.visualization.global_analysis.test as t
from importlib import reload

class PieChart(MultiInstanceVisualization):
    def __init__(self, out_path: Path, fixed_attrs: List[str], rows: List[str], cols: List[str], aggregators: List[Aggregator], 
                 query_to_path: DicoPathConverter,
                 mappings_attr_names: Dict[str,str], legend: Optional[Legend] = None):
        self.buffer: List[BufferDataAggr] = []
        super(PieChart, self).__init__(fixed_attrs,['FI_BI'])
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
        # create a dict counting the number of occurences of the following cases
        # Case Expected avg sign, significant difference with medium/big effect size
        # Case Opposite avg sign, significant difference with medium/big effect size
        # Case no difference at all: classes contains nul
        # Case no significant difference: classes contains pvalue-big
        # Case Expected avg sign, significant difference with small effect size
        # Case Opposite avg sign, significant difference with small effect size
        def customwrap(s,width=30):
            return "<br>".join(textwrap.wrap(s,width=width))
        def get_case(d: dict):
            case_chosen = ""
            main_case = ""
            if d['avg'] == 'avgConclOk' and d['signif'] == 'pvalue-small' and (d['es'] == 'es-big' or d['es'] == 'es-medium'):
                case_chosen = customwrap("Expected avg sign, significant difference with medium/big effect size")
                color = "#09DB00"
                main_case = "Verified"
                case_letter = "A"
            elif d['avg'] == 'avgConclOk' and d['signif'] == 'pvalue-small' and d['es'] == 'es-small':
                case_chosen = customwrap("Expected avg sign, significant difference with small effect size")
                main_case = "Verified"
                color = "#09DB00"
                case_letter = "B"
            elif d['avg'] == 'avgConclKo' and d['signif'] == 'pvalue-small' and (d['es'] == 'es-big' or d['es'] == 'es-medium'):
                case_chosen = customwrap("Opposite avg sign, significant difference with medium/big effect size")
                color = "#FA1D00"
                main_case = "Not verified"
                case_letter = "C"
            elif d['avg'] == 'nul':
                case_chosen = "no difference at all"
                color = "#FA1D00"
                main_case = customwrap("Not verified")
                case_letter = "D"
            elif d['avg'] == 'avgConclKo' and d['signif'] == 'pvalue-small' and d['es'] == 'es-small':
                case_chosen = customwrap("Opposite avg sign, significant difference with small effect size")
                color = "#FA1D00"
                main_case = "Not verified"
                case_letter = "E"
            elif d['signif'] == 'pvalue-big':
                case_chosen = "no significant difference"
                color = "#A100FF"
                main_case = "Undetermined"
                case_letter = "F"
            else:
                raise Exception("Case not found with dico: " + str(d))
            return main_case,case_chosen,color,case_letter
        Ldico = {}
        for b in self.buffer:
            case_dict = {}
            for f in b['fields']:
                if f['type'] == 'avg_diff_ratio':
                    c = [c for c in f['classes'] if "avg" in c]
                    case_dict['avg'] = c[0] if len(c) == 1 else 'nul'
                elif f['type'] == 'pvalue':
                    c = [c for c in f['classes'] if re.match("pvalue",c)]
                    if len(c) == 1:
                        case_dict['signif'] = c[0]
                    else:
                        raise Exception("pvalue should have only one class with "+str(f))
                elif f['type'] == 'effect_size':
                    c = [c for c in f['classes'] if re.match("es",c)]
                    if len(c) == 1:
                        case_dict['es'] = c[0]
                    else:
                        raise Exception("effect_size should have only one class with "+str(f))
                    if f['query']['IMPR'] not in Ldico:
                        Ldico[f['query']['IMPR']] = []
            main_case,case_chosen,color,case_letter = get_case(case_dict)
            Ldico[f['query']['IMPR']].append({'case':case_chosen,'main_case':main_case,'color':color,'letter':case_letter})
        for k in Ldico:
            df = pd.DataFrame(Ldico[k])[['main_case','case','letter','color']].groupby(['main_case','case','letter','color']).size().reset_index(name='number')            
            # add a new column with one uniq letter per case
            df.sort_values(by=['main_case','letter'], ascending=[0,0],inplace=True)
            fig = px.sunburst(df, path=['main_case', 'letter'], values="number", 
                              width=600, height=600, color='color', color_discrete_map={'#A100FF': '#A100FF', '#09DB00': '#09DB00', '#FA1D00': '#FA1D00'}
                              )
            fig.update_traces(textinfo="label+percent entry", insidetextorientation='horizontal',textfont=dict(size=12))
            fig.update_layout(uniformtext=dict(minsize=13, mode="show"))
            fig.write_html(self.out_path / f'cases_{k}.html')
            fig.write_image(self.out_path / f'cases_{k}.png')
            # Create the legend in a neww html file
            s = default_html('style4')
            t = s.new_tag('h1')
            t.string = 'Legend pie chart cases'
            s.append(t)
            for i,case, letter in zip(range(len(df)),df['case'],df['letter']):
                color = df.loc[df['case'] == case,'color'].unique()[0]
                t = s.new_tag('div')
                sp = s.new_tag('span')
                sp.string = letter
                sp['style'] = f'color: {color};'
                p = s.new_tag('p')
                p.string = case.replace('<br>',' ')
                t.append(sp)
                t.append(p)
                s.append(t)
            soup_to_file(s,self.out_path / f'cases_legend_{k}.html')