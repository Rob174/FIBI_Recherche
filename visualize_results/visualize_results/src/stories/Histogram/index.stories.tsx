import React from 'react';
import { ComponentStory, ComponentMeta } from '@storybook/react';

import { Histogram,computeBins } from '../Histogram';

// More on default export: https://storybook.js.org/docs/react/writing-stories/introduction#default-export
export default {
  title: 'Example/Histogram',
  component: Histogram,
  // More on argTypes: https://storybook.js.org/docs/react/api/argtypes
  argTypes: {
    backgroundColor: { control: 'color' },
  },
} as ComponentMeta<typeof Histogram>;

// More on component templates: https://storybook.js.org/docs/react/writing-stories/introduction#using-args
const Template: ComponentStory<typeof Histogram> = (args) => <svg height="100%"><Histogram {...args} /></svg>;

export const Primary = Template.bind({});
// More on args: https://storybook.js.org/docs/react/writing-stories/args
const bins = computeBins(Array(1000).fill(0).map(x=>Math.random()),0.1,0,1)
bins.sort(x=>x.count)
Primary.args = {
    bins: bins,
    binSizePx: 50,
    maxCount: bins[bins.length-1].count,
    maxHeight: 500,
    color: 'blue'
};
