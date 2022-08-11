export {}

export interface StatisticGroup {
    mean: number;
    std: number;
    min: number;
    q1: number;
    median: number;
    q3: number;
    max: number;
}
export type IMPR_CLASSType = "BICL" | "BINL" | "FICL" | "FINL"
export type sourceType = "computed" | "reference"
export type INIT_CHOICEType = "random" | "greedy"
export type problemType = "tsp" | "clustering"
export interface Group {
    NUM_POINTS: number;
    IMPR_CLASS: IMPR_CLASSType;
      source: sourceType;
      final_cost: StatisticGroup;
      num_iter: StatisticGroup
      duration: StatisticGroup
      INIT_CHOICE: INIT_CHOICEType;
      problem: problemType;
      NUM_CLUST: number;
}
export interface Record {
    NUM_POINTS: number;
    IMPR_CLASS: IMPR_CLASSType;
      source: sourceType;
      final_cost: number;
      num_iter: number;
      duration: number;
      INIT_CHOICE: INIT_CHOICEType;
      problem: problemType;
      NUM_CLUST: number;
}
export interface InputJSONType {
    grouped: Array<Group>;
    filtered: {
        [keys: string]: Array<Record>
    };
}