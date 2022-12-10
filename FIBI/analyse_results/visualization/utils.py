from FIBI.analyse_results.data_extractor.types import DfExtract
from FIBI.analyse_results.visualization.__init__ import *


def dicos_fibi_diff(dfs: List[DfExtract], attr: str, diff: bool = True) -> Dict[str,np.ndarray]:
    assert len(dfs) == 2
    dico = {}
    for dfextract in dfs:
        k = dfextract['dico_keys']['FI_BI']
        dico[k] = dfextract['df'].sort_values(by=['SEED_GLOB'])[attr]
    if diff:
        dico['-'.join(default_fibi_order())] = np.array(dico[default_fibi_order()[0]])-np.array(dico[default_fibi_order()[1]]) #type: ignore
    return dico

def default_fibi_order():
    return ["BI", "FI"]