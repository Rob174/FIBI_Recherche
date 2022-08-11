import React from 'react';
import { ComponentStory, ComponentMeta } from '@storybook/react';

import { ComposedLineScatterPlot } from '../ComposedLineScatterPlot';
import * as u from "../../processing/utils"

export default {
  title: 'Example/ComposedLineScatterPlot',
  component: ComposedLineScatterPlot,
  argTypes: {
  },
} as ComponentMeta<typeof ComposedLineScatterPlot>;

const Template: ComponentStory<typeof ComposedLineScatterPlot> = (args) => <svg height="100vh" width="100vw"><ComposedLineScatterPlot {...args} /></svg>;
const width = 600
const height = 500
const values = Array(5).fill(0).map((x,i) => {
    const xScale = new u.LinearScale()
    xScale.dstDomain([0,width])
    const yScale = new u.LinearScale()
    yScale.dstDomain([0,height])
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
    return {color:'red',data:data,callback: (e: any, x: {x:number;y:number;}) => {return}}
})

export const Primary = Template.bind({});
Primary.args = {
    width: width,
    height: height,
    values: values,
    translate:{x:100,y:100},
    xName: 'X',
    yName: 'Y'
};
