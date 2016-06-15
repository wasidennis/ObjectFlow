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

/* This is the main implementation of the streaming graph-based
 * hierarchical segmentation algorithm.
*/

#ifndef SEGMENT_IMAGE_H
#define SEGMENT_IMAGE_H

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <unistd.h>

#include "image.h"
#include "misc.h"
#include "filter.h"
#include "pnmfile.h"

#include "edges.h"
#include "segment-graph.h"
#include "disjoint-set.h"

using namespace std;

/* random color */
rgb random_rgb() {
	rgb c;
	c.r = (uchar) random();
	c.g = (uchar) random();
	c.b = (uchar) random();
	return c;
}

// convert single int into unique rgb values
rgb newColor(int n){
	rgb c;
	c.r = ((n>>16) % 256);
	c.g = ((n>>8) % 256);
	c.b = (n % 256);
	return c;
}

/* Gaussian Smoothing */
void smooth_images(image<rgb> *im[], int num_frame, image<float> *smooth_r[],
		image<float> *smooth_g[], image<float> *smooth_b[], float sigma) {

	int width = im[0]->width();
	int height = im[0]->height();

	image<float>** r = new image<float>*[num_frame];
	image<float>** g = new image<float>*[num_frame];
	image<float>** b = new image<float>*[num_frame];
	for (int i = 0; i < num_frame; i++) {
		r[i] = new image<float>(width, height);
		g[i] = new image<float>(width, height);
		b[i] = new image<float>(width, height);
	}
	for (int i = 0; i < num_frame; i++) {
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				imRef(r[i], x, y) = imRef(im[i], x, y).r;
				imRef(g[i], x, y) = imRef(im[i], x, y).g;
				imRef(b[i], x, y) = imRef(im[i], x, y).b;
			}
		}
	}

	// smooth each color channel
	for (int i = 0; i < num_frame; i++) {
		smooth_r[i] = smooth(r[i], sigma);
		smooth_g[i] = smooth(g[i], sigma);
		smooth_b[i] = smooth(b[i], sigma);
	}
	for (int i = 0; i < num_frame; i++) {
		delete r[i];
		delete g[i];
		delete b[i];
	}
	delete[] r;
	delete[] g;
	delete[] b;
}

/* Save Output */
void generate_output(bool first, char *path, int frame_id_start,
		int frame_id_end, int width, int height, universe *mess,
		int num_vertices, int num_vertices_v, int level_total,
		int color_counter[]) {

	int num_save = frame_id_end - frame_id_start;
	int save_start = frame_id_start + 2;
	if (first == true) {
		num_save = frame_id_end - frame_id_start + 1;
		save_start = frame_id_start + 1;
	}

	char savepath[1024];
	image<rgb>** output = new image<rgb>*[num_save];
	rgb* colors=NULL;
	if(!color_counter[0]){
		colors = new rgb[num_vertices];
		for (int i = 0; i < num_vertices; i++){
			colors[i] = random_rgb();
		}
	}
	rgb blank;
	blank.r = blank.g = blank.b = 0;

	// write out the ppm files.
	for (int k = 0; k <= level_total; k++) {
		for (int i = 0; i < num_save; i++) {
			// add frame index 1 to save
			snprintf(savepath, 1023, "%s/%02d/%05d.ppm", path, k, save_start + i);
			output[i] = new image<rgb>(width, height);
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					int this_id = num_vertices_v + y * width + x
							+ i * (width * height);
					int comp = mess->find_in_level(this_id, k);
					if (mess->get_color_output(comp, k) == blank) {
						// color the segment if the segment is not colored yet
						if(!color_counter[0]){
							//random generation based on colors[]
							imRef(output[i], x, y) = colors[comp];
							mess->set_color_output(this_id, colors[comp], k);
							mess->set_color_output(comp, colors[comp], k);
						}
						else{
							//incremental/unique assignment of colors
							if(color_counter[k]%(256*256*256)==0){
								color_counter[k]++;
								printf("**WARNING** Color options have been "
								"exhausted. Segment colors will no longer be "
								"unique.\nlevel:%d frame:%d\n\n", k, save_start+i);
							}
							imRef(output[i], x, y) = newColor(color_counter[k]);
							mess->set_color_output(this_id, newColor(color_counter[k]), k);
							mess->set_color_output(comp, newColor(color_counter[k]), k);
							color_counter[k]++;
						}
					} else {
						// use the segment color if it is already colored
						imRef(output[i], x, y) = mess->get_color_output(comp, k);
						mess->set_color_output(this_id, mess->get_color_output(comp, k), k);
					}
				}
			}
			savePPM(output[i], savepath);
		}
		for (int i = 0; i < num_save; i++)
			delete output[i];
	}
	delete[] output;

}

/* main operation steps in one iteration */
universe *segment_image(char *path, image<rgb> *im[], int frame_id_start,
		int frame_id_end, float c, float c_reg, int min_size, float sigma,
		int hie_num, universe *v, int color_counter[]) {

	// step 1 -- Get information
	// ----- width, heigh, frame number
	int width = im[0]->width();
	int height = im[0]->height();
	int num_frame = frame_id_end - frame_id_start + 1;

	// ----- first or not
	bool first;
	if (frame_id_start == 0)
		first = true;
	else
		first = false;
	// ----- node number
	int num_vertices = num_frame * width * height;
	int num_vertices_v = width * height;
	// ----- edge number
	int num_edges_plane = (width - 1) * (height - 1) * 2 + width * (height - 1)
			+ (width - 1) * height;
	int num_edges_layer = (width - 2) * (height - 2) * 9 + (width - 2) * 2 * 6
			+ (height - 2) * 2 * 6 + 4 * 4;
	int num_edges = num_edges_plane * (num_frame - 1)
			+ num_edges_layer * (num_frame - 1);
	// if it's first
	if (first == true) {
		num_edges = num_edges_plane * num_frame
				+ num_edges_layer * (num_frame - 1);
		num_vertices_v = 0;
	}
	// ----- hierarchy setup
	vector<vector<edge>*> edges_region;
	edges_region.resize(hie_num + 1);

	// ------------------------------------------------------------------

	// step 2 -- smooth images
	image<float>** smooth_r = new image<float>*[num_frame];
	image<float>** smooth_g = new image<float>*[num_frame];
	image<float>** smooth_b = new image<float>*[num_frame];
	smooth_images(im, num_frame, smooth_r, smooth_g, smooth_b, sigma);
	// ------------------------------------------------------------------

	// step 3 -- build edges
	printf("start build edges\n");
	edge* edges = new edge[num_edges];
	initialize_edges(edges, first, num_frame, width, height, smooth_r, smooth_g,
			smooth_b);
	printf("end build edges\n");
	// ------------------------------------------------------------------

	// step 4 -- build nodes
	printf("start build nodes\n");
	universe* mess = new universe(frame_id_start, frame_id_end, width, height,
			*v, num_vertices_v, smooth_r, smooth_g, smooth_b, hie_num);
	printf("end build nodes\n");
	// ------------------------------------------------------------------

	// step 5 -- over-segmentation
	printf("start over-segmentation\n");
	edges_region[0] = new vector<edge>();
	segment_graph(mess, edges_region[0], edges, num_edges, c, 0);
	// optional merging small components
	for (int i = 0; i < num_edges; i++) {
		int a = mess->find_in_level(edges[i].a, 0);
		int b = mess->find_in_level(edges[i].b, 0);
		if ((a != b)
				&& ((mess->get_size(a) < min_size)
						|| (mess->get_size(b) < min_size)))
			mess->join_noise(a, b, 0);
	}
	printf("end over-segmentation\n");
	// ------------------------------------------------------------------

	// step 6 -- hierarchical segmentation
	for (int i = 0; i < hie_num; i++) {
		printf("level = %d\n", i);
		
		// incremental in each hierarhcy
		min_size = min_size * 1.2;

		printf("start update\n");
		mess->update(i);
		printf("end update\n");

		printf("start fill edge weight\n");
		fill_edge_weight(*edges_region[i], mess, i);
		printf("end fill edge weight\n");

		printf("start segment graph region\n");
		edges_region[i + 1] = new vector<edge>();
		segment_graph_region(mess, edges_region[i + 1], edges_region[i], c_reg, i + 1);
		printf("end segment graph region\n");

		printf("start merging min_size\n");
		for (int it = 0; it < (int) edges_region[i]->size(); it++) {
			int a = mess->find_in_level((*edges_region[i])[it].a, i + 1);
			int b = mess->find_in_level((*edges_region[i])[it].b, i + 1);
			if ((a != b) && ((mess->get_size(a) < min_size) || (mess->get_size(b) < min_size)))
				mess->join_noise(a, b, i + 1);
		}
		printf("end merging min_size\n");

		// incremental in each hierarchy
		c_reg = c_reg * 1.4;
		delete edges_region[i];
	}
	delete edges_region[hie_num];
	// ------------------------------------------------------------------

	// step 8 -- generate output
	printf("start output\n");
	generate_output(first, path, frame_id_start, frame_id_end, width, height,
			mess, num_vertices, num_vertices_v, hie_num, color_counter);
	printf("end output\n");
	// ------------------------------------------------------------------

	// step 9 -- clear everything and return u contains last frame info
	universe *u = new universe(*mess, num_vertices, width * height);
	delete mess;
	delete[] edges;
	for (int i = 0; i < num_frame; i++) {
		delete smooth_r[i];
		delete smooth_g[i];
		delete smooth_b[i];
	}
	delete[] smooth_r;
	delete[] smooth_g;
	delete[] smooth_b;
	return u;

}

#endif /* SEGMENT_IMAGE_H */
