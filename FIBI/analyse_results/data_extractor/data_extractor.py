"""Module to compute all statstics backend necessary for the visualization."""
from FIBI.analyse_results.data_extractor.__init__ import *
from FIBI.analyse_results.utils.conversions import dico_to_query
from FIBI.analyse_results.utils.progressBars import DefaultProgressBar
from FIBI.analyse_results.data_extractor.types import DfExtract


class DataExtractorObserver(abc.ABC):
    """Make the visualizations thanks to the data provided by the DataExtractor"""

    id = 0

    def __init__(
        self,
        fixed_attrs: Optional[List[str]] = None,
        cmp_attrs: Optional[List[str]] = None,
    ):
        super().__init__()
        if fixed_attrs is None:
            fixed_attrs = []
        if cmp_attrs is None:
            cmp_attrs = []
        self.fixed_attrs = fixed_attrs
        self.cmp_attrs = cmp_attrs
        self.id = DataExtractorObserver.id
        DataExtractorObserver.id += 1

    def on_new_data(self, keys: dict, dfs: List[DfExtract]):
        pass

    def on_end(self):
        pass


class DataExtractorObservable(abc.ABC):
    """Extract data from the database and call the observers to make the visualizations."""

    def __init__(self, observers: Optional[List[DataExtractorObserver]]) -> None:
        super().__init__()
        if observers is None:
            observers = []
        self.observers = observers

    def on_new_data(self, keys: dict, dfs: List[DfExtract]):
        for o in self.observers:
            o.on_new_data(keys, dfs)

    def on_end(self):
        for o in self.observers:
            o.on_end()


class SingleInstanceVisualization(DataExtractorObserver):
    """Visualization for a single instance of the experiment. Typically a view of a combination of attributes statistics."""

    def __init__(
        self,
        fixed_attr: Optional[List[str]] = None,
        cmp_atr: Optional[List[str]] = None,
    ) -> None:
        super(SingleInstanceVisualization, self).__init__(fixed_attr, cmp_atr)


class MultiInstanceVisualization(DataExtractorObserver):
    """Visualization to visualize multiple instances: typical usage: overall statistics on a problem"""

    def __init__(
        self,
        fixed_attr: Optional[List[str]] = None,
        cmp_atr: Optional[List[str]] = None,
    ) -> None:
        super(MultiInstanceVisualization, self).__init__(fixed_attr, cmp_atr)
        self.buffer = []

    def on_new_data(self, keys: dict, dfs: List[DfExtract]):
        self.buffer.append((keys, dfs))


class DataExtractor(DataExtractorObservable):
    """
    Used to compare statistics aggregated accross key_attrs bu with simultaneous access to all possibilities of attributes in comp_keys

    # Example of usage:
    """

    def __init__(self, visualizations: List[DataExtractorObserver]):
        """ """
        self.visualizations = visualizations
        super(DataExtractor, self).__init__(visualizations)

    def __make_df_dict(self, keys: dict, src_df: pd.DataFrame) -> DfExtract:
        df = src_df.query(dico_to_query(keys))  # type: ignore
        return {
            "dico_keys": keys,  # type: ignore
            "df": df,
        }
    def generator_operations_dataframes(
        self,
        remaining_ops: List[DataExtractorObserver],
        curr_req: Dict[str, Union[float, int, str]],
        df: pd.DataFrame,
    ) -> Iterable[
        Tuple[DataExtractorObserver, Dict[str, Union[float, int, str]], pd.DataFrame]
    ]:
        """Guarantees that for remaining ops request at least all of the values and keys specified in curr_req"""
        # find the attribute that is required for the biggest number of operations
        # filter the operations that have already the appropriated request and daatframe
        occ_attrs = {}
        elems_to_rmv = set({})
        for op in remaining_ops:
            all_in_request = True
            for attr in op.fixed_attrs:
                if attr not in curr_req:
                    all_in_request = False
                    if attr not in occ_attrs:
                        occ_attrs[attr] = 0
                    occ_attrs[attr] += 1
            # In case the dataframe has already been fully filtered, we yield the element and remove it
            if all_in_request:
                elems_to_rmv.add(op.id)
                yield (op, curr_req, df)
        remaining_ops = [o for o in remaining_ops if o.id not in elems_to_rmv]
        if len(remaining_ops) == 0:
            return
        attr_to_dispatch = max(
            [[k, v] for k, v in occ_attrs.items()], key=lambda x: x[1]
        )[0]
        # for each attribute values required, partially filter the dataframe and delegate the task of further filtering to the next _gen_ops_dataframes calls
        new_remaining_ops_without_attr = []
        for v in df[attr_to_dispatch].unique():
            new_df = df.query(dico_to_query({attr_to_dispatch: v}))
            new_remaining_ops_with_attr = []
            for op in remaining_ops:
                if (
                    attr_to_dispatch not in op.fixed_attrs
                    and op.id not in new_remaining_ops_without_attr
                ):
                    new_remaining_ops_without_attr.append(op.id)
                else:
                    new_remaining_ops_with_attr.append(op.id)
            remaining_ops_with = [
                o for o in remaining_ops if o.id in new_remaining_ops_with_attr
            ]
            yield from self.generator_operations_dataframes(
                remaining_ops_with, {**curr_req, attr_to_dispatch: v}, new_df
            )
        remaining_ops_without = [
            o for o in remaining_ops if o.id in new_remaining_ops_without_attr
        ]
        yield from self.generator_operations_dataframes(
            remaining_ops_without, curr_req, df
        )

    def __call__(self, L: List[dict]):
        return self.call(L)

    def call(self, L: List[dict]):
        """Extract the relevant dataframes from the list of dictionaries and call the observers to build the visualizations

        #Args:
            L: list of dictionaries containing the dataframes to be analysed. Provided by the module parser
        """
        df = pd.DataFrame(L)
        n_elems = 0
        for op, query_dict, new_df in self.generator_operations_dataframes(
            self.visualizations, {}, df
        ):
            n_elems += 1
        with DefaultProgressBar() as progress:
            progress.set_total_steps(n_elems)
            # Iterate over all of the visualizations todo with all possible combinations of keys required
            for op, query_dict, new_df in self.generator_operations_dataframes(
                self.visualizations, {}, df
            ):
                dfs: List[DfExtract] = []
                # If we want only a instance wise data, we can directly append the dataframe
                if len(op.cmp_attrs) == 0:
                    dfs.append(self.__make_df_dict(query_dict, new_df))
                # If we want to compare multiple instances, depending on one or multiple other attributes (ex: FI_BI)
                else:
                    df_extr_keys = new_df[op.cmp_attrs].drop_duplicates().to_dict(orient="records")  # type: ignore
                    for keys_extr in df_extr_keys:
                        df_filtered = new_df.query(dico_to_query(keys_extr))  # type: ignore
                        dfs.append(
                            self.__make_df_dict(
                                {**query_dict, **keys_extr}, df_filtered
                            )
                        )
                op.on_new_data(query_dict, dfs)
                progress.next(dico_to_query(query_dict, ", "))  # type: ignore
        self.on_end()
