import React from "react";
import { CoordType } from "../../processing/linearAlg";
import * as linalg from "../../processing/linearAlg";
export interface ArrowProps {
  start: CoordType;
  end: CoordType;
  rectHeight?: number; // =
  headHeight?: number; // => (added to rect height)                                                           __
  // rectAdjust?: boolean; // if true consider consider a rectangle with borders aligned on X/Y axes and if line /   fill vertically where the arrow would join the rectangle
  headLength?: number;
  fillColor?: string;
  strokeColor?: string;
}

function parametricLine(initPoint: CoordType, gradient: CoordType, t: number) {
  return {
    x: initPoint.x + t * gradient.x,
    y: initPoint.y + t * gradient.y,
  };
}
function tForDist(gradient: CoordType, dist: number) {
  // deltaX**2+deltaY**2=dist**2
  // (t * gradient.x)**2+(t * gradient.y)**2=dist**2
  // t**2*(gradient.x**2+gradient.y**2)=dist**2
  // t = dist/sqrt(gradient.x**2+gradient.y**2)
  return (
    Math.abs(dist) /
    Math.sqrt(Math.pow(gradient.x, 2) + Math.pow(gradient.y, 2))
  );
}
function normalVector(gradient: CoordType) {
  return { x: gradient.y, y: -gradient.x };
}
function gradMinusDist(gradient: CoordType, dist: number) {
  const tmp = tForDist(gradient, dist);
  console.log("tmp:", tmp);
  return {
    x: gradient.x - gradient.x * tmp,
    y: gradient.y - gradient.y * tmp,
  };
}
function cmptGradient(start: CoordType, end: CoordType) {
  return {
    x: end.x - start.x,
    y: end.y - start.y,
  };
}
function add(p1: CoordType, p2: CoordType) {
  return { x: p1.x + p2.x, y: p1.y + p2.y };
}
function sub(p1: CoordType, p2: CoordType) {
  return { x: p1.x - p2.x, y: p1.y - p2.y };
}
function dist(e: CoordType) {
  return Math.sqrt(e.x ** 2 + e.y ** 2);
}
export function Arrow({
  start,
  end,
  rectHeight = 5,
  headHeight = 5,
  headLength = 5,
  fillColor = "black",
  strokeColor = "black",
}: ArrowProps) {
  // Checkings
  if (rectHeight <= 0 || headHeight <= 0 || headLength <= 0) {
    return <path></path>;
  }
  if (headLength > dist(sub(start, end))) {
    throw new Error("head length > dist(start,end)");
  }

  //Create arrow
  const gradient = cmptGradient(start, end);
  const points: Array<CoordType> = [];
  const shiftUp = parametricLine(
    start,
    normalVector(gradient),
    tForDist(gradient, rectHeight / 2)
  );
  points.push(shiftUp);
  points.push(
    parametricLine(
      points[points.length - 1],
      gradient,
      tForDist(gradient, dist(gradient) - headLength)
    )
  );
  points.push(
    parametricLine(
      points[points.length - 1],
      normalVector(gradient),
      tForDist(gradient, headHeight / 2)
    )
  );
  points.push(end);
  points.push(
    parametricLine(
      points[points.length - 3],
      normalVector(gradient),
      -tForDist(gradient, headHeight / 2 + rectHeight)
    )
  );
  points.push(
    parametricLine(
      points[points.length - 1],
      normalVector(gradient),
      tForDist(gradient, headHeight / 2)
    )
  );
  points.push(
    parametricLine(
      points[points.length - 1],
      gradient,
      -tForDist(gradient, dist(gradient) - headLength)
    )
  );
  points.push(start);

  const pointsProvided = points;
  return (
    <g className="arrow">
      <path
        d={
          `M${start.x},${start.y}` +
          pointsProvided.map((x) => ` L${x.x},${x.y}`).join(" ")
        }
        style={{
          fill: fillColor,
          stroke: strokeColor,
        }}
      ></path>
    </g>
  );
}
