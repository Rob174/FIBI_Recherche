import React from 'react';
import { ComponentStory, ComponentMeta } from '@storybook/react';

import { ScatterPlot } from '../ScatterPlot';
import * as u from "../../processing/utils"

export default {
  title: 'Example/ScatterPlot',
  component: ScatterPlot,
  argTypes: {
    backgroundColor: { control: 'color' },
  },
} as ComponentMeta<typeof ScatterPlot>;

const Template: ComponentStory<typeof ScatterPlot> = (args) => <svg height="100vh" width="100vw"><ScatterPlot {...args} /></svg>;

export const Primary = Template.bind({});
const xScale = new u.LinearScale()
xScale.dstDomain([0,600])
const yScale = new u.LinearScale()
yScale.dstDomain([0,500])
const data = [
  {x:Math.random()*10,y: Math.random()},
  {x:Math.random()*10,y: Math.random()},
  {x:Math.random()*10,y: Math.random()},
  {x:Math.random()*10,y: Math.random()},
  {x:Math.random()*10,y: Math.random()},
  {x:Math.random()*10,y: Math.random()},
  {x:Math.random()*10,y: Math.random()},
  {x:Math.random()*10,y: Math.random()},
]
const xStats = u.statsArray(data.map(x=>x.x))
const yStats = u.statsArray(data.map(x=>x.y))
xScale.srcDomain([xStats.min,xStats.max])
yScale.srcDomain([yStats.min,yStats.max])
Primary.args = {
    scaleX:xScale,
    scaleY: yScale,
    values: data,
    translate:{x:50,y:0}
};
