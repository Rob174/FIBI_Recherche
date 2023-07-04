import FIBI.analyse_results.utils.__init__
from FIBI.analyse_results.visualization.global_analysis.components.__init__ import *
from FIBI.analyse_results.visualization.global_analysis.components.averages import AverageFIBI, AverageFIBIDiffTgt
from FIBI.analyse_results.visualization.global_analysis.components.stat_tests import PValueEffectSize
from FIBI.analyse_results.visualization.global_analysis.page_multiinstances import (
    Aggregator,
)
from FIBI.analyse_results.visualization.local_analysis.components.generic import AbstractComponentObserver, GenericComponent
from FIBI.analyse_results.visualization.utils import default_fibi_order, dicos_fibi_diff
from FIBI.analyse_results.visualization.statistical_tests import AbstractStatisticMaker, SignTest, ZTest, WilcoxonTest, TTest
from FIBI.analyse_results.visualization.global_analysis.components.init_distr_shape import (
    InitDistrShape)

from typing import List, Tuple
from collections import defaultdict
from FIBI.analyse_results.visualization.global_analysis.pie_chart import get_case

def open_tsplib(root_folder: Path, seed_problem: int) -> Tuple[pd.DataFrame]:
    with open(root_folder / f'{seed_problem}.txt') as f:
        points = [float(e.strip()) for e in f.readlines()]
    points = np.array(points).reshape(-1,2)
    return (pd.DataFrame({'x':points[:,0],'y':points[:,1]}),)

def open_clustering(root_folder: Path, seed_problem: int) -> Tuple[pd.DataFrame,int]:
    with open(root_folder / 'point' / f'{seed_problem}.txt') as f:
        points = [float(e.strip()) for e in f.readlines()]
    with open(root_folder / 'dims' / f'{seed_problem}.txt') as f:
        num_dims = [float(e.strip()) for e in f.readlines()][0]
    points = np.array(points).reshape(-1,num_dims)
    with open(root_folder / 'num_clusters' / f'{seed_problem}.txt') as f:
        num_clusters = int([float(e.strip()) for e in f.readlines()][0])
    return pd.DataFrame({'x':points[:,0],'y':points[:,1]}), str(num_clusters)
    
def open_maxsat(root_folder: Path, seed_problem: int) -> Tuple[List[List[Tuple[int, bool]]], List[float], int]:
    with open(root_folder / f'{seed_problem}.txt') as f:
        data = [float(e.strip()) for e in f.readlines()]
    return parse_maxsat(data)
def parse_maxsat(data: List[float]) -> Tuple[List[List[Tuple[int, bool]]], List[float], int]:
    # parse data
    weights = []
    clauses = []

    variables = set()
    # taille is the size of the clause (number of variables inside) + 1 for weighted instances (if unweighted weight of 1)
    taille = 0
    skipNext = False  # because first is weight
    for i in range(len(data)):
        if taille == 0:
            taille = data[i]
            skipNext = True
            clauses.append([])
        elif skipNext:
            taille -= 1
            skipNext = False
            weights.append(data[i])
        else:
            variable = int(abs(data[i])) - 1
            variables.add(variable)
            clauses[-1].append((variable, (data[i] > 0)))
            taille -= 1

    return (clauses, weights, len(variables))

def visualize_scatter(path_out: Path, points: pd.DataFrame, num_clust: str = '', name: str = ''):
    fig = px.scatter(points,x="x",y="y", title=f"{name}<br>{num_clust}")
    fig.write_html(path_out)
    
class Instance(Aggregator):
    def __init__(self, folder: Path, folder_out: Path, parse_fn: Callable[[Path,int],Any], visualize_fn: Callable[[Any],None]):
        self.folder = folder
        self.parse_fn = parse_fn
        self.visualize_fn = visualize_fn
        self.folder_out = folder_out
        self.done = set()
    def make_key(self, keys: dict) -> str:
        return f"{keys['INSTANCE']},{keys['PROBLEM']},{keys['DATASET']}"
    def aggregate(
        self, keys: dict, dfs: List[DfExtract]
    ) -> List[dict]:
        key = self.make_key(keys)
        if key in self.done:
            return
        seed_glob = keys['SEED_GLOB']
        data = self.parse_fn(self.folder,seed_glob)
        path = self.folder_out / f"{seed_glob}.html"
        self.visualize_fn(path,*data,name=keys['INSTANCE'])
        self.done.add(key)
        return [
            {
                "type": "instance",
                "data": path,
                "query": keys,
                "name": "Instance",
                "classes": [],
            },
        ]