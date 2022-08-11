import React from "react";
import {LinePlot} from "../LinePlot"
import {ScatterPlot} from "../ScatterPlot"
import "../../App.css"
import * as u from "../../processing/utils"
interface ScatterPlotProps {
  scaleX: u.LinearScale;
  scaleY: u.LinearScale;
  radius?: number;
  color?: string;
  values: Array<{x:number,y:number}>;
  translate?: {x:number; y:number};
  xName: string;
  yName: string;
  callbackClick: (e: any, x: {x:number;y:number;}) => void
}
export const LineScatterPlot = ({
  scaleX,
  scaleY,
  radius = 4,
  values,
  color = "red",
  translate = {x:0,y:0},
  xName = "",
  yName = "",
  callbackClick = (e) => {return},
  ...props
}: ScatterPlotProps) => {
  return (
    <g transform={`translate(${translate.x},${translate.y})`}>
    <LinePlot
    scaleY={scaleY} 
    scaleX={scaleX} 
    color={color}
    values={values} 
    />
        <ScatterPlot 
        scaleY={scaleY} 
        scaleX={scaleX} 
        radius={radius} 
        values={values} 
        color={color}
        xName={xName}
        yName={yName}
        callbackClick={callbackClick}
        />
    </g>
  )
};
