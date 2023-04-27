from cnfgen import GraphColoringFormula, PebblingFormula, RandomKCNF
from h5py import File
import networkx as nx
import time
from pathlib import Path
import random
import json
import os
from typing import List
from itertools import product
import shutil

def stats(F):
    vars = list(F.variables())
    clauses = list(F.clauses())
    n_vars = len(vars)
    n_clauses = len(clauses)
    return n_vars, n_clauses


def pebbling_generator():
    params_dicts = [{"n_nodes": n} for n in range(1000, 5001, 1000)]
    for args in params_dicts:
        G = nx.gnp_random_graph(
            args["n_nodes"], p=0.5, directed=True, seed=random.randint(0, 1000)
        )
        DAG = nx.DiGraph(
            [(u, v, {"weight": random.randint(1, 50)}) for (u, v) in G.edges() if u < v]
        )
        F = PebblingFormula(DAG)
        n_vars, n_clauses = stats(F)
        if 1000 <= n_vars <= 5000 and 1000 <= n_clauses <= 5000:
            yield n_vars, n_clauses, args, F.dimacs()


def kcnf_generator():

    params_dicts = [
        {"n_nodes": n, "n_edges": m, "clause_size": k}
        for n, m, k in product(
            range(1000, 5001, 1000), range(1000, 5001, 1000), range(2, 11, 2)
        )
    ]
    for args in params_dicts:
        F = RandomKCNF(args["clause_size"], args["n_nodes"], args["n_edges"])
        n_vars, n_clauses = stats(F)
        if 1000 <= n_vars <= 5000 and 1000 <= n_clauses <= 5000:
            yield n_vars, n_clauses, args, F.dimacs()


def generate(generator, formula_name: str = "pebbling_formula"):
    path_out = (
        Path(".") / "data" / "src_datasets" / "maxsat" / formula_name / "problems"
    )
    path_json = path_out.parent / "metadata.json"
    dico_metadata = []
    path_out.mkdir(parents=True, exist_ok=True)
    for i, (n_vars, n_clauses, args, dimacs) in enumerate(generator()):
        print(f"{n_vars} vars {n_clauses} clauses ")
        dico_metadata.append(
            {
                "name": f"{formula_name}_{i}",
                "id": i,
                "n_vars": n_vars,
                "n_clauses": n_clauses,
                **args,
            }
        )
        with open(path_out / f"{formula_name}_{i}.cnf", "w") as f:
            f.write(dimacs)
    with open(path_json, "w") as f:
        json.dump(str(dico_metadata), f, indent=4)


def to_hdf5(formula_name: str = "pebbling_formula"):
    folder_in = (
        Path(".") / "data" / "src_datasets" / "maxsat" / formula_name / "problems"
    )
    folder_in = folder_in.resolve()
    folder_out = Path(".") / "data" / "algorithms_in" / "maxsat" / formula_name
    folder_out.mkdir(parents=True, exist_ok=True)
    folder_out = folder_out.resolve()
    path_exe = Path(
        r"C:\Users\robin\Documents\Cours\Poly\Recherche\FIBI_Recherche\cpp_generation\parse_benchmark_maxsat\x64\Release\parse_benchmark_maxsat.exe"
    )
    for _, p in enumerate(folder_in.glob("*.cnf")):
        # We use the fact that the names are in the format "pebbling_formula_{counter}.cnf" to make the name <-> id mapping
        current_path = Path(".").resolve()
        i = p.name.split("_")[-1].split(".")[0]
        os.chdir(folder_out.resolve().as_posix())
        path_wcnf = p.resolve().as_posix()
        print(path_wcnf)
        res = os.system(f"{path_exe} {path_wcnf} {i} true") # true means that we use the unweighted version of the program
        if res != 0:
            raise Exception
        os.chdir(current_path.resolve().as_posix())


if __name__ == "__main__":
    # generate(kcnf_generator, formula_name="kcnf_formula")
    to_hdf5(formula_name="pebbling_formula")
