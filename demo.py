from matplotlib import pyplot as plt
import networkx as nx

graph = nx.Graph()

with open('model.out') as f:
    for line in f:
        line = line.split()
        if line:
            graph.add_edge(int(line[0]), int(line[1]))
        # end if
    # end for
# end with

with open('match.out') as f:
    matched = [int(i) for i in f.read().split()]
# end with
not_matched = [v for v in graph.nodes() if v not in matched]

pos = nx.shell_layout(graph)
plt.xticks([]), plt.yticks([])
nx.draw_networkx_nodes(graph, pos, nodelist=not_matched,
                       alpha=0.4, node_color='blue', node_size=1300)
nx.draw_networkx_nodes(graph, pos, nodelist=matched,
                       alpha=0.8, node_color='red', node_size=1300)
nx.draw_networkx_labels(graph, pos, font_family='sans-serif', font_size=15)
nx.draw_networkx_edges(graph, pos, width=1.5, alpha=0.5)
plt.show()
