"""Main functions to load the system
```
 Main pipeline
───────────────

                            ┌
                            |  Metadata         xxxxxx
                            |                  xx    xxxx
                         ┌─►| SEED_PROBLEM     xx      xx
                         │  | ...              │xxxxxxxx│
                         │  | final_cost       │        │
                 \\\     │  └                  │        │
       ─────────── \\    │                     │  HDF5  │
      C++ program   //  Generates  ──────────► │  files │
       ─────────── //                          │        │
                 ///                           │        │
                                               xxx    xxx
                                                 xxxxxx
                  ┌─────────────────────────┐       │
                  │          MainParser     │       │
                  │   ( AbstractBaseParser) │     ┌─0─┐        AbstractModifier
                  │ parser for which problem│ ────┘ │ └──  (to add/modify columns)
                  └─────────────────────────┘       │
                                                    │ List[dict]       ┌───Comparison attributes
                  ┌───────────────────┐             │                  │
       ///        │                   ◄─────────────┘                  ├───Fixed attributes
     ///───────── │    CmptGlobStat   │                                │
    // HTML stats │ prepare dataframes◄──┐ ┌───────────────────────────▼────┐
    \\  ───────── │    for analysis   │  │ │        AbstractStatOp          │
     \\\\         │                   │  └─┤ to analyse groups of metadata  │
       \\         └────────────▲──────┘    │ and add new global informations│
                               │           │(statistical tests, average...) │
       ┌───────────────────────┴───┐       └─┬──────────────────────────────┘
       │       AbstractConsumer    │         │
       │  To make the visualization├─────────┘ often in pair but AbstractStatOp
       │        with the data      │           can be alone if only each group
       │     of the AbstractStatOp │           can be processed independantly
       └───────────────────────────┘
```
"""
import shutil, cProfile
from FIBI.analyse_results.__init__ import *
from FIBI.analyse_results.factories.clustering.benchmark_aloise import get_clustering_benchmark_aloise_visualizations
from FIBI.analyse_results.factories.clustering.benchmark_franti import get_clustering_franti_visualizations
from FIBI.analyse_results.factories.clustering.quad import get_clustering_uniform_visualizations
from FIBI.analyse_results.factories.clustering.quadNorm import get_clustering_uniform_norm_visualizations
from FIBI.analyse_results.factories.maxsat.benchmark import get_maxsat_instance_visualizations
from FIBI.analyse_results.factories.tsp.quad import get_tsp_uniform_visualizations
from FIBI.analyse_results.factories.tsp.tsplib import get_tsplib_visualizations
from FIBI.analyse_results.test.datasets import check_clustering, check_maxsat, check_tsp
from FIBI.analyse_results.utils.conversions import path_create
from FIBI.analyse_results.test.datasets import check_clustering, check_maxsat, check_tsp


def get_tsp(
    subproblem: Literal["uniform_points", "tsplib"], profile: Optional[bool] = False,
    test_group: Literal["signtest_ztest", "wilcoxon_ttest"] = "signtest_ztest",
):
    if profile:
        profiler = cProfile.Profile()
        profiler.enable()
    path_dataset = existing_path(Path(".") / "data" / "algorithms_out" / "tsp" / "dataset.txt")
    folder_profiler = ""
    if subproblem == "uniform_points":
        get_tsp_uniform_visualizations(
            pathes_hdf5=[
                path_dataset,
            ],
            out_folder=existing_path(
                path_create(Path(".") / "data" / "analysis_results" / "tsp" / "quad")
            ),
            test_group=test_group
        )
        folder_profiler = "quad"
        if profile:
            profiler.disable()
            profiler.dump_stats(
                path_create(
                    Path(".")
                    / "data"
                    / "analysis_results"
                    / "tsp"
                    / "quad"
                    / "profile.prof"
                )
            )
    elif subproblem == "tsplib":
        get_tsplib_visualizations(
            mapping_inst=(
                Path(".")
                / "data"
                / "algorithms_in"
                / "tsp"
                / "tsplib"
                / "mapping_tsplib_less_than_1000.json"
            ),
            pathes_hdf5=[
                path_dataset,
            ],
            out_folder=existing_path(
                path_create(Path(".") / "data" / "analysis_results" / "tsp" / "tsplib")
            ),
            test_group=test_group
        )
        folder_profiler = "tsplib"
    else:
        raise ValueError(f"Unknown subproblem {subproblem}")
    
    if profile:
        profiler.disable()
        profiler.dump_stats(
            path_create(
                Path(".")
                / "data"
                / "analysis_results"
                / "clustering"
                / folder_profiler
                / "profile.prof"
            )
        )





def get_clustering(
    subproblem: Literal["aloise_benchmark", "franti_benchmark", "uniform_points", "uniform_points_norm"],
    profile: Optional[bool] = False,
    test_group: Literal["signtest_ztest", "wilcoxon_ttest"] = "signtest_ztest",
):
    if profile:
        profiler = cProfile.Profile()
        profiler.enable()
    if subproblem == "aloise_benchmark":
        get_clustering_benchmark_aloise_visualizations(
            pathes_hdf5=[
                existing_path(
                    Path(".")
                    / "data"
                    / "algorithms_out"
                    / "clustering"
                    / "dataset2_impr0.txt",
                ),
                existing_path(
                    Path(".")
                    / "data"
                    / "algorithms_out"
                    / "clustering"
                    / "dataset2_impr1.txt"),
                existing_path(
                    Path(".")
                    / "data"
                    / "algorithms_out"
                    / "clustering"
                    / "dataset2_impr2.txt"),
                existing_path(
                    Path(".")
                    / "data"
                    / "algorithms_out"
                    / "clustering"
                    / "dataset2_impr3.txt"),
                existing_path(
                    Path(".")
                    / "data"
                    / "algorithms_out"
                    / "clustering"
                    / "dataset2_impr4.txt"),
                existing_path(
                    Path(".")
                    / "data"
                    / "algorithms_out"
                    / "clustering"
                    / "dataset2_impr5.txt"),
            ],
            mapping_inst=existing_path(
                Path(".")
                / "data"
                / "algorithms_in"
                / "clustering"
                / "benchmark_aloise"
                / "metadata.json"
            ),
            out_folder=existing_path(
                path_create(
                    Path(".")
                    / "data"
                    / "analysis_results"
                    / "clustering"
                    / "benchmark_aloise"
                )
            ),
            test_group=test_group
        )

        if profile:
            profiler.disable()
            profiler.dump_stats(
                path_create(
                    Path(".")
                    / "data"
                    / "analysis_results"
                    / "clustering"
                    / "benchmark_aloise"
                    / "profile.prof"
                )
            )
    elif subproblem == "uniform_points":
        get_clustering_uniform_visualizations(
            pathes_hdf5=[
                existing_path(
                    Path(".")
                    / "data"
                    / "algorithms_out"
                    / "clustering"
                    / "dataset0.txt"
                ),
            ],
            out_folder=existing_path(
                path_create(
                    Path(".") / "data" / "analysis_results" / "clustering" / "quad"
                )
            ),
            test_group=test_group
        )
        if profile:
            profiler.disable()
            profiler.dump_stats(
                path_create(
                    Path(".")
                    / "data"
                    / "analysis_results"
                    / "clustering"
                    / "quad"
                    / "profile.prof"
                )
            )
    elif subproblem == "uniform_points_norm":
        get_clustering_uniform_norm_visualizations(
            pathes_hdf5=[
                existing_path(
                    Path(".")
                    / "data"
                    / "algorithms_out"
                    / "clustering"
                    / "dataset3.txt"
                )
            ],
            out_folder=existing_path(
                path_create(
                    Path(".") / "data" / "analysis_results" / "clustering" / "quadNorm"
                )
            ),
            test_group=test_group
        )
        if profile:
            profiler.disable()
            profiler.dump_stats(
                path_create(
                    Path(".")
                    / "data"
                    / "analysis_results"
                    / "clustering"
                    / "quadNorm"
                    / "profile.prof"
                )
            )
    elif subproblem == "franti_benchmark":
        get_clustering_franti_visualizations(
            mapping_inst=existing_path(
                Path(".")
                / "data"
                / "algorithms_in"
                / "clustering"
                / "benchmark_franti"
                / "name_mapping.json"
            ),
            pathes_hdf5=[
                existing_path(
                    Path(".")
                    / "data"
                    / "algorithms_out"
                    / "clustering"
                    / "dataset1.txt"
                ),
            ],
            out_folder=existing_path(
                path_create(
                    Path(".")
                    / "data"
                    / "analysis_results"
                    / "clustering"
                    / "benchmark_franti"
                )
            ),
            test_group=test_group
        )
        if profile:
            profiler.disable() #type: ignore
            profiler.dump_stats(
                path_create(
                    Path(".")
                    / "data"
                    / "analysis_results"
                    / "clustering"
                    / "benchmark_franti"
                    / "profile.prof"
                )
            )

    else:
        raise ValueError(f"Unknown subproblem {subproblem}")




def existing_path(path: Path) -> Path:
    if not path.exists():
        raise Exception(f"Path {path} does not exist")
    return path


def get_maxsat_problem_visualization(
    subproblem: Literal["maxsat_evaluation_benchmark2021", "randomly_generated", "randomly_generated2", "randomly_generated3"],
    test_group: Literal["signtest_ztest", "wilcoxon_ttest"] = "signtest_ztest",
):
    if subproblem == "maxsat_evaluation_benchmark2021":
        get_maxsat_instance_visualizations(
            path_mapping=(
                Path(".")
                / "data"
                / "algorithms_out"
                / "maxsat"
                / "benchmark"
                / "mapping.json"
            ),
            pathes_hdf5=[
                existing_path(
                    Path(".")
                    / "data"
                    / "algorithms_out"
                    / "maxsat"
                    / "dataset.txt"
                )
            ],
            mapping_inst=existing_path(
                Path(".")
                / "data"
                / "algorithms_in"
                / "maxsat"
                / "benchmark"
                / "stats.json"
            ),
            out_folder=existing_path(
                path_create(
                    Path(".")
                    / "data"
                    / "analysis_results"
                    / "maxsat"
                    / "maxsat_evaluation_benchmark2021"
                )
            ),
            test_group=test_group
        )
    else:
        raise ValueError(f"Unknown subproblem {subproblem}")

def gather_latex(root: Path):
    dico_groups = {}
    for e in root.rglob('*.tex'):
        problem = e.parent.parent.parent.stem
        if problem not in dico_groups:
            dico_groups[problem] = []
        with open(e) as f:
            c = f.read()
        dico_groups[problem].append(c)
    for name, graphs_txts in dico_groups.items():
        with open(root/(name+".tex"),"w") as f:
            f.write("\n".join(graphs_txts))
        
    

if __name__ == "__main__":
    folder_missing = path_create(Path(".")
            / "data"
            / "algorithms_out"
            / "missing"
            )
    num_files = 2
    server = "rmoine@132.207.72.24:/home/rmoine/"
    # check_tsp(
    #     existing_path(Path(".") / "data" / "algorithms_out" / "tsp" / "dataset.txt")
    # )
    test_group: Literal["signtest_ztest", "wilcoxon_ttest"] = "signtest_ztest"   
    # os.system("dvc unprotect "+(Path("./data/analysis_results").as_posix()))
    # profiler = cProfile.Profile()
    # profiler.enable()
    # print("TSP")
    # print("quad")
    # get_tsp("uniform_points", False, test_group=test_group)
    # print("tsplib")
    # get_tsp("tsplib", False, test_group=test_group)
    # print("Clustering")
    # print("Aloise") 
    # get_clustering("aloise_benchmark", profile=False, test_group=test_group)
    # print("Quad")
    # get_clustering("uniform_points", profile=False, test_group=test_group)  
    # print("QuadNorm")
    # get_clustering("uniform_points_norm", profile=False, test_group=test_group)
    # print("Franti")
    # get_clustering("franti_benchmark", profile=False, test_group=test_group)
    gather_latex(Path("data/analysis_results/"))
    # profiler.disable() #type: ignore
    # profiler.dump_stats(
    #     path_create(
    #         Path(".")
    #         / "profile.prof"
    #     )
    # )
    # os.system("dvc add "+(Path("./data/analysis_results").as_posix()))
    # # print("MAXSAT")
    # check_maxsat(
    #     existing_path(
    #                 Path(".")
    #                 / "data"
    #                 / "algorithms_out"
    #                 / "clustering"
    #                 / "dataset.txt"
    #             )
    # )
    # # print("benchmark2021")
    # # get_maxsat_problem_visualization("maxsat_evaluation_benchmark2021")
    
    # check_clustering(
    #     existing_path(
    #         Path(".")
    #         / "data"
    #         / "algorithms_out"
    #         / "clustering"
    #         / "dataset.txt"
    #     ),
    #     folder_missing,
    #     num_files,
    #     server
    # )