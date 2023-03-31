from FIBI.analyse_results.data_extractor.types import DfExtract
from FIBI.analyse_results.visualization.__init__ import *


def dicos_fibi_diff(dfs: List[DfExtract], attr: str, diff: bool = True) -> Dict[str,np.ndarray]:
    assert len(dfs) == 2
    if len(dfs[0]['df']) != len(dfs[1]['df']):
        raise ValueError('The two dataframes must have the same length.')
    dico = {}
    for dfextract in dfs:
        k = dfextract['dico_keys']['FI_BI']
        dico[k] = dfextract['df'].sort_values(by=['SEED_GLOB']).set_index('SEED_ASSIGN')[attr]

    if diff:
        dico['-'.join(default_fibi_order())] = np.array(dico[default_fibi_order()[0]])-np.array(dico[default_fibi_order()[1]]) #type: ignore
    return dico

def dicos_fibi_diff_with_seed_glob(dfs: List[DfExtract], attr: str, diff: bool = True) -> Tuple[Dict[str,np.ndarray],Dict[str,np.ndarray]]:
    assert len(dfs) == 2
    dico = {}
    dico_seeds = {}
    for dfextract in dfs:
        k = dfextract['dico_keys']['FI_BI']
        dico[k] = dfextract['df'].sort_values(by=['SEED_GLOB'])[attr]
        dico_seeds[k] = dfextract['df'].sort_values(by=['SEED_GLOB'])['SEED_GLOB']
    if diff:
        dico['-'.join(default_fibi_order())] = np.array(dico[default_fibi_order()[0]])-np.array(dico[default_fibi_order()[1]]) #type: ignore
        dico_seeds['-'.join(default_fibi_order())] = '('+dico_seeds[default_fibi_order()[0]].map(str).reset_index()['SEED_GLOB']+','+dico_seeds[default_fibi_order()[1]].map(str).reset_index()['SEED_GLOB']+')'
    return dico,dico_seeds
def default_fibi_order():
    return ["BI", "FI"]