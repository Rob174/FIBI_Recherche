import React from 'react';
import { ComponentStory, ComponentMeta } from '@storybook/react';

import { LineScatterPlot } from '../LineScatterPlot';
import * as u from "../../processing/utils"

export default {
  title: 'Example/LineScatterPlot',
  component: LineScatterPlot,
  argTypes: {
    backgroundColor: { control: 'color' },
  },
} as ComponentMeta<typeof LineScatterPlot>;

const Template: ComponentStory<typeof LineScatterPlot> = (args) => <svg height="100vh" width="100vw"><LineScatterPlot {...args} /></svg>;

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
export const Primary = Template.bind({});
Primary.args = {
  scaleX: xScale,
  scaleY: yScale,
    values: data,
    translate:{x:50,y:0}
};
