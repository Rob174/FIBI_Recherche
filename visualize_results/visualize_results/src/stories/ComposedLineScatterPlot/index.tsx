import React from "react";
import {LineScatterPlot} from "../LineScatterPlot"
import {Axis} from "../Axis"
import "../../App.css"
import * as u from "../../processing/utils"
interface ComposedLineScatterPlotProps {
  width: number;
  height: number;
  radius?: number;
  values: Array<{color: string; data:Array<{x:number,y:number}>, callback: (e: any, x: {x:number;y:number;}) => void}>;
  translate: {x:number; y:number};
  xName: string;
  yName: string;
}
export const ComposedLineScatterPlot = ({
  width,
  height,
  radius = 4,
  values,
  translate = {x:0,y:0},
  xName = "",
  yName = "",
  ...props
}: ComposedLineScatterPlotProps) => {
  const statsDataX = values.map(x=>u.statsArray(x.data.map(a=>a.x)))
  const statsDataY = values.map(x=>u.statsArray(x.data.map(a=>a.y)))
  const minDataX = u.statsArray(statsDataX.map(x=>x.min)).min
  const maxDataX = u.statsArray(statsDataX.map(x=>x.max)).max
  const minDataY = u.statsArray(statsDataY.map(x=>x.min)).min
  const maxDataY = u.statsArray(statsDataY.map(x=>x.max)).max
  const xScale = (new u.LinearScale()).dstDomain([0,width]).srcDomain([minDataX,maxDataX])
  const yScale = (new u.LinearScale()).dstDomain([0,height]).srcDomain([minDataY,maxDataY])
  const xTicks = u.getTicks(xScale,10)
  const yTicks = u.getTicks(yScale,10)
  return (
    <g transform={`translate(${translate.x},${translate.y})`}>
      <g transform={`translate(${0},${0})`}>
      {values.map(x=>{
        return (
          <LineScatterPlot 
          scaleX={xScale}
          scaleY={yScale}
          values={x.data} 
          callbackClick={x.callback} 
          xName={xName} 
          yName={yName} 
          radius={radius}
          color={x.color}/>
        )
      })}
      <Axis padding={0} valPerPx={xScale.toValPerPx()} legend={xName} ticks={xTicks} orientation="bottom" transform={`translate(0,${height})`}/>
      </g>
      <Axis padding={0} valPerPx={yScale.toValPerPx()} legend={yName} ticks={yTicks} orientation="left"/>
    </g>
  )
};
