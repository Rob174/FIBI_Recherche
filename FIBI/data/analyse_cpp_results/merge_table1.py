import json

if __name__ == "__main__":
    with open("data/table1_visualize.json", "r") as f:
        table1 = json.load(f)
    with open("data/table1_impr_visualize.json", "r") as f:
        table1_impr = json.load(f)
    with open("data/table1_clust_visualize.json", "r") as f:
        table1_clust = json.load(f)
    with open("data/table1_clust_impr_visualize.json", "r") as f:
        table1_clust_impr = json.load(f)
    new_dico = {"grouped": [], "filtered": {}}
    # TSP
    for elem in table1["grouped"]:
        elem["INIT_CHOICE"] = "random"
        elem["problem"] = "tsp"
        elem["NUM_CLUST"] = -1
        new_dico["grouped"].append(elem)
    for elem in table1_impr["grouped"]:
        elem["INIT_CHOICE"] = "greedy"
        elem["problem"] = "tsp"
        elem["NUM_CLUST"] = -1
        new_dico["grouped"].append(elem)
    for k, v in table1["filtered"].items():
        L = []
        for elem in v:
            elem["INIT_CHOICE"] = "random"
            elem["problem"] = "tsp"
            elem["NUM_CLUST"] = -1
            L.append(elem)
        new_dico["filtered"][k + "_random" + "_tsp"] = L
    for k, v in table1_impr["filtered"].items():
        L = []
        for elem in v:
            elem["INIT_CHOICE"] = "greedy"
            elem["problem"] = "tsp"
            elem["NUM_CLUST"] = -1
            L.append(elem)
        new_dico["filtered"][k + "_greedy" + "_tsp"] = L
    # Clustering
    for elem in table1_clust["grouped"]:
        elem["INIT_CHOICE"] = "random"
        elem["problem"] = "clustering"
        new_dico["grouped"].append(elem)
    for elem in table1_clust_impr["grouped"]:
        elem["INIT_CHOICE"] = "greedy"
        elem["problem"] = "clustering"
        new_dico["grouped"].append(elem)
    for k, v in table1_clust["filtered"].items():
        L = []
        for elem in v:
            elem["INIT_CHOICE"] = "random"
            elem["problem"] = "clustering"
            L.append(elem)
        new_dico["filtered"][k + "_-1_random" + "_clustering"] = L
    for k, v in table1_clust_impr["filtered"].items():
        L = []
        for elem in v:
            elem["INIT_CHOICE"] = "greedy"
            elem["problem"] = "clustering"
            L.append(elem)
        new_dico["filtered"][k + "_-1_greedy" + "_clustering"] = L

    with open("data/table1_visualize_merged.json", "w") as f:
        json.dump(new_dico, f, indent=2)
