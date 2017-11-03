# CPLEXNetworkSimplex
Using CPLEX's Network Simplex Algorithm for various graph algorithms

Data files are data0.txt through till data9.txt

Each data file has the following format

-------
source destination
# of arcs of graph(M)
FromNodeofArc1 ToNodeofArc1 CostofArc1
FromNodeofArc2 ToNodeofArc2 CostofArc2
...
FromNodeofArcM ToNodeofArcM CostofArcM
FromNodeofArc1 ToNodeofArc1 NewCostofArc1
FromNodeofArc2 ToNodeofArc2 NewCostofArc2
...
FromNodeofArcM ToNodeofArcM NewCostofArcM
-------

Thus, lines 3 through lines M-2 constitute one problem instance of the shortest path problem with source and destination as specified.
The next M lines constitute on the same graph, a newer problem instance of the shortest path problem.

