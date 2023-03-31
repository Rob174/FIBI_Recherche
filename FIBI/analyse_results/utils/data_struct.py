import collections
from FIBI.analyse_results.utils.__init__ import *
from itertools import combinations
"""-------------------------------------------Build a hierarchy------------------------------"""
class Node:
    """Node of a tree.
    
    **value**: *Optional[dict] = None*, the value contained into the node. Is None if it is the root Node
    
    **id**: *int*, unique identifier
    
    """
    def __init__(self, value: Optional[dict] = None, id: int = 0):
        self.value: Optional[dict] = value
        self.children: List[Node] = []
        """*List[Node]* Child nodes"""
        self.id = id
        self.parent: Optional[Node] = None

    def total_leaves(self) -> int:
        """Total number of leaves under a arbitrary node"""
        if len(self.children) == 0:
            return 1
        tot = 0
        for c in self.children:
            tot += c.total_leaves()
        return tot

    def get_parents(self) -> List["Node"]:
        """Get parents in hierarchical order """
        if self.parent is None:
            return []
        return self.parent.get_parents() + [self]

class NodeStat(TypedDict):
    value: Any
    n_child: int
    idx: Optional[int]
    first_child: Literal['true','false']
Branch = List[NodeStat]
Level = List[NodeStat]
class Hierarchy:
    def __init__(
        self, poss: List[dict], order: Optional[List[str]] = None
    ):
        if order is None:
            order = list(poss[0].keys())
        root = Node()
        for data_poss in poss:
            curr_node = root
            for attr_name in order:
                value = {attr_name: data_poss[attr_name]}
                if value not in [n.value for n in curr_node.children]:
                    n = Node(value, id=len(curr_node.children))
                    curr_node.children.append(n)
                    n.parent = curr_node
                curr_node = next(n for n in curr_node.children if n.value == value)
        self.elems_branch = list(self._get_branches(root,duplicates=True))
        self.elems_branch_no_dupl = list(self._get_branches(root,duplicates=False))
        self.elems_lvls = list(self._get_levels(root))
    def __call__(self, orient: Literal['branch-dupl','branch-no-dupl','level']) -> List[Union[Branch, Level]]:
        if orient == 'branch-dupl':
            elems = self.elems_branch
        elif orient == 'level':
            elems = self.elems_lvls
        elif orient == 'branch-no-dupl':
            elems = self.elems_branch_no_dupl
        else:
            raise KeyError
        return elems
    def __str__(self):
        txt = 'Hierarchy<depth: %d,width %d>\n' % (self.depth(), self.width())
        txt += 'Branches:\n'
        tab = '    '
        lines = tab+('\n'+tab).join([str([a['value'] for a in b]) for b in self.elems_branch])
        return txt + lines
    def width(self):
        return len(self.elems_branch)
    def depth(self):
        return len(self.elems_lvls)
    def _get_branches(
        self, node: Optional[Node], parent_branch: Optional[Branch] = None, duplicates: bool = True
    ) -> Iterable[Branch]:
        if node is None:
            raise Exception("Empty hierarchy")
        if parent_branch is None:
            parent_branch = []
        if len(node.children) == 0:
            yield parent_branch
        for i,c in enumerate(node.children):
            child_branch: Branch = [{"value":c.value,"n_child":c.total_leaves()}] #type: ignore
            if duplicates or i == 0:
                child_branch = parent_branch[:] + child_branch
                for i in range(len(child_branch)):
                    child_branch[i]['first_child'] = 'true'
            else:
                for i in range(len(child_branch)):
                    child_branch[i]['first_child'] = 'false'
                
            yield from self._get_branches(c, child_branch,duplicates=duplicates)

    def _get_levels(self, node: Node) -> Iterable[Level]:
        levels = {}

        def iterate_tree(node: Optional[Node], curr_level=0):
            if node is None:
                return
            if curr_level not in levels:
                levels[curr_level] = []
            if node.value is not None:
                levels[curr_level].append({"value":node.value,"n_child":node.total_leaves(),'first_child':'true' if len(levels[curr_level]) == 0 else 'false'})
            for c in node.children:
                iterate_tree(c, curr_level + 1)

        iterate_tree(node)
        for i in sorted(levels):
            if len(levels[i]) > 0:
                yield levels[i]
class TableElem(TypedDict):
    type_elem: Literal['header-row','header-col','header','data','header-row-name','header-col-name']
    content: Any
    width: int
    height: int
    first_child: Literal['true','false']
    
def same_dict(d1,d2) -> bool:
    for k in set(d1.keys()).union(set(d2.keys())):
        if k not in d1 or k not in d2:
            return False
        if type(d1[k]) != type(d2[k]):
            return False
        if isinstance(d1[k],dict):
            return same_dict(d1[k],d2[k])
        if isinstance(d1[k],list):
            return len(set(d1[k]).symmetric_difference(set(d2[k]))) == 0
        if d1[k] != d2[k]:
            return False
    return True
def contains_dict(d1,d2) -> bool:
    """Check if d1 contains d2"""
    for k in set(d2.keys()):
        if k not in d1:
            return False
        if isinstance(d1[k],dict):
            return same_dict(d1[k],d2[k])
        if isinstance(d1[k],list):
            return len(set(d1[k]).difference(set(d2[k]))) == 0
        if d1[k] != d2[k]:
            return False
    return True

def all_same_dict(l_d:List[dict]) -> bool:
    for d1,d2 in combinations(l_d,2):
        if not same_dict(d1,d2):
            return False
    return True
    
def same_lists_without_order(lists: List[List[Union[float,str,int]]]) -> bool:
    sets = [set(l) for l in lists]
    for s1,s2 in combinations(sets,2):
        if len(s1.symmetric_difference(s2)) > 0:
            return False
    return True
class Table(dict):
    def __init__(self,rows_poss: Hierarchy,cols_poss: Hierarchy, mappings_attr_names: Dict[str,str]):
        self.rows_poss: Hierarchy = rows_poss
        self.cols_poss: Hierarchy = cols_poss
        self.mappings_attr_names = mappings_attr_names
        self.n_rows = rows_poss.width()
        self.n_cols = cols_poss.width()
        
        rows = self.rows_poss(orient='branch-dupl')
        cols = self.cols_poss(orient='branch-dupl')
        self.cols = cols
        self.rows = rows
        self.idx_mapping_rows = [{'idx':i,'elem':flatten_list_of_dict([a['value'] for a in e])} for i,e in enumerate(rows)] #type: ignore
        self.idx_mapping_cols = [{'idx':i,'elem':flatten_list_of_dict([a['value'] for a in e])} for i,e in enumerate(cols)] #type: ignore
        self.table = [[None for _ in range(self.n_cols)] for _ in range(self.n_rows)]
        
    def map(self,key,field: Literal['idx','n_child']) -> Tuple[int,int]:
        i = next(e for e in self.idx_mapping_rows if same_dict(e['elem'],key[0]))[field]
        j = next(e for e in self.idx_mapping_cols if same_dict(e['elem'],key[1]))[field]
        return i,j
    def get_lines(self) -> Iterable[dict]:
        for l in self.idx_mapping_rows:
            yield l['elem']
    def get_cols(self) -> Iterable[dict]:
        for l in self.idx_mapping_cols:
            yield l['elem']
    def __setitem__(self,key: Tuple[dict,dict],value: Any):
        i,j = self.map(key,'idx')
        self.table[i][j] = value
    def __getitem__(self,key):
        i,j = self.map(key,'idx')
        return self.table[i][j]
    def __str__(self):
        Lelem = []
        for r in self.iter_rows():
            L = []
            for e in r:
                if isinstance(e['content'],dict):
                    txt = ':'.join([list(map(str,a)) for a in e['content'].items()][0])
                else:
                    txt = str(e['content'])
                    
                L.append({"txt":txt,'width':e['width'],'height':e['height']})
            Lelem.append(L)
        size_cell = max(max(len(l2['txt']) for l2 in l1) for l1 in Lelem) + 2
        if size_cell % 2 == 0:
            size_cell += 1
        for i in range(len(Lelem)):
            for j in range(len(Lelem[i])):
                txt = ' '+Lelem[i][j]['txt']+' '
                padding = ((Lelem[i][j]['width'] * size_cell + (Lelem[i][j]['width']-1))-len(txt))
                padding_before  = padding // 2
                padding_after = padding-padding_before
                new_txt = ' '*int(padding_before)+txt+' '*padding_after
                if Lelem[i][j]['width'] == 1:
                    assert len(new_txt) == size_cell
                Lelem[i][j]['txt'] = new_txt
        lines = ['|'+'|'.join([a['txt'] for a in e])+'|' for e in Lelem]
        max_size = max(len(a) for a in lines)
        return ('_'*max_size+'\n')+'\n'.join(lines)+'\n'+('-'*max_size)

    def __repr__(self):
        return self.__str__()
    def __delitem__(self, __key) -> None:
        raise Exception('Not supported')
    def __len__(self):
        return self.n_rows
    @property
    def shape(self) -> Tuple[int,int]:
        return (self.n_rows,self.n_cols)
    def iter_rows(self) -> Iterable[List[TableElem]]:
        for i,c in enumerate(self.cols_poss(orient='level')):
            L: List[TableElem] = []
            if i == 0:
                for e in [list(e[0]['value'].keys())[0] for e in self.rows_poss(orient='level')]:
                    L.append({'type_elem': 'header-row-name','height':max(1,self.cols_poss.depth()),'width':1,'content':self.mappings_attr_names[e],'first_child':'false'})
            e = list(c[0]['value'].keys())[0]
            L.append({'type_elem': 'header-col-name','height':1,'width':1,'content':self.mappings_attr_names[e],'first_child':'false'})
            for e in c:
                L.append({'type_elem': 'header-col','height':1,'width':max(1,e["n_child"]),'content':e['value'],'first_child':e['first_child']})
            yield L
        l_l_header = self.rows_poss(orient='branch-no-dupl')
        for l,l_header in zip(self.rows_poss(orient='branch-dupl'),l_l_header):
            l: Branch
            i_row = flatten_list_of_dict([a['value'] for a in l])
            L = []
            for i,e in enumerate(l_header):
                L.append({'type_elem': 'header-row','height':max(1,e["n_child"]),'width':2 if i==len(l_header)-1 else 1,'content':e['value'],'first_child':e['first_child']})
                
            for c in self.cols_poss(orient='branch-dupl'):
                c: Branch
                i_col = flatten_list_of_dict([a['value'] for a in c])
                value = self[i_row,i_col]
                L.append({'type_elem': 'data','height':1,'width':1,'content':value,'first_child':'false'})
            yield L
                
def flatten_list_of_dict(l: List[dict]) -> dict:
    D = {}
    for d in l:
        D.update(d.items())
    return D
if __name__ == "__main__":
    poss_rows = [
        {"v1":0,"v2":0,"v3":0},
        {"v1":0,"v2":0,"v3":1},
        {"v1":0,"v2":1,"v3":2},
        {"v1":1,"v2":1,"v3":2}
    ]
    poss_cols = [
        {"v3":0,"v4":0,"v5":0},
        {"v3":0,"v4":0,"v5":1},
        {"v3":0,"v4":1,"v5":2},
        {"v3":1,"v4":1,"v5":2},
        {"v3":1,"v4":1,"v5":3}
    ]
    poss_cols = [
        {"v3":0},
        {"v3":1}
    ]
    h_rows = Hierarchy(poss_rows)
    h_cols = Hierarchy(poss_cols)
    t = Table(h_rows,h_cols)
    print(t)
    b=0