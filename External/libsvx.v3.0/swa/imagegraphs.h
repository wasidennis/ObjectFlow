/**
 * imagegraphs.h
 * created by jcorso; augmented by  aychen
 *
 *
 * RELEASED WITH THE SWA PART OF LIBSVX.
 *
 * THIS CODE IS RELEASED UNDER THE MIT-LIKE LICENSE INCLUDED
 * WITH THE SOURCE CODE.
 *
 * CONTACT jcorso@buffalo.edu FOR ANY QUESTIONS.
 *
 *
 *
 * Defines the data structures etc. for defining graphs on images.
 *
 */

#ifndef IMAGEGRAPHS_H
#define IMAGEGRAPHS_H

// graphs.h makes all of the includes we needs except for a few
#include "graphs.h"

#include <cstring>
#include <deque>
#include <ext/hash_set>
#include <ext/hash_map>

namespace vpml_graphs
{


class GraphLayer;
class GraphHierarchy;


/** this SwaEdgeAffinityGraphCoarsener (SWA-EAGC) caorsens a hierarchy
 * by the SWA algorithm, which is different from the GS (GS-EAGC) one with
 * the following major differences:
 *	(1) GS-EAGC takes 4 parameters to initialize (AffinityComputer, alpha,
 *	    ReductionFactor, and levelMax), while SWA-EAGC takes only 1 (levelMax).
 *	(2) However, SWA-EAGC contains more "hard-coded" parameters, although
 *		they were set to values the authors suggested (all protected variables
 *		in the class except levelMax).
 *	(3) GS-EAGC's public function coarsenGraph calls the one and only private
 *		function "coarsen" to build the hierarchy, while SWA-EAGC uses all 6
 *		self-defined private functions.
 */

class SwaEdgeAffinityGraphCoarsener// : public GraphCoarsener
{
protected:
    int    levelMax;

    float stronglyConnectedCoeff;
    float fineAffinityCoeff;
    float coarseAffinityCoeffForIntensityAverage;
    float interpolantCutCoeff;

    float minWeightPercentThresh;
    bool cutSalientSegmentFlag;
    float cutSalientSegmentCoeff;

protected:
    unsigned char* buffer;  // used during coarsening

public:
    SwaEdgeAffinityGraphCoarsener(int lMax);
    virtual ~SwaEdgeAffinityGraphCoarsener() {};
    SwaGraphHierarchy* coarsenGraph(SwaGraphLayer* layer);

protected:
    virtual void     assignCoarseNeighbors(SwaGraphLayer *fGL, SwaGraphLayer *cGL, int level);
    virtual void     checkForSalientSegments(SwaGraphLayer* fGL);
    virtual float    computeCoarseAffinity(SwaGraphLayer* cGL, int k, int l, float coeff);
    virtual void     computeInterlevelWeights(SwaGraphLayer *fineGL, bool *B);
    virtual SwaGraphLayer* createAsSubsetInterlevelCopy(SwaGraphLayer* Gin, bool* B);
    virtual void     updateCoarseStatistics(SwaGraphLayer *fGL, SwaGraphLayer *cGL);
};


class SwaEdgeAffinityGraphCoarsenerRGB: public SwaEdgeAffinityGraphCoarsener
{
public:

    SwaEdgeAffinityGraphCoarsenerRGB(int lMax)  ;
    virtual ~SwaEdgeAffinityGraphCoarsenerRGB() {};

protected:

    virtual float computeCoarseAffinity(SwaGraphLayer* cGL, int k, int l, float coeff);
    virtual void  updateCoarseStatistics(SwaGraphLayer *fGL, SwaGraphLayer *cGL);
};

/** Build a lattice graph for SWA on 2D scalar image. The template is transferred
 * to the image class
 */
template<typename T>
SwaGraphLayer*  buildSwaGraphLattice (const vpml_images::_scalarImage2<T>* I,
                                const vpml_images::RGBImage2* RGB=0x0)
{
    int width=I->W(), height=I->H();
    const int n = width * height;

    SwaLeafNode** leaves = new SwaLeafNode*[n];

    SwaGraphLayer* G = new SwaGraphLayer();

    G->reserveStorage(n);

    int index=0;
    for (int y=0; y<height; y++)
    {
        for (int x=0; x<width; x++,index++)
        {
            SwaLeafNode* lp = new SwaLeafNode();
            leaves[index] = lp;

            // set the attributes
            lp->setUID(index);
            lp->setLayerIndex(index);
            lp->setX(x);
            lp->setY(y);
            int tmp = (int)I->getValue(x,y);
            lp->setIntensity((float)tmp/255.0);
            lp->addToSelfAffinity(1.0);
            lp->addToVolume(1.0);
            G->addNode(lp);
        }
   }

    // now go through the nodes in the graph and link up
    //  edges only to 4-connected neighbors
    index=0;
    float coeff = 10.0;
    for (int y=0; y<height; y++)
    {
        for (int x=0; x<width; x++, index++)
        {
            SwaLeafNode* lp = (SwaLeafNode*)leaves[index];
            SwaLeafNode* neighbor;

            if (x < width-1)
            {  // make 2-way edge between x and x+1
                neighbor = leaves[index+1];
                float affinity = exp(-coeff * fabs(lp->getNormalizedIntensity()
                                              - neighbor->getNormalizedIntensity()));
                lp->addNeighbor(neighbor,affinity);
                neighbor->addNeighbor(lp,affinity);
            }

            if (y < height-1)
            {  //same at y and y+1
                neighbor = leaves[index + width];
                float affinity = (float)exp(-coeff * (float)fabs(lp->getNormalizedIntensity()
                                                              - neighbor->getNormalizedIntensity()));
                lp->addNeighbor(neighbor,affinity);
                neighbor->addNeighbor(lp,affinity);
            }
      }
    }

    delete [] leaves;

    return G;
}


/** Build a SWA graph lattice from a spatiotemporal cube that spans N frames,
 * which is later used to construct the adaptive hierarchy.
 */
template<typename T>
SwaGraphLayer*  buildSwaGraphLatticeFromStCube (vpml_images::_scalarImage2<T>** frame,
                                int stCubeSize)
{
    const int n = frame[0]->W()*frame[0]->H();

    // make some leafnodes in a contiguous block of memory
    //LeafNode* leaves = new LeafNode[n];
    //LeafNode* lp = leaves;
    // NOTE--> the contiguous block approach is what I had originally programmed
    //          but, there is a difficulty in freeing memory at a later time
    //          that was allocated in this fashion.  So, I simply went to this way.
    SwaLeafNode** leaves = new SwaLeafNode*[n*stCubeSize];

    SwaGraphLayer* G = new SwaGraphLayer();

    G->reserveStorage(n*stCubeSize);

    int index=0;

    for (int z=0; z<stCubeSize; z++)
    {
        for (int y=0;y<frame[0]->H();y++)
        {
            for (int x=0;x<frame[0]->W();x++,index++)
            {
                SwaLeafNode* lp = new SwaLeafNode();
                leaves[index] = lp;

                // set the attributes
                lp->setUID(index);
                lp->setLayerIndex(index);
                lp->setX(x);
                lp->setY(y);
                lp->setZ(z);
                int tmp = (int)frame[z]->getValue(x,y);
                lp->setIntensity((float)tmp/255.0);
                lp->addToSelfAffinity(1.0);
                lp->addToVolume(1.0);
                G->addNode(lp);

            }
        }
    }

    // now go through the nodes in the graph and link up
    //  edges only to 4-connected neighbors
    index=0;
    float coeff = 10.0;
    for (int z=0; z<stCubeSize; z++)
    {
        for (int y=0; y<frame[0]->H(); y++)
        {
            for (int x=0; x<frame[0]->W(); x++, index++)
            {
                SwaLeafNode* lp = (SwaLeafNode*)leaves[index];
                SwaLeafNode* neighbor;

                if (x < frame[0]->W()-1)
                {  // make 2-way edge between x and x+1
                    neighbor = leaves[index+1];
                    //float affinity = AC.compute(lp->getIntensity(),neighbor->getIntensity());
                    float affinity = exp(-coeff * fabs(lp->getNormalizedIntensity()
                                                  - neighbor->getNormalizedIntensity()));
                    lp->addNeighbor(neighbor,affinity);
                    neighbor->addNeighbor(lp,affinity);
                }

                if (y < frame[0]->H()-1)
                {  //same at y and y+1
                    neighbor = leaves[index + frame[0]->W()];
                    float affinity = (float)exp(-coeff * (float)fabs(lp->getNormalizedIntensity()
                                                                  - neighbor->getNormalizedIntensity()));
                    lp->addNeighbor(neighbor,affinity);
                    neighbor->addNeighbor(lp,affinity);
                }

                if (z < stCubeSize-1)
                {
                    neighbor = leaves[index + n];
                    float affinity = (float)exp(-coeff * (float)fabs(lp->getNormalizedIntensity()
                                                                  - neighbor->getNormalizedIntensity()));
                    lp->addNeighbor(neighbor,affinity);
                    neighbor->addNeighbor(lp,affinity);
                }
            }
        }
    }

    delete [] leaves;

    return G;

}



// CVPR2012
/** Build a SWA graph lattice from a spatiotemporal cube that spans N frames,
 * which is later used to construct the adaptive hierarchy.
 */
template<typename T>
SwaGraphLayer*  buildSwaGraphLatticeFromStCubeRGB (vpml_images::_rgbImage2<T>** frame,
                                int stCubeSize)
{
    const int n = frame[0]->W()*frame[0]->H();

    // make some leafnodes in a contiguous block of memory
    //LeafNode* leaves = new LeafNode[n];
    //LeafNode* lp = leaves;
    // NOTE--> the contiguous block approach is what I had originally programmed
    //          but, there is a difficulty in freeing memory at a later time
    //          that was allocated in this fashion.  So, I simply went to this way.
    SwaLeafNode** leaves = new SwaLeafNode*[n*stCubeSize];

    SwaGraphLayer* G = new SwaGraphLayer();

    G->reserveStorage(n*stCubeSize);

    int index=0;
    float color[3];

    for (int z=0; z<stCubeSize; z++)
    {
        for (int y=0;y<frame[0]->H();y++)
        {
            for (int x=0;x<frame[0]->W();x++,index++)
            {
                SwaLeafNode* lp = new SwaLeafNode();
                leaves[index] = lp;

                // set the attributes
                lp->setUID(index);
                lp->setLayerIndex(index);
                lp->setX(x);
                lp->setY(y);
                lp->setZ(z);
                color[0] = ((float)(frame[z]->getValue(x,y,0)))/255.;
                color[1] = ((float)(frame[z]->getValue(x,y,1)))/255.;
                color[2] = ((float)(frame[z]->getValue(x,y,2)))/255.;
                lp->setColor(color);
                lp->setIntensity((color[0]+color[1]+color[2])/3.);
                lp->addToSelfAffinity(1.0);
                lp->addToVolume(1.0);
                G->addNode(lp);

            }
        }
    }

    // now go through the nodes in the graph and link up
    //  edges only to 4-connected neighbors
    index=0;
    float coeff = 10.0;
    for (int z=0; z<stCubeSize; z++)
    {
        for (int y=0; y<frame[0]->H(); y++)
        {
            for (int x=0; x<frame[0]->W(); x++, index++)
            {
                SwaLeafNode* lp = (SwaLeafNode*)leaves[index];
                SwaLeafNode* neighbor;

                if (x < frame[0]->W()-1)
                {  // make 2-way edge between x and x+1
                    neighbor = leaves[index+1];
                    const float* c1 = lp->getNormalizedColor();
                    const float* c2 = neighbor->getNormalizedColor();
                    float d = sqrtf( (c1[0]-c2[0])*(c1[0]-c2[0]) +
                               (c1[1]-c2[1])*(c1[1]-c2[1]) +
                               (c1[2]-c2[2])*(c1[2]-c2[2]) );
                    float affinity = exp(-coeff * d);
                    lp->addNeighbor(neighbor,affinity);
                    neighbor->addNeighbor(lp,affinity);
                }

                if (y < frame[0]->H()-1)
                {  //same at y and y+1
                    neighbor = leaves[index + frame[0]->W()];
                    const float* c1 = lp->getNormalizedColor();
                    const float* c2 = neighbor->getNormalizedColor();
                    float d = sqrtf( (c1[0]-c2[0])*(c1[0]-c2[0]) +
                               (c1[1]-c2[1])*(c1[1]-c2[1]) +
                               (c1[2]-c2[2])*(c1[2]-c2[2]) );
                    float affinity = (float)exp(-coeff * d);
                    lp->addNeighbor(neighbor,affinity);
                    neighbor->addNeighbor(lp,affinity);
                }

                if (z < stCubeSize-1)
                {
                    neighbor = leaves[index + n];
                    const float* c1 = lp->getNormalizedColor();
                    const float* c2 = neighbor->getNormalizedColor();
                    float d = sqrtf( (c1[0]-c2[0])*(c1[0]-c2[0]) +
                               (c1[1]-c2[1])*(c1[1]-c2[1]) +
                               (c1[2]-c2[2])*(c1[2]-c2[2]) );
                    float affinity = (float)exp(-coeff * d);
                    lp->addNeighbor(neighbor,affinity);
                    neighbor->addNeighbor(lp,affinity);
                }
            }
        }
    }

    delete [] leaves;

    return G;

}


} // vpml_generalgraphs



#endif /*IMAGEGRAPHS_H*/
