import abc
import plotly.express
from typing import *
from typing import Literal
from FIBI.analyse_results.visualization.types import *
from FIBI.analyse_results.visualization.types import FieldType, Literal
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.ensemble import RandomForestClassifier
from sklearn import preprocessing
from sklearn.tree import plot_tree
import plotly.express as px
class AbstractComponentObserver(abc.ABC):
    def on_table_ready(self, keys: dict, field: FieldType, table: dict):
        pass
    
    def on_statistical_test_ready(self, keys: dict, test_name: TestName, test_res: dict, chosen: bool):
        pass
    
    def on_distribution_found(self, keys: dict, dist: Literal["ng", "gh", "g"]):
        pass
    
    def on_case_found(self, keys: dict, case: CaseType):
        pass
    
class GenericComponent:
    def __init__(self, observers: List[AbstractComponentObserver]):
        self.observers = observers

    def on_table_ready(self, keys: dict, field: FieldType, table: dict):
        for o in self.observers:
            o.on_table_ready(keys,field,table)
    
    def on_statistical_test_ready(self, keys: dict, test_name: TestName, test_res: dict, chosen: bool):
        for o in self.observers:
            o.on_statistical_test_ready(keys,test_name,test_res,chosen)
        
    def on_distribution_found(self, keys: dict, dist: Literal["ng", "gh", "g"]):
        for o in self.observers:
            o.on_statistical_test_ready(keys,dist)
        
    def on_case_found(self, keys: dict, case: CaseType):
        for o in self.observers:
            o.on_case_found(keys,case)

class CaseExplainer(AbstractComponentObserver):
    def __init__(self, path_out: Path) -> None:
        super().__init__()
        self.data = {}
        self.path_out = path_out
    def keys_to_str(self, keys: dict) -> str:
        return str(keys)[1:-1].replace(",","__").replace("'",'--').replace('"',"--")
    def str_to_keys(self, id: str) -> dict:
        return eval("{"+id.replace("__",",").replace('--',"'").replace("--",'"')+"}")
    def add_in_buffer(self, keys: dict, data: dict):
        if self.keys_to_str(keys) not in self.data:
            self.data[self.keys_to_str(keys)] = {}
        self.data[self.keys_to_str(keys)] = {**self.data[self.keys_to_str(keys)], **data}
    def on_table_ready(self, keys: dict, field: FieldType, table: dict):
        if field == "duration":
            return
        d = {}
        for impr,dico in table.items():
            for k,v in dico.items():
                if k == "count":
                    continue
                d[field+'--'+impr+"--"+k] = v
        self.add_in_buffer(keys,d)
    def on_statistical_test_ready(self, keys: dict, test_name: TestName, test_res: dict, chosen: bool):
        d = {}
        d['test_name'] = test_name
        if test_name == "WilcoxonTest":
            # we add the fields n_zeros, R+, R-, pvalue, effect_size
            d['n_zero'] = float(test_res['n_zero'])
            d['R+'] = float(test_res['R+'])
            d['R-'] = float(test_res['R-'])
        elif test_name == "ZTest":
            # we add the fields sigma_norm, variance, pvalue, effect_size
            d['sigma_norm'] = float(test_res['sigma_norm'])
            d['variance'] = float(test_res['variance'])
        d[test_name+"_"+'pvalue'] = float(test_res['pvalue'])
        d[test_name+"_"+'effect_size'] = float(test_res['effect_size'])
        if chosen:
            d['chosen'] = test_name
        self.add_in_buffer(keys,d)
    def on_distribution_found(self, keys: dict, dist: Literal['ng', 'gh', 'g']):
        # we just add the distribution
        self.add_in_buffer(keys, {"distribution":dist})
    def on_case_found(self, keys: dict, case: CaseType):
        self.add_in_buffer(keys, {"case":case})
    
    def explain(self, split_attr: Optional[List] = None):
        if split_attr is None:
            split_attr = []
        # convert self.data into list of dict including the keys inside the dict
        data = {}
        def get_explain_key(dico: dict):
            if len(split_attr) == 0:
                return ""
            else:
                return str(dico[split_attr[0]])
        for k,v in self.data.items():
            key = get_explain_key(self.str_to_keys(k))
            if key not in data:
                data[key] = []
            data[key].append({
                **self.str_to_keys(k),
                **v
            })
        
        for dataset_id in data:
            df = pd.DataFrame(data[dataset_id])
            # conversion in onehot
            poss_cases = ['A1','A2','B1','B2','C1','C2']
            map_cases = {k:i for i,k in enumerate(poss_cases)}
            cases = df['case'].map(lambda x: map_cases[x])
            df = df[[c for c in df.columns if c != 'case']]
            object_cols = df.select_dtypes(include=['object']).columns
            df_encoded = pd.get_dummies(df, columns=object_cols)
            float_cols = df_encoded.select_dtypes(include=['int']).columns
            df_encoded[float_cols] = df_encoded[float_cols].astype(float)
            y = cases
            # delte all cols with nan
            df_encoded = df_encoded.dropna(axis=1)
            # delete infinities
            df_encoded = df_encoded.replace([np.inf, -np.inf], np.nan)
            df_encoded = df_encoded.dropna(axis=1)
            scaler = preprocessing.StandardScaler()
            X_scaled = scaler.fit_transform(df_encoded)
            # for each of the classes, we fit the model and plot the feature importance
            for c in cases.unique():
                classifier = RandomForestClassifier(random_state=42)
                plt.clf()
                plt.figure(figsize=(50, 30))
                y_c = (y == c)
                classifier.fit(X_scaled, y_c)
                feature_importances = classifier.feature_importances_
                importance_df = pd.DataFrame({'Feature': df_encoded.columns, 'Importance': feature_importances})
                importance_df.sort_values(by='Importance', ascending=False, inplace=True)
                # Plot histogram of feature importances
                plt.bar(importance_df['Feature'], importance_df['Importance'])
                plt.xticks(rotation=90)
                plt.xlabel('Features')
                plt.ylabel('Importance')
                plt.title('Feature Importance')
                plt.tight_layout()
                plt.savefig(self.path_out / f"{dataset_id}_{poss_cases[c]}_importance.png")
                # Plot a typical decision tree
                success = False
                i = 0
                while not success:
                    try:
                        plt.clf()
                        plt.figure(figsize=(50, 30))
                        plot_tree(classifier.estimators_[i], feature_names=df.columns, filled=True)
                        plt.savefig(self.path_out / f"{dataset_id}_{poss_cases[c]}_tree_example.png")
                        success = True
                    except Exception as e:
                        i += 1
        