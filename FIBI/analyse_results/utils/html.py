from FIBI.analyse_results.utils.__init__ import *
from colorsys import hls_to_rgb
import os

def default_html(stylesheet: str, script_id: int=1) -> bs4.BeautifulSoup:
    """stylesheet: str name of the stylesheet without css extension"""
    
    s = bs4.BeautifulSoup(f"""  
        <html>
            <head>
                <link  rel="stylesheet" href="../../../styles/{stylesheet}.css"/>
                <script src="../../../scripts/script{script_id}.js"/>
                <script src="https://polyfill.io/v3/polyfill.min.js?features=es6"></script>
                <script type="text/javascript" id="MathJax-script" src="https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-chtml.js" async/>
            </head>
            <body></body>
        </html>""", 
        features="html.parser")
    return s

class HTMLTable:
    def __init__(self,s: bs4.BeautifulSoup):
        self.s = s
        self.table = s.new_tag('table')
        self.curr_line = None
    def new_line(self) -> bs4.Tag:
        if self.curr_line is not None:
            self.table.append(self.curr_line)
        self.curr_line = self.s.new_tag('tr')
        return self.curr_line
    def cell(self,content: str,classes: List[str], cell_type: Literal['th','td'] = 'td',link: Optional[str] = None,**attrs) -> bs4.Tag:
        cell = self.s.new_tag(cell_type)
        if link is not None:
            a = self.s.new_tag('a')
            a.string = content
            a['href'] = link
            cell.append(a)
        else:
            cell.string = content
        cell['class'] = ' '.join(classes)
        for k,v in attrs.items():
            if '_' == k[0]:
                k = k[1:]
            cell[k] = v
        assert self.curr_line is not None
        self.curr_line.append(cell)
        return cell
    def out(self) -> bs4.Tag:
        return self.table
    

def rgb_to_hex(rgb):
    """Converts an rgb tuple to hex string for web.
    
    >>> rgb_to_hex((204, 0, 0))
    'CC0000'
    """
    return ''.join(["%0.2X" % c for c in rgb])

def color_palet(n_colors: int, sat: float, lum: float = 0.8) -> List[str]:
    L = []
    for i in range(n_colors):
        hue = i / n_colors
        L.append(rgb_to_hex(hls_to_rgb(h=hue,l=lum,s=sat)))
    return L

def soup_to_file(s: bs4.BeautifulSoup,p: Path):
    p.parent.mkdir(exist_ok=True,parents=True)
    os.system("dvc unprotect "+p.resolve().as_posix()+".dvc")
    # Remove the file
    with open(p,'w') as f:
        f.write(str(s.prettify()))
    os.system("dvc add "+p.resolve().as_posix())