import React from "react";
import { Histogram, HistoData } from "../Histogram";
import { Axis, yLegend } from "../Axis";
import * as u from "../../processing/utils";
import {getTextWidth} from "../Axis"
type DataType = Array<{ color: string; data: Array<HistoData>; name: string }>;
interface ComposedHistogramProps {
  width: number;
  height: number;
  data: DataType;
  transform: string;
}
export const ComposedHistogram = ({
  width,
  height,
  data,
  transform,
  ...props
}: ComposedHistogramProps) => {
  const stats = u.statsArray(data.map((x) => x.data.length));
  const statsCount = u.statsArray(
    data.map((x) => u.statsArray(x.data.map((a) => a.count)).max)
  );
  const binSize = width / (stats.max + 1);
  const valPerPx =
    (data[stats.argmax].data[0].stop - data[stats.argmax].data[0].start) /
    binSize;
  let ticks = data[stats.argmax].data.map((x) => x.start);
  ticks.push(data[stats.argmax].data[data[stats.argmax].data.length - 1].stop);
  const countStats = data.map(x=>u.statsArray(x.data.map(a=>a.count)))
  const maxCount = u.statsArray(countStats.map(x=>x.max)).max
  const numTicks = 5
  const tickCount = Array(numTicks)
  .fill(0)
  .map(
    (x, i) =>
      (i/(numTicks-1)*(maxCount))
  )
  const ticksValPerPx = u.statsArray(tickCount).max / height
  const offsetYAxis = (tickCount[1]-tickCount[0])/ticksValPerPx/2 - getTextWidth(`${tickCount[0].toFixed(2)}`).width/2
  return (
    <svg width={width+100} height={height + offsetYAxis+100}>
      <g transform={transform}>
        <g transform={`translate(50,${offsetYAxis+50})`}>
        {data.map((x) => {
          return (
            <Histogram
              bins={x.data}
              binSizePx={binSize}
              maxCount={statsCount.max}
              maxHeight={height}
              color={x.color}
              padding={0}
            />
          );
        })}
        <Axis
          valPerPx={valPerPx}
          ticks={ticks}
          legend={"Bins"}
          orientation={"bottom"}
          transform={`translate(0,${height})`}
          padding={0}
        />
        </g>
        <Axis
          valPerPx={ticksValPerPx}
          ticks={tickCount}
          legend={"Count"}
          orientation={"left"}
          transform={`translate(50,${offsetYAxis+50})`}
          precision={2}
          padding={0}
        />
      </g>
    </svg>
  );
};
