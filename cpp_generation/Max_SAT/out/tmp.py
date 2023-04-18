import pandas as pd
import numpy as np
from h5py import File
import json
from rich.progress import track
import shutil, cProfile
from pathlib import Path


with open("mapping.json") as f:
    m = json.load(f)


def parse_hdf5(name: str) -> pd.DataFrame:
    L = []
    with File(name, "r") as f:
        n = len(f["metadata"])
        i = 0
        m_i = 10000
        for k, v in track(f["metadata"].items(), total=m_i):
            if i == m_i:
                break
            array = np.array(v)
            data = {}
            for d in m:
                id = int(d["id"])
                name = d["name"]
                data[name] = array[id]
            L.append(data)
            i+=1
    df = pd.DataFrame(L)
    return df


if __name__ == "__main__":
    profiler = cProfile.Profile()
    profiler.enable()
    df = parse_hdf5('dataset.hdf5')
    profiler.disable()
    profiler.dump_stats(Path(".") / "profile.prof")

