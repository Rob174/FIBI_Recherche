import shutil
import os
from pathlib import Path
import os, shutil

os.chdir(os.path.dirname(__file__))
for f in range(1, 11):
    shutil.rmtree(Path("./" + str(f)).resolve(), ignore_errors=True)
    os.mkdir(Path("./" + str(f)).resolve())
    for i,file in enumerate(['Max_SAT.exe','maxsat_benchmark.hdf5','maxsat_benchmark2.hdf5','maxsat_benchmark3.hdf5','aloise_benchmark.hdf5','franti_benchmark.hdf5','tsplib.hdf5']):
        dst = Path("./" + str(f) + "/"+file).resolve()
        if os.path.exists(str(dst)):
            os.remove(str(dst))
        if i == 0:
            shutil.copyfile(Path("./"+file).resolve(), dst)
        else:
            shutil.copyfile(Path("./backup_orig/"+file).resolve(), dst)
    try:
        dst = Path("./" + str(f) + "/"+"dataset.hdf5").resolve()
        os.remove(str(dst))
    except Exception:
        pass