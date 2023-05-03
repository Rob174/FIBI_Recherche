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
from importlib import reload
from string import Template

def customwrap(s,width=30):
    return "<br>".join(textwrap.wrap(s,width=width))
def get_case(d: dict):
    case_chosen = ""
    main_case = ""
    if d['avg'] == 'avgConclOk' and d['signif'] == 'pvalue-small' and (d['es'] != 'es-small'):
        case_chosen = customwrap("Expected avg sign, significant difference with effect size not small")
        color = "#09DB00"
        main_case = "Verified"
        case_letter = "A"
    elif d['avg'] == 'avgConclOk' and d['signif'] == 'pvalue-small' and d['es'] == 'es-small':
        case_chosen = customwrap("Expected avg sign, significant difference with small effect size")
        main_case = "Verified"
        color = "#09DB00"
        case_letter = "B"
    elif d['avg'] == 'avgConclKo' and d['signif'] == 'pvalue-small' and (d['es'] != 'es-small'):
        case_chosen = customwrap("Opposite avg sign, significant difference with effect size not small")
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
def make_cases(buffer):
    # create a dict counting the number of occurences of the following cases
    # Case Expected avg sign, significant difference with medium/big effect size
    # Case Opposite avg sign, significant difference with medium/big effect size
    # Case no difference at all: classes contains nul
    # Case no significant difference: classes contains pvalue-big
    # Case Expected avg sign, significant difference with small effect size
    # Case Opposite avg sign, significant difference with small effect size
    
    Ldico = {}
    for b in buffer:
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
        Ldico[f['query']['IMPR']].append({'case':case_chosen,'main_case':main_case,'color':color,'letter':case_letter, 'query': f['query']})
    return Ldico

def make_plotly_piechart(Ldico, out_path):
    for k in Ldico:
        df = pd.DataFrame(Ldico[k])[['main_case','case','letter','color']].groupby(['main_case','case','letter','color']).size().reset_index(name='number')            
        # add a new column with one uniq letter per case
        df.sort_values(by=['main_case','letter'], ascending=[0,0],inplace=True)
        fig = px.sunburst(df, path=['main_case', 'letter'], values="number", 
                            width=600, height=600, color='color', color_discrete_map={'#A100FF': '#A100FF', '#09DB00': '#09DB00', '#FA1D00': '#FA1D00'}
                            )
        fig.update_traces(textinfo="label+percent entry", insidetextorientation='horizontal',textfont=dict(size=12))
        fig.update_layout(uniformtext=dict(minsize=13, mode="show"))
        fig.write_html(out_path / f'cases_{k}.html')
        fig.write_image(out_path / f'cases_{k}.png')
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
        soup_to_file(s,out_path / f'cases_legend_{k}.html')

class PieChart(MultiInstanceVisualization):
    """A class to create the pie chart of the cases of """
    def __init__(self, dataset: str, problem: str, out_path: Path, fixed_attrs: List[str], rows: List[str], cols: List[str], aggregators: List[Aggregator], 
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
        self.dataset = dataset
        self.problem = problem
    def on_new_data(
        self, keys: dict, dfs: List[DfExtract]
    ): 
        elems: List[AggregatedData] = []
        for c in self.aggregators:
            elems.extend(c.aggregate(keys,dfs))
        self.buffer.append(BufferDataAggr(query=keys,fields=elems))
    def on_end(self):
        Ldico = make_cases(self.buffer)
        make_latex_piechart(Ldico,self.out_path,self.dataset, self.problem)
        make_plotly_piechart(Ldico, self.out_path)

def format_identifier(s: str) -> str:
    s = s.replace(' ','')
    s = s.replace('.','')
    return s
def make_one_latex_piechart(problem: str, dataset: str, init: str, values:dict, template_path: Path, size_inner:int = 2, size_out:int = 4):
    """Make a latex sunburst pie chart from the values provided:
    # In
        init: str, the initialization used for the data
        values: dict[str,int] the number of times each case as specified in the get_case function has been seen
        template_path: Path, path to the template to create the latex figure
        size_inner: int, the size of the inner pie chart (main categories: A, B, C)
        size_outer: int, the size of the outer pie chart (main categories: A1, A2, B1, B2, B3, C1)
    # Out
        the latex code to show the sunburst pie chart
    """
    a = values['a']
    a1 = values['a1']
    a2 = values['a2']
    b = values['b']
    b1 = values['b1']
    b2 = values['b2']
    b3 = values['b3']
    c = values['c']
    c1 = values['c1']
    tot = a+b+c
    # inner group
    data: Dict[str,Tuple[float,Dict[str,float]]] = {"a":(a,{"a1":a1,"a2":a2}),"b":(b,{"b1":b1,"b2":b2,"b3":b3}),"c":(c,{"c1":c1})}
    lines = []
    def generator_inner(data):
        angle = 0
        for innerCat,(nbVals,dico_inner) in data.items():
            frac = nbVals/tot
            perc = frac*100
            start_angle = angle
            end_angle = start_angle + frac*360
            middle = (start_angle+end_angle)/2
            angle = end_angle
            yield innerCat, nbVals, frac,perc, start_angle, end_angle, middle,dico_inner
    
    def generator_outer(data):
        angle = 0
        for innerCat, nbVals, frac,perc, start_angle, end_angle, middle,dico_inner in generator_inner(data):
            for subcat, nbValsSubCat in dico_inner.items():
                frac = nbValsSubCat/tot
                perc = frac*100
                start_angle = angle
                end_angle = start_angle + frac*360
                middle = (start_angle+end_angle)/2
                angle = end_angle
                yield innerCat,subcat, nbValsSubCat, frac,perc, start_angle, end_angle, middle
    
    lines.append(f"% outer pie: subcategories")
    for innerCat, outerCat, nbVals, frac,perc, start_angle, end_angle, middle in generator_outer(data):
        lines.append(f"\\filledarc{{{start_angle}}}{{{end_angle}}}{{\\sizeOuterPie}}{{\\catOuter{innerCat.upper()}Color}}")
    
        
    lines.append(f"% inner pie: categories")
    for innerCat, nbVals, frac,perc, start_angle, end_angle, middle,dico_inner in generator_inner(data):
        lines.append(f"\\filledarc{{{start_angle}}}{{{end_angle}}}{{\\sizeInnerPie}}{{\\cat{innerCat.upper()}Color}}")
    
    lines.append(f"% Outer pie labels")
    for outerCat, innerCat, nbVals, frac,perc, start_angle, end_angle, middle in generator_outer(data):
        lines.append(f"\\node at ({middle}:\\posRadLabOuter) {{{innerCat.upper()}}};")
        lines.append(f"\\node [below] at ({middle}:\\posRadLabOuter) {{{perc:.2f}\\%}};")
        if perc < 0.01:
            lines[-1] = "% "+lines[-1]
            lines[-2] = "% "+lines[-2]
        
    lines.append(f"% inner pie labels")
    for innerCat, nbVals, frac,perc, start_angle, end_angle, middle, dico in generator_inner(data):
        lines.append(f"\\node at ({middle}:\\posRadLabInner) {{{innerCat.upper()}}};")
        lines.append(f"\\node [below] at ({middle}:\\posRadLabInner) {{{perc:.2f}\\%}};")
        if perc < 0.01:
            lines[-1] = "% "+lines[-1]
            lines[-2] = "% "+lines[-2]
    
        
    with open(template_path) as f:
        template = f.read()
    template = Template(template)
    result = template.substitute(
        diagram="\n    ".join(lines),
        sizeOuter=size_out,
        sizeInner=size_inner,
        sizeFig="\\sizeFig",
        sizeFigCaption="\\sizeFigCaption",
        title=f"{dataset}, {init}",
        reference=f"res{format_identifier(problem).capitalize()}{format_identifier(dataset).capitalize()}{format_identifier(init)}"
    )
    return result
def make_latex_piechart(Ldico, out_path, dataset, problem, template_sunburst: Optional[Path] = None, template_fig: Optional[Path] = None):
    if template_sunburst is None:
        template_sunburst = Path(".") / "data" / "analysis_results" / "styles" / "sunburst.txt"
    if template_fig is None:
        template_fig = Path(".") / "data" / "analysis_results" / "styles" / "templateFigSunburst.txt"
        
    with open(template_fig) as f:
        template = f.read()
    template = Template(template)
    diagrams = {}
    for k in Ldico:
        df = pd.DataFrame(Ldico[k])[['main_case','case','letter','color']].groupby(['main_case','case','letter','color']).size().reset_index(name='number')            
        L = df.to_dict(orient='records')
        
        a = sum(e['number'] for e in L if e['letter'] in ['A','B'])
        a1 = sum(e['number'] for e in L if e['letter'] in ['A'])
        a2 = sum(e['number'] for e in L if e['letter'] in ['B'])
        b = sum(e['number'] for e in L if e['letter'] in ['C','D','E'])
        b1 = sum(e['number'] for e in L if e['letter'] in ['C'])
        b2 = sum(e['number'] for e in L if e['letter'] in ['D'])
        b3 = sum(e['number'] for e in L if e['letter'] in ['E'])
        c = sum(e['number'] for e in L if e['letter'] in ['F'])
        c1 = sum(e['number'] for e in L if e['letter'] in ['F'])
        res = make_one_latex_piechart(
            dataset=dataset,
            problem=problem,
            init=k,
            values={
                "a":a,
                "a1":a1,
                "a2": a2,
                "b":b,
                "b1":b1,
                "b2":b2,
                "b3":b3,
                "c":c,
                "c1": c1
            },
            template_path=template_sunburst
        )
        diagrams[k] = res
    def sort_fn(s: str) -> Tuple:
        pos = 0,0,0
        if 'RAND' in s:
            return pos
        return 1, len(s), s
    diagrams = [diagrams[k] for k in sorted(diagrams,key=sort_fn)]
    result = template.substitute(
        sunbursts="\n    ".join([d.replace("\n","\n    ") for d in diagrams]),
        title=f"{problem}, {dataset}",
        reference=f"res{format_identifier(problem).capitalize()}{format_identifier(dataset).capitalize()}",
    )
    out_path.mkdir(parents=True, exist_ok=True)
    path_out = out_path / f'piechart_{dataset}.tex'
    os.system("dvc unprotect "+path_out.resolve().as_posix()+".dvc")
    with open(path_out, 'w') as f:
        f.write(result)
    os.system("dvc add "+path_out.resolve().as_posix())
    