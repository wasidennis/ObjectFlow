/**
 * vizhierarchy.h
 *
 * RELEASED WITH THE SWA PART OF LIBSVX.
 * THIS IS A UTILITY FILE THAT IS USED IN THE SWA PROCESSING.  
 *
 * THIS CODE IS RELEASED UNDER THE MIT-LIKE LICENSE INCLUDED
 * WITH THE SOURCE CODE.
 *
 * CONTACT jcorso@buffalo.edu FOR ANY QUESTIONS.
 * 
 *
 * Visualize VPML hard/soft hierarchies by slicing it in different directions.
 *
 *
 * Albert Y. C. Chen, summer 2009 @ SUNY-Buffalo
 *
 */

#ifndef VIZHIERARCHY_H_
#define VIZHIERARCHY_H_

// these includes get most of the needed headers
#include "graphs.h"
#include "imagegraphs.h"
#include "config.h"

// C++ libraries
#include <sstream>
#include <iomanip>

namespace IM = vpml_images;

namespace vpml_graphs
{

//// non-templated global functions created in the s.t. cube coarsening project
IM::RGBImage2* patchRGBImages(IM::RGBImage2***, int numOfImagesW,
        int numOfImagesH, int spacing);
IM::ScalarImage2*  patchScalarImages(IM::ScalarImage2*** images,
        int numOfImagesW, int numOfImagesH, int spacing);

//// non-templated global functions for SWA hierarchies
IM::RGBImage2** computeSwaLabelingAtLayerByColormap(GraphHierarchy* H, int layer,
        int currentStCubeSize, int w, int h, byte* lut, IM::Colormap* colormap);


class VisualizeHierarchy
{
public:
    virtual ~VisualizeHierarchy() {};
    virtual IM::RGBImage2*** computeLabelingAtAllLayers(GraphHierarchy* H,
            int currentStCubeSize, int w, int h) = 0;
    virtual int patchSlicesAndSave(IM::RGBImage2*** arrayOfResults,
            int start, int currentStCubeSize, char* name) = 0;
};


// VisualizeSoftHierarchy general template class.
class VisualizeSoftHierarchy : public VisualizeHierarchy
{
protected:
    int maxStCubeSize;
    Index* vizLayers;
    Index* frames;
    unsigned char* lut;
    IM::Colormap* colormap;

public:
    VisualizeSoftHierarchy(Index* vizLayers_, Index* frames_,
            int maxStCubeSize_, unsigned char* lut_,
            IM::Colormap* colormap_= NULL)
    {
        vizLayers = vizLayers_; frames = frames_;
        maxStCubeSize = maxStCubeSize_; lut = lut_; colormap = colormap_;
    }
    ~VisualizeSoftHierarchy() {};
    IM::RGBImage2*** computeLabelingAtAllLayers(GraphHierarchy* H,
            int currentStCubeSize, int w, int h);
    int patchSlicesAndSave(IM::RGBImage2*** arrayOfResults,
            int start, int currentStCubeSize, char* name);
};

} //namespace vpml_generalgraphs

#endif /* VIZHIERARCHY_H_ */
