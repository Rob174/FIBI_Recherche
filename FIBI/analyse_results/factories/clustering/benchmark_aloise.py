from FIBI.analyse_results.factories.clustering.__init__ import *
from FIBI.analyse_results.factories.common import *
from FIBI.analyse_results.visualization.global_analysis.components.instance import open_clustering, visualize_scatter

def get_clustering_benchmark_aloise_visualizations(
    pathes_hdf5: List[Path],
    out_folder: Path,
    mapping_inst: Path,
    test_group: Literal["signtest_ztest", "wilcoxon_ttest"] = "signtest_ztest",
):
    problem = "Clustering"
    dataset = 2
    fixed_attr = ["PROBLEM","DATASET","IMPR", "NUM_POINTS", "NUM_CLUST", "SEED_PROBLEM", "INSTANCE"]
    mapping_datasets = mapping_clustering_datasets()
    # mapping names index in hdf5 to names of the attributes
    with open(mapping_inst, "r") as f:
        mapping = json.load(f)
    mapping_id_name = {v["ID"]: k for k, v in mapping.items()}
    additionnal_modifiers: List[AbstractModifier] = [
            ConvertToInteger(fields=["NUM_POINTS", "NUM_CLUST", "SEED_PROBLEM"]),
            ModifierIntMapping(
                name="SEED_PROBLEM", new_name="INSTANCE", mapping=mapping_id_name
            ),
    ]
    additionnal_filters = [
        FilterAttrValueInt("SEED_PROBLEM",values_to_keep=[0,3,5,6,8,11,13,14,15,16,18,10,17],after=True)
    ]
    Ldata = run_parser(
        problem=problem,
        pathes_data=pathes_hdf5,
        mapping_datasets=mapping_datasets,
        dataset=dataset,
        additional_modifiers=additionnal_modifiers,
        additionnal_filters=additionnal_filters
    )
    path_instance = out_folder / "instances"
    path_instance.mkdir(exist_ok=True,parents=True)
    run_data_extractor(
        dataset_name=mapping_datasets[dataset], 
        problem=problem,
        out_folder=out_folder,
        fixed_attr=fixed_attr,
        Ldata=Ldata,
        test_group=test_group,
        clust_col=False,
        instance_saver=(
            Path("data") / "algorithms_in" / "folder" / "aloise_benchmark",
            path_instance,
            open_clustering,
            visualize_scatter
        )
    )
