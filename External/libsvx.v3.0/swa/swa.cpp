/**
 * swa.cpp
 *
 * RELEASED WITH THE SWA PART OF LIBSVX.
 *
 * THIS CODE IS RELEASED UNDER THE MIT-LIKE LICENSE INCLUDED
 * WITH THE SOURCE CODE.
 *
 * CONTACT jcorso@buffalo.edu FOR ANY QUESTIONS.
 * 
 *
 */

#define DEBUG

// headers files from current project
#include "config.h"
#include "graphs.h"
#include "imagegraphs.h"
#include "vizhierarchy.h"

// C libraries
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>

// C++ libraries
#include <iostream>
#include <fstream>
#include <sstream>

// VPML libraries
#include "images.h"

using namespace std;
namespace IM = vpml_images;
namespace GR = vpml_graphs;


int main(int argc, char * argv[]) {

    char name[MAX_PATH_LENGTH];

    // variables for recording system run time
    double timeSystemInitialized = 0, timeSystemTerminated = 0, elapsedTime = 0;

    // variables for the parameters in the config file
    GR::Index* frames = NULL;
    int numberOfLayers = 0;
    int maxStCubeSize = 0;

    GR::Index* vizLayers = NULL;

    string inputSequence = "";
    string outputSequence = "";
    string vizFileName = "";

    // variables used throughout all frames of the sequence
    GR::GraphHierarchy* H = NULL;
    IM::RGBImage2** I = NULL; //pointer to the spatiotemporal cube
    int currentStCubeSize = 0;
    unsigned char* lut = NULL;
    GR::VisualizeHierarchy* vizHierarchy = NULL;

    // ScalarImage2 and RGB Image2 should inherit from the same base class in
    // in the future for polymorphism to . Having to declare two types of pointers is ugly.
    IM::RGBImage2*** arrayOfResults = NULL;

    char currentFrameName[MAX_PATH_LENGTH];


    if (argc != 2) // argc should be 2 for correct execution
    {
        // argv[0] is the program name
        cout << "\nUsage: " << argv[0] << " <config file name> \n\n";
        return 1;
    } else {
        // argv[1], the first parameter following the program name,
        // is the config file name to open.
        ifstream config_file(argv[1]);
        // Always check to see if file opening succeeded
        if (!config_file.is_open()) {
            cout << "\nCould not open config file \n\n";
            return 1;
        }
    }

    // Load the config file
    GR::Config config(argv[1]);
    cout << config << endl;
    string tempstr;

    // parse the parameters from the config file
    try {
        
        // the following 2 parameters need to be dummy-proofed in the future
        if ((tempstr = config.get("InputSequence")) != "")
            inputSequence = tempstr;
        if ((tempstr = config.get("OutputSequence")) != "")
            outputSequence = tempstr;
        if ((tempstr = config.get("Frames")) != "") {
            frames = new GR::Index();
            frames->parseString(tempstr);
        }
        if ((tempstr = config.get("NumOfLayers")) != "") {
            numberOfLayers = atoi(tempstr.c_str());
            if (numberOfLayers < 2)
                throw "NumOfLayers";
        }
        if ((tempstr = config.get("MaxStCubeSize")) != "") {
            maxStCubeSize = atoi(tempstr.c_str());
            if (maxStCubeSize < 1)
                throw "MaxStCubeSize";
        }
        if ((tempstr = config.get("VizLayer")) != "") {
            vizLayers = new GR::Index();
            vizLayers->parseString(tempstr);
        }
        if ((tempstr = config.get("VizFileName")) != "")
            vizFileName = tempstr;
    } catch (char const* e) {
        cerr << "Error in loading parameter " << e << " in Config.txt!" << endl;
    }
    cout << "\nAll parameters in " << argv[1] << " successfully parsed! \n\n";


    timeSystemInitialized = clock();

    //// Declare the objects that stay unchanged throughout frames ////

    // I is used to store the input data.
    // I[n] points to a frames of a spatiotemporal cube.
    I = new IM::RGBImage2*[maxStCubeSize];


    // CVPR2012 
    //   Changing it so that it is a unique rgb entry in the lut for 
    //   each possible segment.
    // generate a fixed lookup table that's shared by all frames
    // for painting the nodes
	sprintf(currentFrameName, inputSequence.c_str(), (*frames)[0]);
	I[0] = IM::load_image_rgb(currentFrameName);

	lut = new unsigned char[I[0]->W() * I[0]->H() * maxStCubeSize * 3];
	memset(lut,0,I[0]->W() * I[0]->H() * maxStCubeSize * 3);
	srand48((unsigned) time(0));
	for (int j = 0; j < (I[0]->W() * I[0]->H() * maxStCubeSize * 3); j++)
		lut[j] = (unsigned char) (20 + (drand48() * 200));

	delete I[0];
	I[0] = NULL;

    ///// Build/store the hierarchies for all frames  ////
    GR::SwaEdgeAffinityGraphCoarsenerRGB* graphCoarsener =
    		new GR::SwaEdgeAffinityGraphCoarsenerRGB(numberOfLayers);

	vizHierarchy = new GR::VisualizeSoftHierarchy(vizLayers, frames,
                    maxStCubeSize, lut);

    // Create the output directory tree
    //  one directory gets created for each hierarchy level.
    struct stat st;
    int status=0;
    for (int i=0; i<=vizLayers->getSize()-1; i++) 
    {
        snprintf(name,MAX_PATH_LENGTH-1,"%s/%02d",vizFileName.c_str(),(*vizLayers)[i]);
        if (stat(name, &st) != 0) {
            /* Directory does not exist */
            if (mkdir(name, S_IRWXU) != 0) {
                status = -1;
            }
        }
    }
    if (status == -1)
    {
        fprintf(stderr,"Unable to create the output directories\n");
        return 1;
    }

    cout << "Building (bunches of) soft hierarchies from the video (s.t.cube)" << endl;
	for (int i = 0; i <= frames->getSize() - 1; i++) {
		sprintf(currentFrameName, inputSequence.c_str(), (*frames)[i]);
		cout << "___ Processing " << currentFrameName << " ___" << endl;

		currentStCubeSize = ((*frames)[i] < maxStCubeSize) ?
				(*frames)[i] : maxStCubeSize;

		// construct the spatiotemporal cube that spans from frame i back
		// to frame i-currentStCubeSize+1, see Albert's note (1) for more.
		//for (int j = 0; j <= currentStCubeSize - 1; j++) {
		for (int jj=0, j = (*frames)[i]-currentStCubeSize+1; j <= (*frames)[i]; j++,jj++) {
			char frameName[MAX_PATH_LENGTH];
			sprintf(frameName, inputSequence.c_str(), j);
			I[jj] = IM::load_image_rgb(frameName);
		}

		GR::SwaGraphLayer* swaLattice =
				GR::buildSwaGraphLatticeFromStCubeRGB(I,currentStCubeSize);
		H = (GR::GraphHierarchy*) graphCoarsener->coarsenGraph(swaLattice);

		// arrayOfResults[frame# in the s.t. cube][level# of the hierarchy]
		arrayOfResults = vizHierarchy->computeLabelingAtAllLayers(H,
				currentStCubeSize, I[0]->W(), I[0]->H());

		sprintf(name, vizFileName.c_str(), (*frames)[i]);
		vizHierarchy->patchSlicesAndSave(arrayOfResults, (*frames)[i]-currentStCubeSize, currentStCubeSize, name);

		// delete the arrayOfResults structure
		for (int j = 0; j <= maxStCubeSize - 1; j++)
			delete[] arrayOfResults[j];
		delete[] arrayOfResults;

		for (int j = 0; j <= currentStCubeSize - 1; j++) {
			delete I[j];
			I[j] = NULL;
		}
		delete H;
		H = NULL;
	}


    // Done processing all frames! Free the memory!
    delete[] I;
    if (lut != NULL) {
        delete lut;
        lut = NULL;
    }
    if (frames != NULL) {
        delete frames;
        frames = NULL;
    }
    if (vizLayers != NULL) {
        delete vizLayers;
        vizLayers = NULL;
    }

    timeSystemTerminated = clock();
    elapsedTime = (timeSystemTerminated - timeSystemInitialized)
            / CLOCKS_PER_SEC;
    cout << "Overall running time: " << elapsedTime << " seconds" << endl;

    return 0;

}

