export function statsArray(arr: Array<number>) {
    let min = Number.MAX_SAFE_INTEGER
    let max = Number.MIN_SAFE_INTEGER
    let argmin = -1
    let argmax = -1
    arr.forEach((x,i) => {
        if(x<min) {
            min = x
            argmin = i
        }
        if(x>max) {
            max = x
            argmax = i
        }
    })
    return {min:min,max:max,argmin:argmin,argmax:argmax}
}
export class LinearScale {
    _srcDomain: Array<number>;
    _dstDomain: Array<number>;
    constructor() {
        this._srcDomain = [0,0]
        this._dstDomain = [0,0]
    }
    srcDomain(d: Array<number>|null = null) {
        if(d !== null) {
            this._srcDomain = [...d]
        }
        return this;
    }
    dstDomain(r: Array<number>|null = null) {
        if(r !== null) {
            this._dstDomain = [...r]
        }
        return this
    }
    getSrcDomain() : Array<number> | null{
        if (this._srcDomain[0] === null || this._srcDomain[1] === null) return null;
        return this._srcDomain
    }
    getDstDomain() {
        if (this._dstDomain[0] === null || this._dstDomain[1] === null) return null;
        return this._dstDomain
    }
    srcToDst(v: number) {
        return (v-this._srcDomain[0])/(this._srcDomain[1]-this._srcDomain[0])*(this._dstDomain[1]-this._dstDomain[0])+this._dstDomain[0]
    }
    dstToSrc(v: number) {
        return (v-this._dstDomain[0])/(this._dstDomain[1]-this._dstDomain[0])*(this._srcDomain[1]-this._srcDomain[0])+this._srcDomain[0]
    }
    toValPerPx() {
        return (this._srcDomain[1]-this._srcDomain[0])/(this._dstDomain[1]-this._dstDomain[0])
    }
}
export function getTicks(scale: LinearScale, nTicks: number) : Array<number>{
    const domainSrc = scale.getSrcDomain()
    if(domainSrc !== null) {
        const delta = (domainSrc[1]-domainSrc[0])/nTicks
        return Array(nTicks+1).fill(0).map((x,i)=> {
            return domainSrc[0]+i*delta
        })
    } else {
        return []
    }
}
