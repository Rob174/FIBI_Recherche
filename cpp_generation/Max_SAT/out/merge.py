from h5py import File
from pathlib import Path
import numpy as np
import json
from rich.progress import track

if __name__ == "__main__":
    NUM_FOLDERS = 10

    root = Path(".").resolve()
    data = []
    with File(str(root / "dataset.hdf5"), "w") as f:
        g = f.create_group("metadata")
        for folder_id in reversed(range(1, NUM_FOLDERS + 1)):
            # Extract the json file if present and content is a list
            try:
                with open(str(root / f"{folder_id}" / "actions.json"), "r") as f_:
                    content = json.load(f_)
                    if isinstance(content, list):
                        data.extend(content)
            except Exception as e:
                pass
            
            path = root / f"{folder_id}" / "dataset.hdf5"
            print(path)
            try:
                with File(str(path), "r") as f_:
                    f_ = f_["metadata"]
                    keys = list(f_.keys())
                    for key in track(keys):
                        if key not in g.keys():
                            g.create_dataset(key, data=f_[key])
            except Exception as e:
                print(e)
                continue
    with File(str(root / "dataset.hdf5"), "r") as f:
        print(len(list(f["metadata"].keys())))
    with open(str(root / "actions.json"), "w") as f:
        json.dump(data, f)

