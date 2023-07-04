from FIBI.analyse_results.factories.tsp.__init__ import *
from FIBI.analyse_results.factories.common import *
from FIBI.analyse_results.visualization.global_analysis.components.instance import open_tsplib, visualize_scatter


def get_tsplib_visualizations(
    pathes_hdf5: List[Path], out_folder: Path, mapping_inst: Path,
    test_group: Literal["signtest_ztest", "wilcoxon_ttest"] = "signtest_ztest",
):
    problem = "TSP"
    mapping_datasets = mapping_tsp_datasets()
    dataset = 1
    fixed_attr = ["PROBLEM","DATASET","IMPR", "NUM_TOWNS", "INSTANCE"]
    # mapping names index in hdf5 to names of the attributes
    with open(mapping_inst, "r") as f:
        mapping_id_name = {int(k): v for k, v in json.load(f).items()}
    additionnal_modifiers: List[AbstractModifier] = [
        ConvertToInteger(fields=["NUM_TOWNS"]),
        ModifierIntMapping(
            name="SEED_PROBLEM", new_name="INSTANCE", mapping=mapping_id_name
        ),
    ]
    Ldata = run_parser(
        problem=problem,
        pathes_data=pathes_hdf5,
        mapping_datasets=mapping_datasets,
        dataset=dataset,
        additional_modifiers=additionnal_modifiers
    )
    path_instance = out_folder / "instances"
    path_instance.mkdir(exist_ok=True,parents=True)
    run_data_extractor(
        dataset_name=mapping_datasets[dataset], 
        problem="TSP",
        out_folder=out_folder,
        fixed_attr=fixed_attr,
        Ldata=Ldata,
        test_group=test_group,
        instance_saver=(
            Path("data") / "algorithms_in" / "aloise_benchmark",
            path_instance,
            open_tsplib,
            visualize_scatter
        )
    )
