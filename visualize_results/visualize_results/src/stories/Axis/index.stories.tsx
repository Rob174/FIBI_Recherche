import React from "react";
import { ComponentStory, ComponentMeta } from "@storybook/react";

import { Axis } from ".";

export default {
  title: "Example/Axis",
  component: Axis,
  argTypes: {
    backgroundColor: { control: "color" },
  },
} as ComponentMeta<typeof Axis>;

const Template: ComponentStory<typeof Axis> = (args) => (
  <svg height="100vh">
    <g transform="translate(50,250)">
      <Axis {...args} />
    </g>
  </svg>
);

export const Bottom = Template.bind({});
Bottom.args = {
  valPerPx: 0.25,
  padding: 2,
  ticks: [-5, 0, 5, 10, 15],
  orientation: "bottom",
  legend: "x axis value",
  precision:0
};

export const Top = Template.bind({});
Top.args = {
  valPerPx: 0.25,
  padding: 2,
  ticks: [-5, 0, 5, 10, 15],
  orientation: "top",
  legend: "x axis value",
  precision:0
};
export const Left = Template.bind({});
Left.args = {
  valPerPx: 0.25,
  padding: 2,
  ticks: [-5, 0, 5, 10, 15],
  orientation: "left",
  legend: "x axis value",
  precision:0
};
export const Right = Template.bind({});
Right.args = {
  valPerPx: 0.25,
  padding: 2,
  ticks: [-5, 0, 5, 10, 15],
  orientation: "right",
  legend: "x axis value",
  precision:0
};
export const Test = Template.bind({});
Test.args = {
  valPerPx: 0.1,
  padding: 2,
  ticks: [92, 97.25, 102.5, 107.75, 113],
  orientation: "bottom",
  legend: "x axis value",
  precision:2
};
