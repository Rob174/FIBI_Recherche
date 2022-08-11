import React, { useState, useEffect, useRef } from "react";
import { Arrow } from "../Arrow";
import * as u from "../../processing/utils";

/**
 * Primary UI component for user interaction
 */
interface AxisProps {
  valPerPx: number;
  ticks: Array<number>;
  padding?: number;
  color?: string;
  orientation?: "top" | "bottom" | "left" | "right";
  legend?: string;
  precision?: number;
  transform?: string;
}
export function getTextWidth(text: string) {
  const t = document.createElement("text");
  const svg = document.createElement("svg");
  svg.appendChild(t);
  document.body.appendChild(svg);
  t.innerText = text;
  const dims = t.getBoundingClientRect();
  const res = { width: dims.width, height: dims.height };
  t.parentElement?.removeChild(t);

  return res;
}
export const sizeTick = 8;
export const sizeArrow = 1;
export const sizeLegend = getTextWidth("tmp");
const textYOffset = 4;
const legendYOffset = 5;
export const yLegend =
  sizeTick / 2 + sizeLegend.height + textYOffset + legendYOffset;
export const Axis = ({
  valPerPx,
  ticks,
  padding = 2,
  color = "black",
  orientation = "bottom",
  legend = "",
  precision = 2,
  transform = "",
  ...props
}: AxisProps) => {
  const statsTicks = u.statsArray(ticks);
  console.log(statsTicks);
  const valToPx = (val: number) => val / valPerPx - statsTicks.min / valPerPx;
  const orientInner =
    orientation === "bottom" || orientation === "right" ? "bottom" : "top";
  const margin = valToPx(ticks[1])/2;
  console.log('margin:',margin)
  const sizeLegend = getTextWidth(legend);
  const totalLengthArrow =
    (statsTicks.max - statsTicks.min)/valPerPx + ticks.length * padding + margin;
  console.log(valPerPx, ticks,totalLengthArrow,margin,valToPx(statsTicks.max - statsTicks.min));
  const xLegend = totalLengthArrow / 2 - sizeLegend.width / 2;
  let yLegend = sizeTick / 2 + sizeLegend.height + textYOffset + legendYOffset;
  yLegend = orientInner === "bottom" ? yLegend : -yLegend;

  return (
    <g transform={transform}>
      <g
        transform={`translate(0,${
          orientation === "bottom" || orientation === "top"
            ? 0
            : (statsTicks.max - statsTicks.min)/valPerPx + ticks.length * padding
        })`}
      >
        <g
          transform={`rotate(${
            orientation === "bottom" || orientation === "top" ? 0 : -90
          })`}
        >
          <g>
            <Arrow
              start={{ x: 0, y: 0 }}
              end={{ x: totalLengthArrow, y: 0 }}
              rectHeight={sizeArrow}
              headHeight={sizeArrow * 4}
              fillColor={color}
              strokeColor={color}
            />
            <g>
              {ticks.map((x, i) => {
                const size = getTextWidth(`${x.toFixed(precision)}`);
                return (
                  <g key={x}>
                    <line
                      x1={valToPx(x) + i * padding}
                      x2={valToPx(x) + i * padding}
                      y1={-sizeTick / 2}
                      y2={sizeTick / 2}
                      stroke={color}
                      strokeWidth={sizeArrow * 2.5}
                    ></line>
                    <text
                      transform={`translate(${
                        valToPx(x) + i * padding - size.width / 2
                      },${
                        orientInner === "bottom"
                          ? size.height
                          : -sizeTick / 2 - textYOffset
                      })`}
                      color={color}
                    >
                      {x.toFixed(precision)}
                    </text>
                  </g>
                );
              })}
            </g>
            <text transform={`translate(${xLegend},${yLegend})`}>{legend}</text>
          </g>
        </g>
      </g>
    </g>
  );
};
