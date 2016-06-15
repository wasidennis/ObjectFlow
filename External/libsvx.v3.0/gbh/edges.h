/*
Original Code From:
Copyright (C) 2006 Pedro Felzenszwalb
Modifications (may have been made) Copyright (C) 2011, 2012
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

/* Implements the edge data structure. */

#ifndef EDGES_H
#define EDGES_H

#include <vector>

#include "image.h"
#include "disjoint-set.h"
#include "histogram.h"


using namespace std;

/* define a edge */
typedef struct {
	float w;
	int a, b;
} edge;


/* fill pixel level edges */
void generate_edge(edge *e, image<float> *r_v, image<float> *g_v,
		image<float> *b_v, image<float> *r_u, image<float> *g_u,
		image<float> *b_u, int x_v, int y_v, int z_v, int x_u, int y_u,
		int z_u) {
	int width = r_v->width();
	int height = r_v->height();

	e->a = y_v * width + x_v + z_v * (width * height);
	e->b = y_u * width + x_u + z_u * (width * height);
	e->w = sqrt(
			square(imRef(r_v, x_v, y_v) - imRef(r_u, x_u, y_u))
					+ square(imRef(g_v, x_v, y_v) - imRef(g_u, x_u, y_u))
					+ square(imRef(b_v, x_v, y_v) - imRef(b_u, x_u, y_u)));

}

/* fill region graph edges */
void fill_edge_weight(vector<edge>& edges_region, universe *mess, int level) {
	for (int i = 0; i < ((int) edges_region.size()); i++) {
		int a = edges_region[i].a;
		int b = edges_region[i].b;
		int a_p = mess->find_in_level(a, level);
		int b_p = mess->find_in_level(b, level);
		if (a_p != b_p) {
			edges_region[i].w = mess->get_His_L(a_p)->chiSquared(*mess->get_His_L(b_p))
					+ mess->get_His_a(a_p)->chiSquared(*mess->get_His_a(b_p))
					+ mess->get_His_b(a_p)->chiSquared(*mess->get_His_b(b_p));
		} else {
			edges_region[i].w = 0;
		}
	}
}

/* initialize pixel level edges */
void initialize_edges(edge *edges, int num_frame, int width, int height,
		image<float> *smooth_r[], image<float> *smooth_g[],
		image<float> *smooth_b[]) {

	int num_edges = 0;
	for (int z = 0; z < num_frame; z++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				// in the same plane
				if (x < width - 1) {
					generate_edge(&edges[num_edges], smooth_r[z], smooth_g[z],
							smooth_b[z], smooth_r[z], smooth_g[z], smooth_b[z],
							x + 1, y, z, x, y, z);
					num_edges++;
				}
				if (y < height - 1) {
					generate_edge(&edges[num_edges], smooth_r[z], smooth_g[z],
							smooth_b[z], smooth_r[z], smooth_g[z], smooth_b[z],
							x, y + 1, z, x, y, z);
					num_edges++;
				}
				if ((x < width - 1) && (y < height - 1)) {
					generate_edge(&edges[num_edges], smooth_r[z], smooth_g[z],
							smooth_b[z], smooth_r[z], smooth_g[z], smooth_b[z],
							x + 1, y + 1, z, x, y, z);
					num_edges++;
				}
				if ((x < width - 1) && (y > 0)) {
					generate_edge(&edges[num_edges], smooth_r[z], smooth_g[z],
							smooth_b[z], smooth_r[z], smooth_g[z], smooth_b[z],
							x + 1, y - 1, z, x, y, z);
					num_edges++;
				}

				// to the previous plane
				if (z > 0) {

					generate_edge(&edges[num_edges], smooth_r[z - 1],
							smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
							smooth_g[z], smooth_b[z], x, y, z - 1, x, y, z);
					num_edges++;

					if (x > 0 && x < width - 1 && y > 0 && y < height - 1) {
						// additional 8 edges
						// x - 1, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y - 1, z - 1,
								x, y, z);
						num_edges++;
						// x, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x, y - 1, z - 1, x, y,
								z);
						num_edges++;
						// x + 1, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y - 1, z - 1,
								x, y, z);
						num_edges++;
						// x - 1, y
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y, z - 1, x, y,
								z);
						num_edges++;
						// x + 1, y
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y, z - 1, x, y,
								z);
						num_edges++;
						// x - 1, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y + 1, z - 1,
								x, y, z);
						num_edges++;
						// x, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x, y + 1, z - 1, x, y,
								z);
						num_edges++;
						// x + 1, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y + 1, z - 1,
								x, y, z);
						num_edges++;
					} else if (x == 0 && y > 0 && y < height - 1) {
						// additional 5 edges
						// x, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x, y - 1, z - 1, x, y,
								z);
						num_edges++;
						// x + 1, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y - 1, z - 1,
								x, y, z);
						num_edges++;
						// x + 1, y
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y, z - 1, x, y,
								z);
						num_edges++;
						// x, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x, y + 1, z - 1, x, y,
								z);
						num_edges++;
						// x + 1, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y + 1, z - 1,
								x, y, z);
						num_edges++;
					} else if (x == width - 1 && y > 0 && y < height - 1) {
						// additional 5 edges
						// x - 1, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y - 1, z - 1,
								x, y, z);
						num_edges++;
						// x, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x, y - 1, z - 1, x, y,
								z);
						num_edges++;
						// x - 1, y
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y, z - 1, x, y,
								z);
						num_edges++;
						// x - 1, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y + 1, z - 1,
								x, y, z);
						num_edges++;
						// x, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x, y + 1, z - 1, x, y,
								z);
						num_edges++;
					} else if (y == 0 && x > 0 && x < width - 1) {
						// additional 5 edges
						// x - 1, y
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y, z - 1, x, y,
								z);
						num_edges++;
						// x + 1, y
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y, z - 1, x, y,
								z);
						num_edges++;
						// x - 1, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y + 1, z - 1,
								x, y, z);
						num_edges++;
						// x, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x, y + 1, z - 1, x, y,
								z);
						num_edges++;
						// x + 1, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y + 1, z - 1,
								x, y, z);
						num_edges++;
					} else if (y == height - 1 && x > 0 && x < width - 1) {
						// additional 5 edges
						// x - 1, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y - 1, z - 1,
								x, y, z);
						num_edges++;
						// x, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x, y - 1, z - 1, x, y,
								z);
						num_edges++;
						// x + 1, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y - 1, z - 1,
								x, y, z);
						num_edges++;
						// x - 1, y
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y, z - 1, x, y,
								z);
						num_edges++;
						// x + 1, y
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y, z - 1, x, y,
								z);
						num_edges++;
					} else if (x == 0 && y == 0) {
						// additional 3 edges
						// x + 1, y
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y, z - 1, x, y,
								z);
						num_edges++;
						// x, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x, y + 1, z - 1, x, y,
								z);
						num_edges++;
						// x + 1, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y + 1, z - 1,
								x, y, z);
						num_edges++;
					} else if (x == 0 && y == height - 1) {
						// additional 3 edges
						// x, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x, y - 1, z - 1, x, y,
								z);
						num_edges++;
						// x + 1, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y - 1, z - 1,
								x, y, z);
						num_edges++;
						// x + 1, y
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x + 1, y, z - 1, x, y,
								z);
						num_edges++;
					} else if (x == width - 1 && y == 0) {
						// additional 3 edges
						// x - 1, y
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y, z - 1, x, y,
								z);
						num_edges++;
						// x - 1, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y + 1, z - 1,
								x, y, z);
						num_edges++;
						// x, y + 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x, y + 1, z - 1, x, y,
								z);
						num_edges++;
					} else if (x == width - 1 && y == height - 1) {
						// additional 3 edges
						// x - 1, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y - 1, z - 1,
								x, y, z);
						num_edges++;
						// x, y - 1
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x, y - 1, z - 1, x, y,
								z);
						num_edges++;
						// x - 1, y
						generate_edge(&edges[num_edges], smooth_r[z - 1],
								smooth_g[z - 1], smooth_b[z - 1], smooth_r[z],
								smooth_g[z], smooth_b[z], x - 1, y, z - 1, x, y,
								z);
						num_edges++;
					}

				}
			}
		}
	}

//	printf("num_edges = %d\n", num_edges);
}

#endif /* EDGES_H */
