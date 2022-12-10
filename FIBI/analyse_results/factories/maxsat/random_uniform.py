from FIBI.analyse_results.factories.maxsat.__init__ import *
def get_maxsat_uniformly_generated_visualizations(
    path_mapping: Path,
    pathes_hdf5: List[Path],
    out_folder: Path,
):
    stats = MainParser(Parser(path_mapping))
    query_to_path = DicoPathConverter(out_folder / "stats" / "dico_path_converter.json")
    Ldata = stats(
        pathes_hdf5,
        modifiers=[
            ConvertToInteger(fields=["NUM_VARIABLES", "NUM_CLAUSES"]),
            ModifierIntMapping(
                name="DATASET",
                mapping={0: "uniformly_generated", 1: "maxsat_benchmark"},
            ),
            ModifierOperation(
                dst_name="init_cost",
                operation=lambda d: np.round(d["init_cost"], decimals=5),
            ),
            ModifierOperation(
                dst_name="final_cost",
                operation=lambda d: np.round(d["final_cost"], decimals=5),
            ),
            ModifierIntMapping(name="IMPR", mapping={0: "RAND", 1: "GREEDY"}),
            ModifierIntMapping(name="FI_BI", mapping={0: "BI", 1: "FI"}),
            ModifierOperation(
                dst_name="duration", operation=lambda d: d["duration"] * 1e-6
            ),
            ModifierOperation(
                dst_name="NUM_VARIABLES", operation=lambda d: int(d["NUM_VARIABLES"])
            ),
            ModifierOperation(
                dst_name="NUM_CLAUSES", operation=lambda d: int(d["NUM_CLAUSES"])
            ),
            ModifierOperation(
                dst_name="ratio",
                operation=lambda d: d["final_cost"] / d["init_cost"],
            ),
        ],  # type: ignore
        filters=[],
    )
    # legend for the table
    with open(
        Path(".")
        / "data"
        / "analysis_results"
        / "data"
        / "legend_template_for_maximization.json",
        "r",
    ) as f:
        legend = json.load(f)
    fixed_attr = ["IMPR", "NUM_VARIABLES", "NUM_CLAUSES"]
    cmp_attr = ["FI_BI"]
    with open(
        Path(".") / "data" / "analysis_results" / "data" / "names_mapping.json", "r"
    ) as f:
        mappings_attr_names = json.load(f)
    metric_latex = lambda m: "$$\\frac{" + m + "}{initCost}$$"
    aggregators = [
        AverageFIBI(metric="ratio", name=metric_latex),
        AverageFIBIDiff(
            metric="ratio",
            attr_diff="FI_BI",
            diff_order=default_fibi_order(),
            name=metric_latex,
        ),
        InitDistrShape("ratio"),
        PValueEffectSize("ratio"),
    ]
    visualizations = [
        InitCostVariation(
            path_create(out_folder / "initDistr"), fixed_attrs=[*fixed_attr, *cmp_attr]
        ),
        PageMultiInstance(
            out_path=path_create(out_folder / "tables" / "concise.html"),
            fixed_attrs=fixed_attr,
            aggregators=aggregators,
            mappings_attr_names=mappings_attr_names,
            legend=Legend(legend),
            query_to_path=query_to_path,
            rows=["IMPR", "NUM_VARIABLES", "NUM_CLAUSES"],
            cols=[],
        ),
        PageInstance(
            path_create(out_folder / "stats"),
            fixed_attrs=fixed_attr,
            query_to_path=query_to_path,
        ),
    ]
    DataExtractor(visualizations=visualizations)(Ldata)
    query_to_path.save()
