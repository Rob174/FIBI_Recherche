import requests
import json
import re

if __name__ == "__main__":
    with open("data/tsplib_urls.json", "r") as f:
        urls = json.load(f)
    problems = {}
    # NB : pattern for integer, float or float written in scientific notation
    # WARNING : order of condition is important to match if possible scientific notation and then other possibilities
    pattern_number = r"-?(?:\d+\.\d+[eE]?[+-]?\d*|\d+)"
    regexes = {
        "data": rf"^{pattern_number} +({pattern_number}) +({pattern_number})$",
        "dataOpt": r"^({pattern_number})$",
        "type": r"^TYPE : (.*)$",
        "edge_weight_type": r"^EDGE_WEIGHT_TYPE : (.*)$",
        "comment": r"^COMMENT : (.*)$",
    }
    for url in urls:
        problem_name = url.split("/")[-1]
        data = requests.get(url).content
        problems[problem_name] = {"data": []}
        for l in data.decode("utf-8").split("\n"):
            l = l.strip()
            if l.strip() == "":
                continue
            for regex in regexes:
                match = re.search(regexes[regex], l)
                if match is not None:
                    values = match.groups()
                    if regex in ["data", "dataOpt"]:
                        val = [float(x) for x in values]
                        problems[problem_name]["data"].append(
                            val if len(val) > 1 else val[0]
                        )
                    else:
                        problems[problem_name][regex] = values[0]
                    break
        # Filter the euclidean problems
        if "edge_weight_type" not in problems[problem_name]:
            del problems[problem_name]
        elif "EUC" not in problems[problem_name]["edge_weight_type"]:
            del problems[problem_name]
        elif len(problems[problem_name]["data"]) == 0:
            raise Exception(
                "No data found for problem {}. View if file comply with this script parsing process".format(
                    problem_name
                )
            )
    with open("data/tsplib.json", "w") as f:
        json.dump(problems, f)

