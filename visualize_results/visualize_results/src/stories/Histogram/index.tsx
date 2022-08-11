import React from "react";
import "../../App.css"
import * as p from "../popup"
/**
 * Primary UI component for user interaction
 */
interface HistogramProps {
  bins: Array<HistoData>;
  binSizePx: number;
  maxCount: number;
  maxHeight: number;
  padding?: number;
  color: string;
}
export interface HistoData {
  start: number;
  stop: number;
  count: number;
  binIndex: number;
}
export function computeBins(
  data: Array<number>,
  binSize: number,
  start: number,
  end: number
): Array<HistoData> {
  const data_sorted = [...data];
  data_sorted.sort();
  let startBin = start === undefined ? data_sorted[0] : start;
  const stopLoop =
    end === undefined ? data_sorted[data_sorted.length - 1] : end;
  let i = 0;
  let bins: Array<HistoData> = [];
  while (startBin + binSize < stopLoop) {
    const btwStart = startBin;
    const btwStop = btwStart + binSize;
    let count = 0;
    while (
      i < data_sorted.length &&
      data_sorted[i] < btwStop &&
      data_sorted[i] >= btwStart
    ) {
      count++;
      i++;
    }
    bins.push({ start: btwStart, stop: btwStop, count: count ,binIndex: bins.length});
    startBin = btwStop;
  }
  return bins;
}
export const Histogram = ({
  bins,
  binSizePx,
  maxCount,
  maxHeight,
  padding = 2,
  color="black",
  ...props
}: HistogramProps) => {
  return (
    <g className="hist-bars" key={`${binSizePx}_${maxCount}_${maxHeight}`}>
      {bins.map((x, i) => {
        console.log(x.binIndex)
        const height = (x.count / maxCount) * maxHeight
        return (
          <rect
          key={`${x.binIndex}_${binSizePx}_${x.count}`}
            x={x.binIndex * (binSizePx+padding)}
            y={maxHeight-height}
            width={binSizePx}
            height={height}
            style={{fill:color}}
            onMouseMove={(e) => {
              document.querySelectorAll('rect').forEach(x=> {
                x.style.filter = 'brightness(1)'
              })
              const contentPopup = `<strong>Values:</strong>[${x.start.toFixed(2)};${x.stop.toFixed(2)}]<br><strong>Count:</strong>${x.count}`
              p.createPopup(e,contentPopup)
              const self = document.querySelector('rect:hover')
              if(self !== null) {
                console.log('changing')
                self.style.filter = 'brightness(75%)'
              }
            }}
            onMouseLeave={(e)=> {
              p.cleanPopup(e)
              document.querySelectorAll('rect').forEach(x=> {
                x.style.filter = 'brightness(1)'
              })
            }}
          ></rect>
        );
      })}
    </g>
  );
};
