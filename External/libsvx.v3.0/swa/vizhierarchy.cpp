/*
 * vizhierarchy.cpp
 *
 * RELEASED WITH THE SWA PART OF LIBSVX.
 * THIS IS A UTILITY FILE THAT IS USED IN THE SWA PROCESSING.  
 *
 * THIS CODE IS RELEASED UNDER THE MIT-LIKE LICENSE INCLUDED
 * WITH THE SOURCE CODE.
 *
 * CONTACT jcorso@buffalo.edu FOR ANY QUESTIONS.
 
 */

#include "vizhierarchy.h"


namespace vpml_graphs
{
using namespace std;

//// non-templated global functions implementation BEGINS ////

/**
 * computeSwaLabelingAtLayerByColormap
 *
 * Two variants are possible.
 * 1.  You have a colormap IM:Colormap and the lut will back-reference 
 * from label index into the Colormap.
 * 2.  Colormap is null.  And lut is an N * 3 byte map which each 
 * three are an rgb value.
 **/
IM::RGBImage2** computeSwaLabelingAtLayerByColormap(SwaGraphHierarchy* H,
        int layer, int currentStCubeSize, int w, int h, byte* lut,
        IM::Colormap* colormap)
{
    IM::RGBImage2** L = new IM::RGBImage2*[currentStCubeSize];

    // get the coarser layer g's corresponding lattice layer labeling result
    int* labels = H->getSegmentsFromScale(layer);

    int index=0;
    for (int z=0; z<=currentStCubeSize-1;z++)
    {
        L[z] = new IM::RGBImage2(w,h);
        for (int y=0;y<h;y++)
        {
            for (int x=0;x<w;x++,index++)
            {
                if (colormap)
                {
                  byte value = lut[labels[index]];
                  (*L[z])(x,y  ) = (*colormap)(value-1, 0);
                  (*L[z])(x,y,1) = (*colormap)(value-1, 1);
                  (*L[z])(x,y,2) = (*colormap)(value-1, 2);
                }
                else
                {
                  (*L[z])(x,y  ) = lut[labels[index]*3  ];
                  (*L[z])(x,y,1) = lut[labels[index]*3+1];
                  (*L[z])(x,y,2) = lut[labels[index]*3+2];
                }
            }
        }
    }

    free(labels); labels=NULL;
    return L;
}


/** Function for patching several RGB images into a single one. Patches are
 * separated by black lines of width "spacing".
 */
IM::RGBImage2*  patchRGBImages(IM::RGBImage2*** images,
        int numOfImagesW, int numOfImagesH, int spacing)
{
    int singleImageW = images[0][0]->W();
    int singleImageH = images[0][0]->H();
    int patchedImageW = singleImageW*numOfImagesW + spacing*(numOfImagesW-1);
    int patchedImageH = singleImageH*numOfImagesH + spacing*(numOfImagesH-1);
    IM::RGBImage2* patchedImage = new IM::RGBImage2(patchedImageW,patchedImageH);
    patchedImage->fillWithValue((byte) 0);


    for (int j=0; j<numOfImagesH; j++)
    {
        for (int i=0; i<numOfImagesW; i++)
        {
            for (int y=0; y<singleImageH; y++)
            {
                for (int x=0; x<singleImageW; x++)
                {
                    patchedImageW = x+(singleImageW+spacing)*i;
                    patchedImageH = y+(singleImageH+spacing)*j;

                    (*patchedImage)(patchedImageW, patchedImageH   ) = (*images[j][i])(x,y   );
                    (*patchedImage)(patchedImageW, patchedImageH, 1) = (*images[j][i])(x,y, 1);
                    (*patchedImage)(patchedImageW, patchedImageH, 2) = (*images[j][i])(x,y, 2);
                }
            }
        }
    }

    return patchedImage;
}


/** Function for patching several ScalarImage2's into a single one. Patches are
 * separated by black lines of width "spacing".
 */
IM::ScalarImage2*  patchScalarImages(IM::ScalarImage2*** images, int numOfImagesW,
        int numOfImagesH, int spacing)
{
    int singleImageW = images[0][0]->W();
    int singleImageH = images[0][0]->H();
    int patchedImageW = singleImageW*numOfImagesW + spacing*(numOfImagesW-1);
    int patchedImageH = singleImageH*numOfImagesH + spacing*(numOfImagesH-1);
    IM::ScalarImage2* patchedImage = new IM::ScalarImage2(patchedImageW,patchedImageH);
    patchedImage->fillWithValue((byte) 0);


    for (int j=0; j<numOfImagesH; j++)
    {
        for (int i=0; i<numOfImagesW; i++)
        {
            for (int y=0; y<singleImageH; y++)
            {
                for (int x=0; x<singleImageW; x++)
                {
                    patchedImageW = x+(singleImageW+spacing)*i;
                    patchedImageH = y+(singleImageH+spacing)*j;
                    patchedImage->setValue(patchedImageW,patchedImageH, images[j][i]->getValue(x,y));
                }
            }
        }
    }

    return patchedImage;
}
//// non-templated global function implementation ENDS ////


IM::RGBImage2***
VisualizeSoftHierarchy::computeLabelingAtAllLayers(GraphHierarchy* H,
        int currentStCubeSize, int w, int h)
{
    // arrayOfResults[frame# in the s.t. cube][level# of the hierarchy]
    IM::RGBImage2*** arrayOfResults = new IM::RGBImage2**[maxStCubeSize];
    for (int i = 0; i < maxStCubeSize; i++)
        arrayOfResults[i] = new IM::RGBImage2*[vizLayers->getSize()];

    SwaGraphHierarchy* swaH = dynamic_cast<SwaGraphHierarchy*>(H);

    for (int j=0; j<=vizLayers->getSize()-1; j++)
    {
        IM::RGBImage2** L = computeSwaLabelingAtLayerByColormap(swaH,
                (*vizLayers)[j], currentStCubeSize, w, h, lut, colormap);

        for (int k =0; k<=currentStCubeSize-1; k++)
            arrayOfResults[k][j] = L[k];
    }

    return arrayOfResults;
}


int
VisualizeSoftHierarchy::patchSlicesAndSave(IM::RGBImage2*** arrayOfResults,
        int start, int currentStCubeSize, char* name)
{
    //IM::RGBImage2* allResultsPatched=NULL;
    // patching the slices (if the user selected this function)
    //allResultsPatched = patchRGBImages(arrayOfResults,
    //        vizLayers->getSize(), currentStCubeSize, 2);

    // saving the slices as images
    for (int k=0; k<=currentStCubeSize-1; k++) {
        for (int j=0; j<=vizLayers->getSize()-1; j++) {
            stringstream ss;
            ss << name << "/" << setfill('0') << setw(2) << (*vizLayers)[j] <<
              "/" << setfill('0') << setw(5) << start+k+1 << ".png";
            string fileName = ss.str();
            arrayOfResults[k][j]->saveToFile(fileName.c_str());
        }
    }

    // saving the patched image
    //stringstream ss;
    //ss << name << "allPatched.png";
    //string fileName = ss.str();
    //allResultsPatched->saveToFile(fileName.c_str());

    return 0;
};


} // namespace vpml_generalgraphs
