from FIBI.analyse_results.visualization.__init__ import *

class LegendFieldDict(TypedDict):
    txt: str
    classes: str
class LegendElem(TypedDict):
    name: str
    fields: List[LegendFieldDict]
    
class Legend:
    def __init__(self,elems: List[LegendElem]):
        self.elems = elems
    def __call__(self) -> bs4.BeautifulSoup:
        s = bs4.BeautifulSoup('<div class="legend"></div>',features="html.parser")
        title = s.new_tag('h2')
        title.string = "Legend"
        s.div.append(title) #type: ignore
        for e in self.elems:
            d = s.new_tag('div')
            subtitle = s.new_tag('div')
            subtitle['class'] =  'legend-elem legend-title'
            subtitle.string = e['name']
            d.append(subtitle)
            for f in e['fields']:
                legendDiv = s.new_tag('div')
                legendDiv['class'] =  'legend-elem'
                
                rect = s.new_tag('div')
                rect['class'] = ' '.join(['rect',f['classes']])
                legendDiv.append(rect)
                
                txt = s.new_tag('div')
                txt.string = f['txt']
                legendDiv.append(txt)
                
                d.append(legendDiv)
            s.div.append(d) #type: ignore
        return s