import FIBI.analyse_results.visualization.global_analysis.pie_chart as pc
from pathlib import Path
import pyperclip
p = pyperclip.copy(pc.make_one_latex_piechart(
    "","","",
    values={
        "a":3,
        "a1":2,
        "a2":1,
        "b":5,
        "b1":2,
        "b2":2,
        "b3":1,
        "c":2,
        "c1":2
    },
    template_path=Path(".") / "data" / "analysis_results" / "styles" / "sunburst.txt",
    ))