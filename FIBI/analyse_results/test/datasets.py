import pandas as pd
from typing import *
from pathlib import Path
from FIBI.analyse_results.parser.parser import JSONParser
from math import ceil
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
        return missing
    def save_missing_seeds(self, missing: List[int], folder: Path, num_files: int = 2, server: str = ""):
        # split missing into num_files lists
        step = ceil(len(missing) / num_files)
        lists = [[missing[i] for i in range(k*step,(k+1)*step) if i < len(missing)] for k in range(num_files)]
        for i in range(num_files):
            with open(folder / f"missing{i+1}.txt",'w') as f:
                f.write(",".join(map(str,lists[i])))
        if server != "":
            with open(folder / f"upload.sh",'w') as f:
                f.write("#!/bin/bash/\n"+"\n".join([f"mv missing{i+1}.txt missing.txt&&scp missing.txt {server}{i+1}/" for i in range(num_files)]))
        
        
        
def check_tsp(path: Path, folder_missing: Path, num_files: int = 2, server: str = ""):
    p = JSONParser()
    with p.open_file(path) as f:
        data = list(p.get_data(f))
    c = CheckNumSeedGlob(data)
    # c.save_missing_seeds(c.get_missing_seeds(),folder_missing,num_files,server)
    if not c.check(0,["NUM_TOWNS","IMPR","FI_BI"]):
        raise Exception("Not ok for TSP uniform points")
    if not c.check(1,["SEED_PROBLEM","IMPR","FI_BI"]):
        raise Exception("Not ok for TSP uniform points")
    
def check_clustering(path: Path, folder_missing: Path, num_files: int = 2, server: str = ""):
    p = JSONParser()
    with p.open_file(path) as f:
        data = list(p.get_data(f))
    c = CheckNumSeedGlob(data)
    # c.save_missing_seeds(c.get_missing_seeds(),folder_missing,num_files,server)
    if not c.check(0,["NUM_POINTS","NUM_CLUST","IMPR","FI_BI"]):
        raise Exception("Not ok for Clustering uniform points")
    if not c.check(1,["SEED_PROBLEM","NUM_CLUST","IMPR","FI_BI"]):
        raise Exception("Not ok for Clustering franti")
    if not c.check(2,["SEED_PROBLEM","NUM_CLUST","IMPR","FI_BI"]):
        raise Exception("Not ok for Clustering aloise")
    if not c.check(3,["NUM_POINTS","NUM_CLUST","IMPR","FI_BI"]):
        raise Exception("Not ok for Clustering blobs")
    
def check_maxsat(path: Path, folder_missing: Path, num_files: int = 2, server: str = ""):
    p = JSONParser()
    with p.open_file(path) as f:
        data = list(p.get_data(f))
    c = CheckNumSeedGlob(data)
    # c.save_missing_seeds(c.get_missing_seeds(),folder_missing,num_files,server)
    if not c.check(0,["SEED_PROBLEM","NUM_VARIABLES","NUM_CLAUSES","IMPR","FI_BI"]):
        raise Exception("Not ok for MAXSAT benchmark")
    
    