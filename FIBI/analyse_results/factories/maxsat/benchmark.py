from FIBI.analyse_results.factories.maxsat.__init__ import *
from FIBI.analyse_results.factories.common import *
def get_maxsat_instance_visualizations(
    pathes_hdf5: List[Path],
    mapping_inst: Path,
    out_folder: Path,
    test_group: Literal["signtest_ztest", "wilcoxon_ttest"] = "signtest_ztest",
):
    
    problem = "Weighted-MAXSAT"
    dataset = 1
    fixed_attr = ["IMPR", "INSTANCE", "NUM_VARIABLES", "NUM_CLAUSES"]
    mapping_datasets = mapping_maxsat_datasets()
    # mapping names index in hdf5 to names of the attributes
    with open(mapping_inst, "r") as f:
        mapping = json.load(f)
    mapping_id_name = {int(e["id"]): "/".join(e["name"].split("--")[1:]) for e in mapping}
    additionnal_modifiers: List[AbstractModifier] = [
            ConvertToInteger(fields=["NUM_VARIABLES", "NUM_CLAUSES"]),
            ModifierIntMapping(
                name="SEED_PROBLEM", new_name="INSTANCE", mapping=mapping_id_name
            ),
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
        minimization=False
    )
