"""File in charge of creating the datasets for the TSP problem. 

```txt
                 http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/...
                                     ▲
                                     │

                                     │
                        ┌────────────┴────────────┐
tsplib_urls.json ───────►get_instances_from_urls()│
                        └──┬──────────────────────┘
                           │ {
                           │    "inst1.tsp": {
                           │        "data":[[x1,y1],[x2,y2],...],
                           │        "comment": "",
┌───────────────┬──────────┘        "type":"TSP",
│               │                   "edge_weight_type": "EUC_2D"
│               │               },
│               │               ...
│               │            }
│    ┌──────────▼─────────────┐
│    │ dump_name_ids(problems)├────► mapping_tsplib_less_than_1000.json  (names / ids mapping)
│    └────────────────────────┘
│
│
│    ┌─────────────────────┐
└────►create_hdf5(problems)├──────► tsplib_1000_stop.hdf5 (data for the c++ program)
     └─────────────────────┘
```

Done with https://asciiflow.com/
"""

import requests
import json
import re
from h5py import File
import numpy as np
import time
from pathlib import Path
from typing import *

InstanceName = str
Point = List[float]
TypeInstance = Literal['TSP']
EdgeWeightType = Literal['EUC_2D']

class Instance(TypedDict):
    data: List[Point]
    """The points of the instance. In the instance taken always 2 coordinates (only 2D instances)"""
    comment: str
    type: TypeInstance
    """The type of the instance (always TSP)"""
    edge_weight_type: EdgeWeightType
    """The type of the edge weight (always EUC_2D, preselected to only get 2D euclidean instances)"""

def get_instances_from_urls() -> Dict[InstanceName, Instance]:
    """
    Get the instances from the urls of each instance (urls of http://elib.zib.de/pub/mp-testdata/tsp/tsplib/tsp/... ) specified in the json file.
    
    ## Returns
        A dictionary with the name of the instance as key and the instance (cf Instance doc) as value.
    
    """
    with open(Path('.') / "data" / "src_datasets" / "tsp" / "tsplib_urls.json", "r") as f:
        urls = json.load(f)
    problems = {}
    # NB : pattern for integer, float or float written in scientific notation
    # WARNING : order of condition is important to match if possible scientific notation and then other possibilities
    pattern_number = r"-?(?:\d+\.\d+[eE]?[+-]?\d*|\d+)"
    regexes = {
        "data": rf"^{pattern_number} +({pattern_number}) +({pattern_number})$",
        "dataOpt": r"^({pattern_number})$",
        "type": r"^TYPE ?: ?(.*)$",
        "edge_weight_type": r"^EDGE_WEIGHT_TYPE ?: ?(.*)$",
        "comment": r"^COMMENT : (.*)$",
    }
    for url in urls:
        # Get the problem name from the url
        problem_name = url.split("/")[-1]
        time.sleep(np.random.rand()*0.1) # to avoid spamming the server
        data = requests.get(url).content
        problems[problem_name] = {"data": []}
        for l in data.decode("utf-8").split("\n"):
            l = l.strip()
            if l.strip() == "":
                continue
            for regex in regexes:
                match = re.search(regexes[regex], l)
                if match is not None:
                    values = match.groups()
                    if regex in ["data", "dataOpt"]:
                        val = [float(x) for x in values]
                        problems[problem_name]["data"].append(
                            val if len(val) > 1 else val[0]
                        )
                    else:
                        problems[problem_name][regex] = values[0]
                    break
        # Filter out the euclidean problems
        if "edge_weight_type" not in problems[problem_name]:
            del problems[problem_name]
        elif "EUC" not in problems[problem_name]["edge_weight_type"]:
            del problems[problem_name]
        elif len(problems[problem_name]["data"]) == 0:
            raise Exception(
                "No data found for problem {}. View if file comply with this script parsing process".format(
                    problem_name
                )
            )
    return problems
def dump_name_ids(problems):
    """Map the index of the problem (as retrieved in problems) to the name of the instance as it is easier in the c++ program to get the instances with an integer (knowing the maximum number of instances) than with a string (which would require a mapping in c++)"""
    mapping_name = {}
    i = 0
    for k, v in problems.items():
        k = k.split(".")[0]
        condition = 0 <= np.array(v["data"]).flatten().shape[0] < 1000*2
        if condition:
            mapping_name[i] = k
            i+=1
    with open(Path('.') / 'data' / 'algorithms_in' / 'tsp'/ 'tsplib' / 'mapping_tsplib_less_than_1000.json','w') as fp:
        json.dump(mapping_name,fp)
def create_hdf5(problems):
    """Build the hdf5 file with the instances. The instances are stored in the dataset with the index of the instance as name."""
    with File(Path('.') / 'data' / 'algorithms_in' / 'tsp' / 'tsplib' / 'tsplib_1000_stop.hdf5', "w") as f:
        i = 0
        for k, v in problems.items():
            k = k.split(".")[0]
            condition = 0 <= np.array(v["data"]).flatten().shape[0] < 1000*2
            if condition:
                f.create_dataset(str(i), data=np.array(v["data"]))
                i+=1

def main():
    """Main function to get the instances and create the hdf5 file"""
    problems = get_instances_from_urls()
    dump_name_ids(problems)
    create_hdf5(problems)
    