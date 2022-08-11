import React from "react";
import * as u from "../../processing/utils"
import * as p from "../popup"
import "../../App.css"
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
export const ScatterPlot = ({
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
  
  let height = 0
  const dstDomain = scaleY.getDstDomain()
  if(dstDomain !== null){
    if(dstDomain[1] !== null) height = dstDomain[1]
  }
  return (
    <g transform={`translate(${translate.x},${translate.y})`}>
        {values.map(x=> {
            return (
            <circle 
                cx={scaleX.srcToDst(x.x)} 
                cy={height-scaleY.srcToDst(x.y)} 
                r={radius} 
                style={{
                fill: color}}
                onMouseEnter={(e) => {
                    document.querySelectorAll('circle').forEach(x=> {
                      x.style.filter = 'brightness(1)'
                    })
                    const xLabel = xName === '' ? 'X' : xName
                    const yLabel = yName === '' ? 'Y' : yName
                    const contentPopup = `<strong>${xLabel}:</strong> ${x.x}<br><strong>${yLabel}:</strong> ${x.y}`
                    p.createPopup(e,contentPopup)
                    const self = document.querySelector('circle:hover')
                    if(self !== null) {
                      console.log('changing')
                      self.style.filter = 'brightness(75%)'
                    }
                }}
                onMouseLeave={(e) => {
                    p.cleanPopup(e)
                    document.querySelectorAll('circle').forEach(x=> {
                      x.style.filter = 'brightness(1)'
                    })
                }}
                onMouseDown={(e) => {
                    callbackClick(e,x)
                }}
            ></circle>)
        })}
    </g>
  )
};
