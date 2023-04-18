from graphviz import Digraph, Graph
import numpy as np
from typing import *
from rich.progress import track
import pyperclip
import re

encoding_t = Tuple[bool]


def swap_neighbours(solution_encoding: encoding_t) -> Generator[encoding_t, None, None]:
    for i in range(len(solution_encoding)):
        for j in range(i + 1, len(solution_encoding)):
            if solution_encoding[i] == solution_encoding[j]:
                continue
            neighbour = (
                solution_encoding[:i]
                + (not solution_encoding[i],)
                + solution_encoding[i + 1 : j]
                + (not solution_encoding[j],)
                + solution_encoding[j + 1 :]
            )
            yield neighbour


def one_flip_neighbours(
    solution_encoding: encoding_t,
) -> Generator[encoding_t, None, None]:
    for i in range(len(solution_encoding)):
        neighbour = (
            solution_encoding[:i]
            + (not solution_encoding[i],)
            + solution_encoding[i + 1 :]
        )
        yield neighbour


def exchange_neighbours(
    solution_encoding: encoding_t,
) -> Generator[encoding_t, None, None]:
    for i in range(len(solution_encoding)):
        for j in range(i + 1, len(solution_encoding)):
            if solution_encoding[i] == solution_encoding[j]:
                continue
            neighbour = (
                solution_encoding[:i]
                + (solution_encoding[j],)
                + solution_encoding[i + 1 : j]
                + (solution_encoding[i],)
                + solution_encoding[j + 1 :]
            )
            if neighbour == solution_encoding:
                continue
            yield neighbour


def move_two_bits_neighbours(
    solution_encoding: encoding_t,
) -> Generator[encoding_t, None, None]:
    for src_pos in range(len(solution_encoding)):
        for dst_pos in range(0, len(solution_encoding)):
            if src_pos == dst_pos:
                continue
            cpy_encoding = solution_encoding[:] + solution_encoding[:]
            neighbour = cpy_encoding[:src_pos] + cpy_encoding[src_pos + 2 :]
            neighbour = (
                neighbour[:dst_pos]
                + cpy_encoding[src_pos : src_pos + 2]
                + neighbour[dst_pos:]
            )
            neighbour = neighbour[: len(solution_encoding)]
            if neighbour == solution_encoding:
                continue
            yield neighbour


def swap_adjacent_bits_neighbours(
    solution_encoding: encoding_t,
) -> Generator[encoding_t, None, None]:
    for i in range(len(solution_encoding) - 1):
        neighbour = (
            solution_encoding[:i]
            + (solution_encoding[i + 1], solution_encoding[i])
            + solution_encoding[i + 2 :]
        )
        if neighbour == solution_encoding:
            continue
        yield neighbour

def flip_block_neighbours(
    solution_encoding: encoding_t,
) -> Generator[encoding_t, None, None]:
    block_size = 2
    for i in range(len(solution_encoding) - block_size + 1):
        neighbour = (
            solution_encoding[:i]
            + tuple(not x for x in solution_encoding[i : i + block_size])
            + solution_encoding[i + block_size :]
        )
        if neighbour == solution_encoding:
            continue
        yield neighbour

class Node:
    node_encoding_obj = {}
    links = set()

    def __init__(self, solution_encoding: encoding_t, cost: float, id: int) -> None:
        self.cost = cost
        self.solution_encoding = solution_encoding
        self.id = id
        Node.node_encoding_obj[str(solution_encoding)] = self
        self.children: List[Node] = []
        self.visited = {"fi": -1, "bi": -1}

    def make_node(self, g):
        encoding_as_nbr = "".join([str(int(x)) for x in self.solution_encoding])
        g.node(
            str(self.id),
            ""
            + "\\n".join(
                [
                    # f"{{id       | {self.id}}}",
                    f"{encoding_as_nbr}",
                    f"cost {self.cost}",
                ]
            )
            + "",
        )

    def neighbours(self):
        for neighbour in flip_block_neighbours(self.solution_encoding):
            try:
                yield Node.node_encoding_obj[str(neighbour)]
            except KeyError:
                pass

    def make_edges(self, g):
        for neighbour in self.neighbours():
            first, last = sorted([self.id, neighbour.id])
            if (first, last) not in Node.links:
                Node.links.add((first, last))
                g.edge(str(first), str(last))

    def make_links(self):
        for neighbour in self.neighbours():
            self.children.append(neighbour)


def clear(nodes: List[Node]):
    for n in nodes:
        n.visited = {"fi": -1, "bi": -1}


def fi(node: Node, iteration: int = 0):
    for child in node.children:
        if child.cost < node.cost:
            child.visited["fi"] = iteration + 1
            return fi(child, iteration + 1)
    return node


def bi(node: Node, iteration: int = 0):
    best = None
    for child in node.children:
        if child.cost < node.cost:
            best = child
    if best is not None:
        best.visited["bi"] = iteration + 1
        best = bi(best, iteration=iteration + 1)
    else:
        best = node
    return best

def generator():
    np.random.seed(0)
    i = 0
    for size_costs in track(range(4, 100), description="Size"):
        for _ in range(10):
            cost_vector = list(np.random.randint(1, 20, size=size_costs))
            print(f"Cost vector: {cost_vector}")
            # make all posibilities of the encoding with constant hamming sum
            Node.node_encoding_obj = {}
            nodes = []
            for i in range(2**size_costs):
                i_bin = bin(i)[2:].zfill(size_costs)
                i_bin = tuple([True if x == "1" else False for x in i_bin])
                cost = sum([cost_vector[i] for i in range(len(i_bin)) if i_bin[i]])
                nodes.append(Node(solution_encoding=i_bin, cost=cost, id=i))

            for n in nodes:
                n.make_links()
            g = Graph("G", format="png", strict=True)
            g.attr(rankdir= 'LR')
            g_dot = Graph("G", format="dot", strict=True)
            g_dot.attr(rankdir='LR')
            Node.links = set()
            for n in nodes:
                n.make_node(g)
                n.make_node(g_dot)
            for n in nodes:
                n.make_edges(g)
                n.make_edges(g_dot)
            yield g, g_dot
def selected_graph():
    cost_vector = np.array([4,8,10,19])
    size_costs = 4
    print(f"Cost vector: {cost_vector}")
    # make all posibilities of the encoding with constant hamming sum
    Node.node_encoding_obj = {}
    nodes = []
    for i in range(2**size_costs):
        i_bin = bin(i)[2:].zfill(size_costs)
        i_bin = tuple([True if x == "1" else False for x in i_bin])
        cost = sum([cost_vector[i] for i in range(len(i_bin)) if i_bin[i]])
        nodes.append(Node(solution_encoding=i_bin, cost=cost, id=i))

    for n in nodes:
        n.make_links()
    g = Graph("G", format="png", strict=True)
    g.attr(rankdir= 'LR')
    g_dot = Graph("G", format="dot", strict=True)
    g_dot.attr(rankdir= 'LR')
    Node.links = set()
    for n in nodes:
        n.make_node(g)
        n.make_node(g_dot)
    for n in nodes:
        n.make_edges(g)
        n.make_edges(g_dot)
    g.view()
    # copy to clipboard
    src = g.source
    pyperclip.copy(src)
if __name__ == "__main__":
    for i,(g,g_dot) in enumerate(generator()):
        g.view()
        # copy to clipboard
        src = g.source
        pyperclip.copy(src)
        b=0
    #selected_graph()
    b = 0
