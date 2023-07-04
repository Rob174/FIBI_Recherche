from FIBI.analyse_results.factories.common import mapping_tsp_datasets, run_data_extractor, run_parser
from FIBI.analyse_results.factories.tsp.__init__ import *
from FIBI.analyse_results.parser.parser import JSONParser
from FIBI.analyse_results.visualization.global_analysis.components.averages import AverageFIBIDiffTgt, MinimizationTgtDiff
from FIBI.analyse_results.visualization.global_analysis.components.init_distr_shape import TestUsed
from FIBI.analyse_results.visualization.global_analysis.pie_chart import PieChart
from FIBI.analyse_results.visualization.global_analysis.pie_chart_distr import PieChartDistrib
from FIBI.analyse_results.visualization.statistical_tests import SignTest, ZTest

def get_tsp_uniform_visualizations(
    pathes_hdf5: List[Path], out_folder: Path,
    test_group: Literal["signtest_ztest", "wilcoxon_ttest"] = "signtest_ztest",
):
    problem = "TSP"
    mapping_datasets = mapping_tsp_datasets()
    dataset = 0
    fixed_attr = ["PROBLEM","DATASET","IMPR", "NUM_TOWNS"]
    additionnal_modifiers: List[AbstractModifier] = [
        ConvertToInteger(fields=["NUM_TOWNS"]),
    ]
    Ldata = run_parser(
        problem=problem,
        pathes_data=pathes_hdf5,
        mapping_datasets=mapping_datasets,
        dataset=dataset,
        additional_modifiers=additionnal_modifiers
    )
    run_data_extractor(
        dataset_name=mapping_datasets[dataset], 
        problem="TSP",
        out_folder=out_folder,
        fixed_attr=fixed_attr,
        Ldata=Ldata,
        test_group=test_group
    )
