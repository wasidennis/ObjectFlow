/*
Original Code From:
Copyright (C) 2006 Pedro Felzenszwalb
Modifications (may have been made) Copyright (C) 2011,2012 
  Chenliang Xu, Jason Corso.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/

/* Implements node merging criteria. */

#ifndef SEGMENT_GRAPH
#define SEGMENT_GRAPH

#include <algorithm>
#include <cmath>
#include <vector>

#include "disjoint-set.h"
#include "edges.h"

using namespace std;

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

bool operator<(const edge &a, const edge &b) {
	return a.w < b.w;
}

/* pixel level minimum spanning tree merge */
void segment_graph(universe *mess, vector<edge>* edges_remain, edge *edges, 
		int num_edges, float c, int level) {
	// new vector containing remain edges
	edges_remain->clear();
	// sort edges by weight
	sort(edges, edges + num_edges);
	// for each edge, in non-decreasing weight order...
	for (int i = 0; i < num_edges; i++) {
		edge *pedge = &edges[i];
		// components conected by this edge
		int a = mess->find_in_level(pedge->a, level);
		int b = mess->find_in_level(pedge->b, level);
		if (a != b) {
			// merging objective function
			if ((pedge->w <= mess->get_mst(a) + (c / mess->get_size(a)))
					&& (pedge->w <= mess->get_mst(b) + (c / mess->get_size(b)))) {
				if (mess->join(a, b, pedge->w, level) == 1)
					edges_remain->push_back(*pedge);
			} else {
				edges_remain->push_back(*pedge);
			}
		}
	}
}
	
/* region graph level minimum spanning tree merge */
void segment_graph_region(universe *mess, vector<edge>* edges_remain, 
		vector<edge>* edges_region, float c_reg, int level) {
	edges_remain->clear();
	sort(edges_region->begin(), edges_region->end());
	for (int i=0; i < (int) edges_region->size(); i++) {
		int a = mess->find_in_level(edges_region->at(i).a, level);
		int b = mess->find_in_level(edges_region->at(i).b, level);
		if (a != b) {
			if ((edges_region->at(i).w
					<= mess->get_mst(a) + (c_reg / mess->get_size(a)))
					&& (edges_region->at(i).w
							<= mess->get_mst(b) + (c_reg / mess->get_size(b)))) {
				if (mess->join(a, b, edges_region->at(i).w, level) == 1)
					edges_remain->push_back(edges_region->at(i));
			} else {
				edges_remain->push_back(edges_region->at(i));
			}
		}

	}
}
#endif /* SEGMENT_GRAPH_H */
