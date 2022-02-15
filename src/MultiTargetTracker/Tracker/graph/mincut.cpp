// $Id: mincut.cpp,v 1.1.1.1 2003/11/05 15:19:13 rdmp1c Exp $

#include "mincut.h"


#include <cstdlib>
#include <cassert>
#include <iostream>
#include <queue>
#include <list>
#include <map>
#include <set>
#include <set>
#include <limits>
#include <algorithm>

#include "fheap.h"

mincut::mincut () : GTL::algorithm () 
{
	set_vars_executed = false;
}


mincut::~mincut ()
{
}

void mincut::set_vars(const GTL::edge_map<int>& edge_weight)
{
    this->edge_weight = edge_weight;
	min_cut = 0;
    set_vars_executed = true;
}

int mincut::check (GTL::graph& G) 
{
	if (!set_vars_executed)
		return(GTL_ERROR);

    if ((G.number_of_nodes() <= 1) || (!G.is_connected()) || (G.is_directed()))
		return(GTL_ERROR);

    return GTL_OK;
}

void mincut::reset () 
{
	st_list.erase (st_list.begin(), st_list.end());
}

int mincut::run(GTL::graph& G)
{
	GTL::graph g;
    g.make_undirected();

    // Make a local copy of the graph as mincut modifies the original graph

	// List of nodes in the original graph
	GTL::node_map <GTL::node> partner (G);
	GTL::node_map <GTL::node> orig (g);

	GTL::node x;
	forall_nodes (x, G)
	{
		partner[x] = g.new_node(); 
		orig[partner[x]] = x; // so we can look up original node
	}

	// Create edges and associated weights
	GTL::edge_map<int> w(g, 0);
	GTL::edge e;
	forall_edges (e, G)
	{
		if (e.source() != e.target())
		{
			GTL::edge ec = g.new_edge (partner[e.source()], partner[e.target()]);
			w[ec] = edge_weight[e];
		}
	}

	// Start of algorithm. $a$ is an arbitrary single node in $g$. The set $A$
	// of nodes initially comprises $a$
	GTL::graph::node_iterator na = g.nodes_begin();
	GTL::node a = *na;
	int n = g.number_of_nodes();
	int cut_weight = std::numeric_limits<int>::max();
	int best_value = std::numeric_limits<int>::max();
	while (n >= 2 )
	{
		GTL::node t = a;
		GTL::node s, v;
		GTL::edge e;
   		GTL::node::adj_edges_iterator it;
		GTL::node::adj_edges_iterator end;
		
		fheap_t *pq = fh_alloc (n);
		GTL::node_map<int> vertex_number (g, 0);
		std::map <int, GTL::node, std::less<int> > nv;
		int vertex_count = 0;
			
		// Nodes in $A$ are not in the queue
		GTL::node_map<bool> in_PQ(g, false);
		forall_nodes (v, g)
		{
			vertex_number[v] = vertex_count;
			nv[vertex_count] = v;
			vertex_count++;
			if (v != a)
			{
				in_PQ[v] = true;
				fh_insert (pq, vertex_number[v], 0);	
			}
		}
		GTL::node_map<int> inf (g, 0);
		// Get weight of edges adjacent to $a$
		it = a.adj_edges_begin();
		end = a.adj_edges_end();
		while (it != end)
		{
			v = a.opposite (*it);
			inf[v] += w[*it];	
			it++;
		}
		// Store weights in a queue
		it = a.adj_edges_begin();
		end = a.adj_edges_end();
		while (it != end)
		{
			v = a.opposite (*it);
			fh_decrease_key (pq, vertex_number[v], -inf[v]);  
			it++;
		}

		while (pq->n > 0)
		{
			s = t;

			// Get the node that is most tightly connected to $A$
			t = nv[fh_delete_min (pq)];
			cut_weight = inf[t];
			in_PQ[t] = false;

			// Increase the key of nodes adjacent to t and not in $A$ by adding the
			// weights of edges connecting t with nodes not in $A$ 
			it = t.adj_edges_begin();
			end = t.adj_edges_end();
			while (it != end)
			{
				v = t.opposite (*it);
				if (in_PQ[v])
				{
					inf[v] += w[*it];
					fh_decrease_key (pq, vertex_number[v], -inf[v]);  
				}
				it++;
			}	
		}
		fh_free (pq);

		//cout << "   cut-of-the-phase = " << cut_weight << endl;
		
		if (cut_weight <= best_value)
		{
			if (cut_weight < best_value)
			{
				// Clear list of (s,t) pairs
				st_list.erase (st_list.begin(), st_list.end());
				best_value = cut_weight;
			}
			st_list.push_back (node_pair (orig[s], orig[t]));
		}

		// Nodes s and t are the last two nodes to be added to A
		//cout << "s=" << s << " t=" << t << endl;

		// Get list of edges adjacent to s
		GTL::edge dummy;
		GTL::node_map<GTL::edge> s_edge(g, dummy);
		it = s.adj_edges_begin();
		end = s.adj_edges_end();
		while (it != end)
		{
			s_edge[s.opposite(*it)] = *it;
			it++;
		}

		// Merge s and t
   		it = t.adj_edges_begin();
    	end = t.adj_edges_end();

		// Iterate over edges adjacent to t. If a node v adjacent to
		// t is also adjacent to s, then add w(it) to e(s,v)
		// otherwise make a new edge e(s,v)
		while (it != end)
		{
			v = t.opposite (*it);

			if (s_edge[v] != dummy)
			{
				w[s_edge[v]] += w[*it];
			}
			else if (s != v)
			{
				GTL::edge ne = g.new_edge (s, v);
				w[ne] = w[*it];
			}				
			++it;
		}

		// Delete node t from graph
		g.del_node(t);
		n--;
	}
	
	min_cut = best_value;

    return(GTL_OK);
}
