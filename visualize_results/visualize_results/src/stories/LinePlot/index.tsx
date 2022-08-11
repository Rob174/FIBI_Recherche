import React from "react";
import * as u from "../../processing/utils"
import "../../App.css"
import "./lineplot.css"
interface LinePlotProps {
  scaleX: u.LinearScale;
  scaleY: u.LinearScale;
  color: string;
  values: Array<{x:number,y:number}>;
}
export const LinePlot = ({
  scaleX,
  scaleY,
  values,
  color,
  ...props
}: LinePlotProps) => {
  const valuesSorted = [...values]
  valuesSorted.sort((a,b)=>(a.x-b.x))
  let height = 0
  const dstDomain = scaleY.getDstDomain()
  if(dstDomain !== null){
    if(dstDomain[1] !== null) height = dstDomain[1]
  }

  let pathStr = `M${scaleX.srcToDst(valuesSorted[0].x)},${height-scaleY.srcToDst(valuesSorted[0].y)}`
  valuesSorted.forEach((x,i)=> {
    if(i > 0) {
      pathStr += ` L${scaleX.srcToDst(x.x)},${height-scaleY.srcToDst(x.y)}`
    }
  })
  return (
    <path d={pathStr} style={{
      fill:'transparent',
      stroke: color
    }}></path>
  );
};
