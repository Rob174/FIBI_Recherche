from FIBI.analyse_results.visualization.global_analysis.components.averages import (
    AverageFIBIDiffTgt,
    MinimizationTgtDiff,
)
from FIBI.analyse_results.visualization.global_analysis.components.init_distr_shape import (
    TestUsed,
)
from FIBI.analyse_results.visualization.global_analysis.pie_chart import PieChart
from FIBI.analyse_results.visualization.global_analysis.pie_chart_distr import (
    PieChartDistrib,
)
from FIBI.analyse_results.factories.__init__ import *
from FIBI.analyse_results.parser.parser import JSONParser
from FIBI.analyse_results.visualization.statistical_tests import (
    tests_signtest_ztest,
    tests_wilcoxon_ttest,
)


def get_common_modifiers(mapping_dataset: Dict[int, str]):
    mapping_impr = {
        0: "RAND",
        1: "GREEDY randomized",
        2: "GREEDY",
        3: "GREEDY TOP3",
        4: "GREEDY TOP4",
        5: "GREEDY TOP5",
    }
    return [
        ConvertToInteger(fields=["NUM_TOWNS"]),
        ModifierIntMapping(
            name="IMPR",
            mapping=mapping_impr,
        ),
        ModifierIntMapping(name="DATASET", mapping=mapping_dataset),
        ModifierOperation(
            dst_name="init_meth",
            operation=lambda x: "random" if x["IMPR"] == "RAND" else "greedy",
        ),
        ModifierIntMapping(name="FI_BI", mapping={0: "BI", 1: "FI"}),
        ModifierOperation(
            dst_name="init_cost",
            operation=lambda d: np.round(d["init_cost"], decimals=5),
        ),
        ModifierOperation(
            dst_name="final_cost",
            operation=lambda d: np.round(d["final_cost"], decimals=5),
        ),
        ModifierOperation(
            dst_name="duration", operation=lambda d: d["duration"] * 1e-6
        ),
        ModifierOperation(
            dst_name="ratio",
            operation=lambda d: d["final_cost"] / d["init_cost"],
        ),
    ]


def run_parser(
    pathes_data: List[Path],
    mapping_datasets: Dict[int, str],
    dataset: int,
    additional_modifiers: List[AbstractModifier],
):
    stats = MainParser(JSONParser())
    Ldata = stats(
        pathes_data,
        modifiers=[
            *additional_modifiers,
            *get_common_modifiers(mapping_datasets),
        ],  # type: ignore
        filters=[
            FilterMetricsObserved(),
            FilterDuplicatedKeysPerGroup(["DATASET", "IMPR"]),
            FilterAttrValueInt(attr="DATASET", values_to_keep=[dataset]),
        ],
    )
    return Ldata


def run_data_extractor(
    dataset_name: str,
    problem: str,
    out_folder: Path,
    fixed_attr: List[str],
    Ldata: List[dict],
    minimization: Optional[bool] = True,
    test_group: Literal["signtest_ztest", "wilcoxon_ttest"] = "signtest_ztest",
):
    metric_latex = lambda m: "$$\\frac{" + m + "}{initCost}$$"
    if test_group == "signtest_ztest":
        tests = tests_signtest_ztest()
    elif test_group == "wilcoxon_ttest":
        tests = tests_wilcoxon_ttest()
    else:
        raise Exception
    query_to_path = DicoPathConverter(
        path_create(out_folder / "stats" / "dico_path_converter.json"), overwrite=True
    )
    with open(
        Path(".") / "data" / "analysis_results" / "data" / "names_mapping.json", "r"
    ) as f:
        mappings_attr_names = json.load(f)
    # legend for the table
    filename = "minimization" if minimization else "maximization"
    with open(
        Path(".")
        / "data"
        / "analysis_results"
        / "data"
        / f"legend_template_for_{filename}.json",
        "r",
    ) as f:
        legend = json.load(f)
    cmp_attr = ["FI_BI"]
    aggregators = [
        AverageFIBI(metric="final_cost", name=lambda m: "$$" + m + "$$"),
        AverageFIBI(metric="ratio", name=metric_latex),
        AverageFIBIDiffTgt(
            metric="ratio",
            attr_diff="FI_BI",
            diff_order=default_fibi_order(),
            name=metric_latex,
            tgt_vals=MinimizationTgtDiff,
        ),
        TestUsed(metric="ratio",**tests),
        PValueEffectSize("ratio", **tests),
    ]
    visualizations = [
        InitCostVariation(
            path_create(out_folder / "initDistr"), fixed_attrs=[*fixed_attr, *cmp_attr]
        ),
        PieChart(
            dataset=dataset_name,
            problem=problem,
            out_path=path_create(out_folder / "cases"),
            fixed_attrs=fixed_attr,
            aggregators=aggregators,
            mappings_attr_names=mappings_attr_names,
            legend=Legend(legend),
            query_to_path=query_to_path,
            rows=fixed_attr,
            cols=[],
        ),
        PieChartDistrib(
            out_path=path_create(out_folder / "distributions"),
            fixed_attrs=fixed_attr,
            aggregators=aggregators,
            mappings_attr_names=mappings_attr_names,
            legend=Legend(legend),
            query_to_path=query_to_path,
            rows=fixed_attr,
            cols=[],
        ),
        PageMultiInstance(
            out_path=path_create(out_folder / "tables" / "concise.html"),
            fixed_attrs=fixed_attr,
            aggregators=aggregators,
            mappings_attr_names=mappings_attr_names,
            legend=Legend(legend),
            query_to_path=query_to_path,
            rows=fixed_attr[:2],
            cols=[],
        ),
        PageInstance(
            path_create(out_folder / "stats"),
            fixed_attrs=fixed_attr,
            query_to_path=query_to_path,
            tests_used=tests
        ),
    ]
    DataExtractor(visualizations=visualizations)(Ldata)
    query_to_path.save()


def mapping_tsp_datasets():
    return {0: "Uniformly distr. towns", 1: "TSPLIB"}


def mapping_clustering_datasets():
    return {
        0: "Uniformly distr. points",
        1: "Franti benchmark",
        2: "Aloise benchmark",
        3: "Normaly distributed points",
    }
