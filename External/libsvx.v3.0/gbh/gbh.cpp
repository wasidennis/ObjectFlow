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

#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "image.h"
#include "pnmfile.h"
#include "segment-image.h"
#include "disjoint-set.h"

int main(int argc, char **argv) {
	if (argc != 8) {
		printf("%s c c_reg min sigma hie_num input output\n", argv[0]);
		printf("       c --> value for the threshold function in over-segmentation\n");
		printf("   c_reg --> value for the threshold function in hierarchical region segmentation\n");
		printf("     min --> enforced minimum supervoxel size\n");
		printf("   sigma --> variance of the Gaussian smoothing.\n");
		printf(" hie_num --> desired number of hierarchy levels\n");
		printf("   input --> input path of ppm video frames\n");
		printf("  output --> output path of segmentation results\n");
		return 1;
	}

	// Read Parameters
	float c = atof(argv[1]);
	float c_reg = atof(argv[2]);
	int min_size = atoi(argv[3]);
	float sigma = atof(argv[4]);
	int hie_num = atoi(argv[5]);
	char* input_path = argv[6];
	char* output_path = argv[7];
	if (c <= 0 || c_reg < 0 || min_size < 0 || sigma < 0 || hie_num < 0) {
		fprintf(stderr, "Unable to use the input parameters.");
		return 1;
	}

	// count files in the input directory
	int frame_num = 0;
	struct dirent* pDirent;
	DIR* pDir;
	pDir = opendir(input_path);
	if (pDir != NULL) {
		while ((pDirent = readdir(pDir)) != NULL) {
			int len = strlen(pDirent->d_name);
			if (len >= 4) {
				if (strcmp(".ppm", &(pDirent->d_name[len - 4])) == 0)
					frame_num++;
			}
		}
	}
	if (frame_num == 0) {
		fprintf(stderr, "Unable to find video frames at %s", input_path);
		return 1;
	}
	printf("Total number of frames in fold is %d\n", frame_num);


	// make the output directory
	struct stat st;
	int status = 0;
	char savepath[1024];
  	snprintf(savepath,1023,"%s",output_path);
	if (stat(savepath, &st) != 0) {
		/* Directory does not exist */
		if (mkdir(savepath, S_IRWXU) != 0) {
			status = -1;
		}
	}
	for (int i = 0; i <= hie_num; i++) {
  		snprintf(savepath,1023,"%s/%02d",output_path,i);
		if (stat(savepath, &st) != 0) {
			/* Directory does not exist */
			if (mkdir(savepath, S_IRWXU) != 0) {
				status = -1;
			}
		}
	}
	if (status == -1) {
		fprintf(stderr,"Unable to create the output directories at %s",output_path);
		return 1;
	}


	// Initialize Parameters
	image<rgb>** images = new image<rgb>*[frame_num];
	char filepath[1024];

	// Time Recorder
	time_t Start_t, End_t;
	int time_task;
	Start_t = time(NULL);

	// Read Frames
	for (int i = 0; i < frame_num; i++) {
		snprintf(filepath, 1023, "%s/%05d.ppm", input_path, i + 1);
		images[i] = loadPPM(filepath);
		printf("load --> %s\n", filepath);
	}

	// segmentation
	segment_image(output_path, images, frame_num, c, c_reg, min_size, sigma, hie_num);

	// Time Recorder
	End_t = time(NULL);
	time_task = difftime(End_t, Start_t);
	std::ofstream myfile;
	char timefile[1024];
	snprintf(timefile, 1023, "%s/%s", output_path, "time.txt");
	myfile.open(timefile);
	myfile << time_task << endl;
	myfile.close();

	printf("Congratulations! It's done!\n");
	printf("Time_total = %d seconds\n", time_task);
	return 0;
}
