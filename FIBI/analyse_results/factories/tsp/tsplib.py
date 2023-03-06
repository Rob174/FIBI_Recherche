from FIBI.analyse_results.factories.tsp.__init__ import *
from FIBI.analyse_results.visualization.global_analysis.components.averages import AverageFIBIDiffTgt, MinimizationTgtDiff
from FIBI.analyse_results.visualization.global_analysis.components.init_distr_shape import TestUsed
from FIBI.analyse_results.visualization.global_analysis.pie_chart import PieChart
from FIBI.analyse_results.visualization.global_analysis.pie_chart_distr import PieChartDistrib
def get_tsplib_visualizations(
    path_mapping: Path, pathes_hdf5: List[Path], out_folder: Path, mapping_inst: Path
):
    # mapping names index in hdf5 to names of the attributes
    with open(mapping_inst, "r") as f:
        mapping_id_name = {int(k): v for k, v in json.load(f).items()}
    stats = MainParser(Parser(path_mapping))
    # mapping names index in hdf5 to names of the attributes=
    query_to_path = DicoPathConverter(
        path_create(out_folder / "stats" / "dico_path_converter.json"), overwrite=True
    )
    mapping_filename = {
        "tsplib_rand": "RAND",
        "tsplib_greedy_stoch_top3": "GREEDY TOP3",
        "tsplib_greedy_stoch_top4": "GREEDY TOP4",
        "tsplib_greedy_stoch_top5": "GREEDY TOP5",
    }
    Ldata = stats(
        pathes_hdf5,
        modifiers=[
            ConvertToInteger(fields=["NUM_TOWNS"]),
            ModifierOperation(
                dst_name="IMPR",
                operation=lambda d: mapping_filename[d["filename"]],
            ),
            Glob_SEEEDMaker(
                variating_params=["IMPR", "SEED_PROBLEM", "FI_BI", "SEED_ASSIGN"]
            ),
            ModifierOperation(dst_name="DATASET", operation=lambda x: "TSPLIB"),
            ModifierOperation(dst_name="init_meth", operation=lambda x: 'random' if x['IMPR'] == "RAND" else 'greedy'),
            ModifierIntMapping(name="FI_BI", mapping={0: "BI", 1: "FI"}),
            ModifierIntMapping(
                name="SEED_PROBLEM", new_name="INSTANCE", mapping=mapping_id_name
            ),
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
        ],  # type: ignore
        filters=[
            FilterDuplicatedKeys(),
            FilterAttrValueInt(attr="FI_BI", values_to_keep=[0, 1]),
            FilterAttrValueInt(attr="IT_ORDER", values_to_keep=[1]),
        ],
    )
    # legend for the table
    with open(
        Path(".")
        / "data"
        / "analysis_results"
        / "data"
        / "legend_template_for_minimization.json",
        "r",
    ) as f:
        legend = json.load(f)
    fixed_attr = ["IMPR", "NUM_TOWNS", "INSTANCE"]
    cmp_attr = ["FI_BI"]
    with open(
        Path(".") / "data" / "analysis_results" / "data" / "names_mapping.json", "r"
    ) as f:
        mappings_attr_names = json.load(f)
    metric_latex = lambda m: "$$\\frac{" + m + "}{initCost}$$"
    aggregators = [
        AverageFIBI(metric="final_cost", name=lambda m:"$$" + m + "$$"),
        AverageFIBI(metric="ratio", name=metric_latex),
        AverageFIBIDiffTgt(
            metric="ratio",
            attr_diff="FI_BI",
            diff_order=default_fibi_order(),
            name=metric_latex,
            tgt_vals=MinimizationTgtDiff
        ),
        TestUsed("ratio"),
        PValueEffectSize("ratio"),
    ]
    visualizations = [
        InitCostVariation(
            path_create(out_folder / "initDistr"), fixed_attrs=[*fixed_attr, *cmp_attr]
        ),
        PieChart(out_path=path_create(out_folder / "cases"),
            fixed_attrs=fixed_attr,
            aggregators=aggregators,
            mappings_attr_names=mappings_attr_names,
            legend=Legend(legend),
            query_to_path=query_to_path,
            rows=fixed_attr,
            cols=[]),
        PieChartDistrib(out_path=path_create(out_folder / "distributions"),
            fixed_attrs=fixed_attr,
            aggregators=aggregators,
            mappings_attr_names=mappings_attr_names,
            legend=Legend(legend),
            query_to_path=query_to_path,
            rows=fixed_attr,
            cols=[]),
        PageMultiInstance(
            out_path=path_create(out_folder / "tables" / "concise.html"),
            fixed_attrs=fixed_attr,
            aggregators=aggregators,
            mappings_attr_names=mappings_attr_names,
            legend=Legend(legend),
            query_to_path=query_to_path,
            rows=fixed_attr,
            cols=[],
        ),
        PageInstance(
            path_create(out_folder / "stats"),
            fixed_attrs=fixed_attr,
            query_to_path=query_to_path,
        )
        
    ]
    DataExtractor(visualizations=visualizations)(Ldata)
    query_to_path.save()

