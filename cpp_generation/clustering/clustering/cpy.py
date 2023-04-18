import shutil
import os
from pathlib import Path

for f in range(0, 10):
    # dst = Path("./" + str(f) + "/clustering.exe").resolve()
    dataset = Path("./" + f"/dataset{f}.hdf5").resolve()
    tsplib = Path("./" + f"/tsplib{f}.hdf5").resolve()
    benchmark = Path("./" + f"/benchmark{f}.hdf5").resolve()
    # if os.path.exists(str(dst)):
        # os.remove(str(dst))
    if os.path.exists(str(dataset)):
        os.remove(str(dataset))
    if os.path.exists(str(tsplib)):
        os.remove(str(tsplib))
    if os.path.exists(str(benchmark)):
        os.remove(str(benchmark))
    # shutil.copyfile(Path("./clustering.exe").resolve(), dst)
    shutil.copyfile(Path("./backup_orig/dataset.hdf5").resolve(), str(dataset))
    shutil.copyfile(Path("./backup_orig/tsplib.hdf5").resolve(), str(tsplib))
    shutil.copyfile(Path("./backup_orig/benchmark.hdf5").resolve(), str(benchmark))
