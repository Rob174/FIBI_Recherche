from FIBI.analyse_results.factories.clustering.__init__ import *
from FIBI.analyse_results.factories.common import *

def get_clustering_uniform_norm_visualizations(
    pathes_hdf5: List[Path],
    out_folder: Path,
    test_group: Literal["signtest_ztest", "wilcoxon_ttest"] = "signtest_ztest",
):
    problem = "Clustering"
    dataset = 3
    fixed_attr = ["IMPR", "NUM_POINTS", "NUM_CLUST"]
    mapping_datasets = mapping_clustering_datasets()
    additionnal_modifiers: List[AbstractModifier] = [
            ConvertToInteger(fields=["NUM_POINTS", "NUM_CLUST"]),
    ]
    Ldata = run_parser(
        pathes_data=pathes_hdf5,
        mapping_datasets=mapping_datasets,
        dataset=dataset,
        additional_modifiers=additionnal_modifiers
    )
    run_data_extractor(
        dataset_name=mapping_datasets[dataset], 
        problem=problem,
        out_folder=out_folder,
        fixed_attr=fixed_attr,
        Ldata=Ldata,
        test_group=test_group,
        clust_col=True
    )
