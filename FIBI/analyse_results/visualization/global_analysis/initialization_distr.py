from FIBI.analyse_results.data_extractor.data_extractor import MultiInstanceVisualization
from FIBI.analyse_results.visualization.global_analysis.__init__ import *
from FIBI.analyse_results.visualization.types import AggregatedData, BufferDataAggr

class InitCostVariation(MultiInstanceVisualization):
    def __init__(self, out_folder: Path, fixed_attrs: List[str]):
        self.fixed_attrs = fixed_attrs
        super(InitCostVariation, self).__init__(fixed_attrs,[])
        self.cmp_attrs = []
        self.out_folder = out_folder
        self.buffer = []
            
    def on_new_data(
        self, keys: dict, dfs: List[DfExtract]
    ): 
        assert len(dfs) == 1
        elems: List[AggregatedData] = []
        elems.append(AggregatedData(type='init_distr_std',data=np.std(dfs[0]["df"]['init_cost'])/np.mean(dfs[0]["df"]['init_cost']),name='',classes=[],query=keys))
        self.buffer.append(BufferDataAggr(query=keys,fields=elems))
    def on_end(self):
        query_keys = list(self.buffer[0]['query'].keys())
        df = pd.DataFrame([{**d['query'],'σ normalized': d['fields'][0]['data']} for d in self.buffer])
        fig = px.strip(
            y=df["σ normalized"],
            stripmode="overlay",
            width=600,
            height=500,
            hover_name=df[query_keys].astype(str).agg(", ".join, axis=1),
        )
        fig.add_trace(go.Box(y=df['σ normalized'], boxpoints=False, name=""))
        fig.update_layout(showlegend=False)
        fig.update_traces(marker=dict(color="red"))
        fig.update_layout(yaxis_title=r"σ normalized")
        folder = self.out_folder
        (folder).mkdir(parents=True, exist_ok=True)
        
        path_img = folder / "initDistribution.png"
        path_html = folder / "initDistribution.html"
        os.system("dvc remove "+path_html.resolve().as_posix()+".dvc")
        os.system("dvc remove "+path_img.resolve().as_posix()+".dvc")
        fig.write_html(path_html)
        fig.write_image(path_img)
        os.system("dvc add "+path_html.resolve().as_posix())
        os.system("dvc add "+path_img.resolve().as_posix())
        
        