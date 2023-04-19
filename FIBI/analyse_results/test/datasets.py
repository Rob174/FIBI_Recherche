import pandas as pd
from typing import *
from pathlib import Path
from FIBI.analyse_results.parser.parser import JSONParser
import pandas as pd
class CheckNumSeedGlob:
    """Check if there are the expected number of elements per hyperparameter combination"""
    def __init__(self, data: List[Dict]) -> None:
        L = []
        seeds_glob = set({})
        for d in data:
            if d['SEED_GLOB'] in seeds_glob:
                continue
            seeds_glob.add(d['SEED_GLOB'])
            if "duration" not in d:
                continue
            L.append(d)
        self.seeds_glob = seeds_glob
        self.df = pd.DataFrame(L)
    def check(self, dataset: int, columns_agg: List, exp_num: int = 1000) -> bool:
        check_ok = True
        result = self.df.query("DATASET == "+str(dataset)).groupby(columns_agg).agg({'SEED_GLOB': ['count', lambda x: ','.join(map(lambda a:str(int(a)), x))]})
        result.columns = result.columns.map('_'.join)
        result = result.rename(columns={'SEED_GLOB_count': 'SEED_GLOB_count_agg', 'SEED_GLOB_<lambda_0>': 'SEED_GLOB_agg'})
        result = result.reset_index()
        for e in result.to_dict(orient='records'):
            if e['SEED_GLOB_count_agg'] != exp_num:
                check_ok = False
                print({k:v for k,v in e.items() if k != "SEED_GLOB_agg"})
                # print(e['SEED_GLOB_count_agg'])
        return check_ok
    def get_contiguous_intervals(self,lst):
        intervals = []
        start = lst[0]
        end = lst[0]
        for i in range(1, len(lst)):
            if lst[i] == end + 1:
                end = lst[i]
            else:
                intervals.append([start, end])
                start = lst[i]
                end = lst[i]
        intervals.append([start, end])
        return intervals
    def get_missing_seeds(self):
        max_seed = max(self.seeds_glob)
        missing = []
        for i in range(int(max_seed)):
            if i not in self.seeds_glob:
                missing.append(i)
        cont_int = self.get_contiguous_intervals(missing)
        # print("missing: ",cont_int)
        return missing
        
def check_tsp(path: Path):
    p = JSONParser()
    with p.open_file(path) as f:
        data = list(p.get_data(f))
    c = CheckNumSeedGlob(data)
    if not c.check(0,["NUM_TOWNS","IMPR","FI_BI"]):
        raise Exception("Not ok for TSP uniform points")
    if not c.check(1,["SEED_PROBLEM","IMPR","FI_BI"]):
        raise Exception("Not ok for TSP uniform points")
    
def check_clustering(path: Path):
    p = JSONParser()
    with p.open_file(path) as f:
        data = list(p.get_data(f))
    c = CheckNumSeedGlob(data)
    with open("missing.txt","w") as f:
        f.write(",".join(str(e) for e in c.get_missing_seeds()))
    if not c.check(0,["NUM_POINTS","NUM_CLUST","IMPR","FI_BI"]):
        raise Exception("Not ok for Clustering uniform points")
    if not c.check(1,["SEED_PROBLEM","NUM_CLUST","IMPR","FI_BI"]):
        raise Exception("Not ok for Clustering franti")
    if not c.check(2,["SEED_PROBLEM","NUM_CLUST","IMPR","FI_BI"]):
        raise Exception("Not ok for Clustering aloise")
    if not c.check(3,["NUM_POINTS","NUM_CLUST","IMPR","FI_BI"]):
        raise Exception("Not ok for Clustering blobs")
    
def check_maxsat(path: Path):
    p = JSONParser()
    with p.open_file(path) as f:
        data = list(p.get_data(f))
    c = CheckNumSeedGlob(data)
    if not c.check(0,["SEED_PROBLEM","NUM_VARIABLES","NUM_CLAUSES","IMPR","FI_BI"]):
        raise Exception("Not ok for MAXSAT benchmark")
    
    