import React from "react";
import { ComponentStory, ComponentMeta } from "@storybook/react";

import { ComposedHistogram } from ".";
import { computeBins } from "../Histogram";

export default {
  title: "Example/ComposedHistogram",
  component: ComposedHistogram,
  argTypes: {
    backgroundColor: { control: "color" },
  },
} as ComponentMeta<typeof ComposedHistogram>;

const Template: ComponentStory<typeof ComposedHistogram> = (args) => (
      <ComposedHistogram {...args} />
);

const bins1 = computeBins(Array(1000).fill(0).map(x=>Math.random()),0.1,0,1)
const bins2 = computeBins(Array(1000).fill(0).map(x=>Math.random()),0.1,0,1)
export const Solo = Template.bind({});
Solo.args = {
  width: 500,
  height: 200,
  data: [{color:'red',data:bins1,name:'data1'}],
  transform: 'translate(0,0)'
};
export const Duo = Template.bind({});
Duo.args = {
  width: 500,
  height: 200,
  data: [{color:'rgba(255,0,0,0.5)',data:bins1,name:'data1'},{color:'rgba(0,0,255,0.5)',data:bins2,name:'data2'}],
  transform: 'translate(0,0)'
};