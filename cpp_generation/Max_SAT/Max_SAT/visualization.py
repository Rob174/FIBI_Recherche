import argparse
import matplotlib.pyplot as plt
import json
from typing import TypedDict, List, Dict, Any, Union, Literal
import numpy as np
from matplotlib.animation import FuncAnimation
import mpl_toolkits.axes_grid1
import matplotlib.widgets
import colorsys

Problem = Literal["tsp", "clustering", "maxsat"]
class TSPResult:
    problem: Problem
    towns: List[float]
    num_dim: int
    tours: List[List[int]]
    annotations: List[str]

class ClusteringResult:
    problem: Problem
    points: List[List[float]]
    num_dim: int
    assignements: List[List[int]]
    centroids: List[List[float]]
    annotations: List[str]
    
class MAXSATResult:
    problem: Problem
    clauses: List[List[List[int, bool]]]
    num_vars: int
    num_clauses: int
    assignments: List[List[int]]
    annotations: List[str]

class Player(FuncAnimation):
    def __init__(self, fig, func, frames=None, init_func=None, fargs=None,
                 save_count=None, mini=0, maxi=100, pos=(0.125, 0.92), **kwargs):
        self.i = 0
        self.min=mini
        self.max=maxi
        self.runs = True
        self.forwards = True
        self.fig = fig
        self.func = func
        self.setup(pos)
        FuncAnimation.__init__(self,self.fig, self.update, frames=self.play(), 
                                           init_func=init_func, fargs=fargs,
                                           save_count=save_count, **kwargs )    

    def play(self):
        while self.runs:
            self.i = self.i+self.forwards-(not self.forwards)
            if self.i > self.min and self.i < self.max:
                print(self.i)
                yield self.i
            else:
                self.stop()
                yield self.i

    def start(self):
        self.runs=True
        self.event_source.start()

    def stop(self, event=None):
        self.runs = False
        self.event_source.stop()

    def forward(self, event=None):
        self.forwards = True
        self.start()
    def backward(self, event=None):
        self.forwards = False
        self.start()
    def oneforward(self, event=None):
        self.forwards = True
        self.onestep()
    def onebackward(self, event=None):
        self.forwards = False
        self.onestep()

    def onestep(self):
        if self.i > self.min and self.i < self.max:
            self.i = self.i+self.forwards-(not self.forwards)
        elif self.i == self.min and self.forwards:
            self.i+=1
        elif self.i == self.max and not self.forwards:
            self.i-=1
        self.func(self.i)
        self.slider.set_val(self.i)
        self.fig.canvas.draw_idle()

    def setup(self, pos):
        playerax = self.fig.add_axes([pos[0],pos[1], 0.64, 0.04])
        divider = mpl_toolkits.axes_grid1.make_axes_locatable(playerax)
        bax = divider.append_axes("right", size="80%", pad=0.05)
        sax = divider.append_axes("right", size="80%", pad=0.05)
        fax = divider.append_axes("right", size="80%", pad=0.05)
        ofax = divider.append_axes("right", size="100%", pad=0.05)
        sliderax = divider.append_axes("right", size="500%", pad=0.07)
        self.button_oneback = matplotlib.widgets.Button(playerax, label='$\u29CF$')
        self.button_back = matplotlib.widgets.Button(bax, label='$\u25C0$')
        self.button_stop = matplotlib.widgets.Button(sax, label='$\u25A0$')
        self.button_forward = matplotlib.widgets.Button(fax, label='$\u25B6$')
        self.button_oneforward = matplotlib.widgets.Button(ofax, label='$\u29D0$')
        self.button_oneback.on_clicked(self.onebackward)
        self.button_back.on_clicked(self.backward)
        self.button_stop.on_clicked(self.stop)
        self.button_forward.on_clicked(self.forward)
        self.button_oneforward.on_clicked(self.oneforward)
        self.slider = matplotlib.widgets.Slider(sliderax, '', 
                                                self.min, self.max, valinit=self.i)
        self.slider.on_changed(self.set_pos)

    def set_pos(self,i):
        self.i = int(self.slider.val)
        self.func(self.i)

    def update(self,i):
        self.slider.set_val(i)
class HoverManager:
    def __init__(self, fig, ax, sc, annot, names):
        self.fig = fig
        self.ax = ax
        self.sc = sc
        self.annot = annot
        self.names = names
        self.fig.canvas.mpl_connect("motion_notify_event", self.hover)
    def update_annot(self,ind):

        pos = self.sc.get_offsets()[ind["ind"][0]]
        self.annot.xy = pos
        text = f"{' '.join([self.names[n] for n in ind['ind']])}"
        self.annot.set_text(text)
        self.annot.get_bbox_patch().set_facecolor("white")
        self.annot.get_bbox_patch().set_alpha(0.4)


    def hover(self,event):
        vis = self.annot.get_visible()
        if event.inaxes == self.ax:
            cont, ind = self.sc.contains(event)
            if cont:
                self.update_annot(ind)
                self.annot.set_visible(True)
                self.fig.canvas.draw_idle()
            else:
                if vis:
                    self.annot.set_visible(False)
                    self.fig.canvas.draw_idle()
def tsp_visu(data):
    # reformat towns according to num_dim
    data['towns'] = np.array(data['towns']).reshape(-1, data['num_dim'])
    if data['num_dim'] > 2:
        print("Warning: the visualization is only for 2D towns, only showing the first two dimensions here")
    if data['num_dim'] < 2:
        print("Error: the visualization is only for 2D towns, but the towns have less than 2 dimensions")
        return
        
    # make an animation of the tours
    fig, ax = plt.subplots()
    sc = ax.scatter(data['towns'][:,0], data['towns'][:,1])
    Lquivers = []
    for tour in data['tours']:
        dataTour = data['towns'][tour]
        dataTourNext = np.roll(dataTour, -1, axis=0)
        Lquivers.append([dataTour[:,0], dataTour[:,1], dataTourNext[:,0]-dataTour[:,0], dataTourNext[:,1]-dataTour[:,1]])
    names = [f"{i}" for i in range(len(data['towns']))]
    annot = ax.annotate("", xy=(0,0), xytext=(5,5),textcoords="offset points",
                    bbox=dict(boxstyle="round", fc="w"),
                    arrowprops=dict(arrowstyle="->"))
    annot.set_visible(False)
    h = HoverManager(fig=fig, ax=ax, sc=sc, annot=annot, names=names)
    
    Q = ax.quiver(*Lquivers[0], angles='xy', scale_units='xy', width=0.005, scale=1)
    ax.set_title(data['annotations'][0])
    def update_quiver(i):
        # i = i%len(data['tours'])
        # update the data of the quiver changing the X, Y, U, V values
        Q.set_UVC(*Lquivers[i][2:])
        Q.set_offsets(np.array([Lquivers[i][0], Lquivers[i][1]]).T)
        ax.set_title(data['annotations'][i])
        return Q,
    n_tours = len(data['tours'])
    print("Number of tours: ", n_tours)
    anim = Player(fig, update_quiver, maxi=len(data['tours'])-1)
    plt.show()

def rainbow(n: int):
    """Make an HSV rainbow colormap with n colors"""
    # create n evenly spaced colors in HSV space hue from 0 to 1
    return [colorsys.hsv_to_rgb(i/n, 1, 1) for i in range(n)]
    
def clustering_visu(data):
    # reformat points according to num_dim
    data['points'] = np.array(data['points']).reshape(-1, data['num_dim'])
    if data['num_dim'] > 2:
        print("Warning: the visualization is only for 2D points, only showing the first two dimensions here")
    if data['num_dim'] < 2:
        print("Error: the visualization is only for 2D points, but the points have less than 2 dimensions")
        return
    # make the animation of the clustering
    # Scatter plot for the points and change color according to the cluster assigned
    colors_clusters = rainbow(np.unique(data['assignements']).shape[0])
    fig, ax = plt.subplots()
    sc = ax.scatter(data['points'][:,0], data['points'][:,1])
    def update_plot(i):
        sc.set_color([colors_clusters[c] for c in data['assignements'][i]])
        ax.set_title(data['annotations'][i])
        return sc,
    names = [f"{i}" for i in range(len(data['points']))]
    annot = ax.annotate("", xy=(0,0), xytext=(5,5),textcoords="offset points",
                    bbox=dict(boxstyle="round", fc="w"),
                    arrowprops=dict(arrowstyle="->"))
    annot.set_visible(False)
    h = HoverManager(fig=fig, ax=ax, sc=sc, annot=annot, names=names)
    ax.set_title(data['annotations'][0])
    anim = Player(fig, update_plot, maxi=len(data['assignements'])-1)
    plt.show()

def maxsat_visu(data):
    pass

if __name__ == "__main__":
    # argparse with one argument indicating the path to a json describing the sequence of actions
    ap = argparse.ArgumentParser()
    ap.add_argument("-f", "--file", required=False, help="path to the json file", default="actions.json")
    args = vars(ap.parse_args())
    with open(args["file"]) as f:
        actions = json.load(f)
    
    function_visu = {"tsp":tsp_visu, "clustering":clustering_visu, "maxsat":maxsat_visu}
    function_visu[actions["problem"]](actions)