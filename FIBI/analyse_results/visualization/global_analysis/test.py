import pandas as pd
import plotly.express as px
from pathlib import Path
import plotly.graph_objects as go
from math import cos, sin, pi

def run(Ldico):
    
    for k in Ldico:
        
        # convert to percentage (get total and divide value by total) the main_case and case columns
        df = pd.DataFrame(Ldico[k])[['main_case','case','color']].groupby(['main_case','case','color']).size().reset_index(name='number')
        df.sort_values(by=['main_case','number'], ascending=[0,0],inplace=True)
        a = "main_case"
        b = "case"
        e = 'number'
        total = df.groupby([a,b]).sum()
        total["aaa_sum"] = getattr(df.groupby([a, b]), e).sum().groupby(level=a).transform('sum')
        total["aaa_bbb_sum"] = getattr(df.groupby([a, b]), e).sum().groupby(level=[a,b]).transform('sum')
        total.sort_values(by=['aaa_sum','aaa_bbb_sum'], ascending=[0,0],inplace=True)
        ## calculate the angle subtended by each category
        sum_ddd = getattr(df,e).sum()
        delta_angles = 360*total[e] / sum_ddd
        angles_in_degrees = pd.concat([pd.DataFrame(data=[0]),delta_angles]).cumsum().rolling(window=2).mean().dropna().values
        
        def get_xy_coordinates(angles_in_degrees, r=1):
            return [r*cos(angle*pi/180)+0.5 for angle in angles_in_degrees], [r*sin(angle*pi/180)+0.5 for angle in angles_in_degrees]
        x_coordinates, y_coordinates = get_xy_coordinates(angles_in_degrees, r=0.55)
        # use the colors of the column color to color the sunburst ['#A100FF', '#09DB00',, '#FA1D00']
        fig = px.sunburst(df, path=['main_case', 'case'], values="number", width=600, height=600, color='color', color_discrete_map={'#A100FF': '#A100FF', '#09DB00': '#09DB00', '#FA1D00': '#FA1D00'})
        fig.update_traces(textinfo="label", insidetextorientation='horizontal',textfont=dict(size=12))
        padding = 0.1
        fig.update_layout(
            xaxis=dict(
                range=[-1 - padding, 1 + padding], 
                showticklabels=False
            ), 
            yaxis=dict(
                range=[-1 - padding, 1 + padding],
                showticklabels=False
            ),
            plot_bgcolor='rgba(0,0,0,0)'
        )
        out_path = Path("data\\analysis_results\\tsp\\tsplib\\cases")
        # Iterate over the labels and change the text where the percentage is less than 5% to be blank. Add an annotation to the exterior of the chart to show the percentage of the blank labels.
        for i, label in enumerate(fig.data[0].labels):
            # if label not in df['main_case'].unique():
            fig.data[0].labels[i] = f'{fig.data[0].values[i]/sum_ddd*100:.0f}%'
        for x,y,angle,t in zip(x_coordinates,y_coordinates,angles_in_degrees,df['case']):
            
            xanchor = 'center' if 45 <= angle <= 135 or 225 <= angle <= 315 else 'left' if angle < 45 or angle > 315 else 'right'
            yanchor = 'middle' if angle <= 45 or angle >= 315 or 135 <= angle <= 225 else 'bottom' if 45 < angle < 135 else 'top'
            fig.add_annotation(
                x=x,
                y=y,
                text=t,
                showarrow=False,
                xanchor=xanchor,
                yanchor=yanchor,
                font=dict(size=12)
            )
            
        fig.update_layout(
            xaxis=dict(
                range=[-1 - padding, 1 + padding], 
                showticklabels=False
            ), 
            yaxis=dict(
                range=[-1 - padding, 1 + padding],
                showticklabels=False
            ),
            plot_bgcolor='rgba(0,0,0,0)',
            margin=dict(l=200, r=200, t=150, b=150)
        )
        fig.write_html(out_path / f'cases_{k}.html')
        fig.write_image(out_path / f'cases_{k}.png')