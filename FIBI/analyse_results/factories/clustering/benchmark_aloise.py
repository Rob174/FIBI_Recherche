from FIBI.analyse_results.factories.clustering.__init__ import *
def get_clustering_benchmark_aloise_visualizations(
    path_mapping: Path,
    pathes_hdf5: List[Path],
    mapping_inst: Path,
    out_folder: Path,
):
    stats = MainParser(Parser(path_mapping))
    # mapping names index in hdf5 to names of the attributes
    with open(mapping_inst, "r") as f:
        mapping = json.load(f)
    mapping_id_name = {v["ID"]: k for k, v in mapping.items()}
    query_to_path = DicoPathConverter(
        path_create(out_folder / "stats" / "dico_path_converter.json"), overwrite=True
    )

    Ldata = stats(
        pathes_hdf5,
        modifiers=[
            ConvertToInteger(fields=["NUM_POINTS", "NUM_CLUST"]),
            ModifierIntMapping(
                name="DATASET",
                mapping={
                    0: "uniform_points",
                    4: "franti_benchmark",
                    5: "aloise_benchmark",
                },
            ),
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
            ModifierIntMapping(name="IMPR", mapping={0: "RAND", 2: "GREEDY"}),
            ModifierIntMapping(name="FI_BI", mapping={0: "BI", 1: "FI"}),
            ModifierOperation(
                dst_name="duration", operation=lambda d: d["duration"] * 1e-6
            ),
            ModifierOperation(
                dst_name="ratio",
                operation=lambda d: d["final_cost"] / d["init_cost"],
            ),
        ],  # type: ignore
        filters=[
            FilterAttrValueInt(attr="IMPR", values_to_keep=[0, 2]),
            FilterAttrValueInt(attr="FI_BI", values_to_keep=[0, 1]),
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
    fixed_attr = ["IMPR", "NUM_POINTS", "INSTANCE", "NUM_CLUST"]
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
            rows=fixed_attr[:3],
            cols=fixed_attr[3:],
        ),
        PageInstance(
            path_create(out_folder / "stats"),
            fixed_attrs=fixed_attr,
            query_to_path=query_to_path,
        ),
    ]
    DataExtractor(visualizations=visualizations)(Ldata)
    query_to_path.save()
