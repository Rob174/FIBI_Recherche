import * as t from "./type"
import data from "../assets/table1_visualize.json"
export function averages(imprClass: t.IMPR_CLASSType, source: t.sourceType, init: t.INIT_CHOICEType, numClust: number = -1) : t.Group {
    let res;
    if(numClust !== -1) {
        res = data.grouped.filter(x=>x.IMPR_CLASS === imprClass && x.source === source && x.INIT_CHOICE === init && x.NUM_CLUST === numClust)
    } else {
        res = data.grouped.filter(x=>x.IMPR_CLASS === imprClass && x.source === source && x.INIT_CHOICE === init)
    }
    return res
}
export function getData(numPoints: number, imprClass: t.IMPR_CLASSType, source: t.sourceType, init: t.INIT_CHOICEType, numClust: number = -1) {
    let dataExtr
    
    if(numClust !== -1) {
        dataExtr = data.filtered.filter(x=>x.IMPR_CLASS === imprClass && x.source === source && x.INIT_CHOICE === init && x.NUM_CLUST === numClust)
    } else {
        dataExtr = data.filtered.filter(x=>x.IMPR_CLASS === imprClass && x.source === source && x.INIT_CHOICE === init)
    }
    dataExtr.sort((x: t.Record)=>x.final_cost)
}
