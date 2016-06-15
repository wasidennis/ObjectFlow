/**
 *  imagegraphs.cpp
 * jcorso
 *
 * RELEASED WITH THE SWA PART OF LIBSVX.
 *
 * THIS CODE IS RELEASED UNDER THE MIT-LIKE LICENSE INCLUDED
 * WITH THE SOURCE CODE.
 *
 * CONTACT jcorso@buffalo.edu FOR ANY QUESTIONS.
 *
 *
 * Implementation file for the various image graph definitions 
 */

#include "imagegraphs.h"

namespace vpml_graphs
{
using namespace vpml_images;

//// SwaEdgeAffinityGraphCoarsener implementation BEGINS ////

SwaEdgeAffinityGraphCoarsener ::  SwaEdgeAffinityGraphCoarsener(
   int levelMax_)
   : levelMax(levelMax_)
{
   stronglyConnectedCoeff = 0.2;
   fineAffinityCoeff = 20.0; // 10.0 is the original default
   coarseAffinityCoeffForIntensityAverage = 20.0; // 10.0 is the original default
   interpolantCutCoeff = 0.4;

   minWeightPercentThresh = 0.1;
   cutSalientSegmentFlag = false;
   cutSalientSegmentCoeff = 1e-6;
}



SwaGraphHierarchy* SwaEdgeAffinityGraphCoarsener::coarsenGraph(SwaGraphLayer* layer)
{
    SwaGraphHierarchy* H = new SwaGraphHierarchy();
    H->addLayer(layer);
    printf("there are %d nodes in layer 0\n", layer->getNumberOfNodes());

    SwaGraphLayer* layerCurrent = layer;
    for (int layerI=0; layerI<levelMax; layerI++)
    {
        layerCurrent = H->getLayer(layerI);
        layerCurrent->computeStats();

        // we should break when segments no longer have any neighbors
        if (layerCurrent->getMaxNumCouplings() == 0)
            break;

        // check for salient segments and cut them off from the graph (if the option is selected)
        if (this->cutSalientSegmentFlag && (layerI > 3))
          checkForSalientSegments(layerCurrent);

        // Select the block pixels (the representative subset of nodes that will be used as
        // the next coarser-layer nodes).
        // Returns a boolean array that is the same size as the number of nodes in layerCurrent,
        // where true: selected node, false: un-selected node.
        // 		Commented by A.Y.C.Chen
        bool* B = layerCurrent->selectBlockPixels(layerI, H->getVolSortingStart(),
                                                      this->stronglyConnectedCoeff);
        // Create new layer.
        SwaGraphLayer* layerNext = createAsSubsetInterlevelCopy(layerCurrent, B);

        // breaking because no more coarsening occurred
        if (layerCurrent->getNumberOfNodes() == layerNext->getNumberOfNodes())
            break;

        // compute interpolation weights between levels
        computeInterlevelWeights(layerCurrent, B);

        //each supernode aggregates weighted statistics from its children at the fine level
        updateCoarseStatistics(layerCurrent, layerNext);

        //assign neighbors in coarse level based on the relationships at the fine level
        assignCoarseNeighbors(layerCurrent, layerNext, layerI);

        H->addLayer(layerNext);
        delete [] B; B=NULL;
    }

    return H;
}



/** the goal of this function is to go through the existing nodes in the coarse graph
 * (created from representative nodes) and assigns neighbors based on the children
 * they have in common
 */
void SwaEdgeAffinityGraphCoarsener::assignCoarseNeighbors(SwaGraphLayer *fGL, SwaGraphLayer *cGL, int level)
{
    int klCouplingCount, jPLSize, ijCLSize, kChLSize;
    int i, j, k, l;
    float v, v2, PkiWij;

    // unused: int gFNoN = fGL->getNumberOfNodes();
    int gCNoN = cGL->getNumberOfNodes();

    float* klCouplingValues = new float[gCNoN];
    int* klCouplingRevLookups = new int[gCNoN];
    int* klCouplingLookups = new int[gCNoN];

    // initialization here not used until later
    for (i=0; i<gCNoN; i++)
    {
        klCouplingLookups[i] = -1;
        klCouplingRevLookups[i] = 0;
        klCouplingValues[i] = 0.0;
    }

    // Next, compute the affinities between the nodes in coarse layer
    // by going through all coarse nodes and accumulating their affinities
    // to other coarse nodes in a set of parallel arrays.
    for (k=0; k<gCNoN; k++)
    {
        klCouplingCount = 0;
        SwaNode* coarseNode = cGL->getNode(k);

        // iterate through k's children (i) on the finer level
        kChLSize = coarseNode->getNumberOfChildren();
        for (i=0; i<kChLSize; i++)
        {
            SwaNode* fineNode = coarseNode->getChild(i);
            float kiCValue = coarseNode->getChildWeight(i);

            // iterate through all neighbors j of node i
            ijCLSize = fineNode->getNumberOfNeighbors();
            for (j=0; j<ijCLSize; j++)
            {
                SwaNode* fine_nNode = fineNode->getNeighbor(j);
                float ijCValue = fineNode->getWeight(j);
                PkiWij = kiCValue*ijCValue;

                // iterate through all parents (l) of j on the coarser level
                jPLSize = fine_nNode->getNumberOfParents();
                for (l=0; l<jPLSize; l++)
                {
                    SwaNode* fine_par_nNode = fine_nNode->getParent(l);
                    int jlCLabel = fine_par_nNode->getLayerIndex();
                    float jlCValue = fine_nNode->getParentWeight(l);

                    if (klCouplingLookups[jlCLabel] == -1)
                    { // newly created neighbor
                        // the CouplingLoopkups store the index into the neighborhood values array
                        // for a node l in the coarse graph
                        klCouplingLookups[jlCLabel] = klCouplingCount;
                        // the CouplingValues is the weight of the  edge from k to l
                        klCouplingValues[klCouplingCount] = PkiWij*jlCValue;
                        // the reverse lookup gives the index l of the coupling value in
                        klCouplingRevLookups[klCouplingCount] = jlCLabel;
                        klCouplingCount++;
                    }
                    else
                    { // coupling already exists, just add to it
                        klCouplingValues[klCouplingLookups[jlCLabel]] += PkiWij*jlCValue;
                    }
                }
            }
        }// end iterating through k's i children (done with i)
        // add the neighbors coming out of this node k.addCouplingOneWay
        // note, we also compute k's saliency here
        v=0.0;
        for (l=0; l<klCouplingCount; l++)
        {
            if (klCouplingRevLookups[l] == k)
            {
                coarseNode->addToSelfAffinity(klCouplingValues[l]);
                // reset the lookups
                klCouplingLookups[k] = -1;
            }
            else
            {
                //include the average intensity to adjust the coupling
                int lN = klCouplingRevLookups[l];
                v2 = klCouplingValues[l] * computeCoarseAffinity(cGL, k, lN,
                                        this->coarseAffinityCoeffForIntensityAverage);
                SwaNode* n_node = cGL->getNode(lN);
                coarseNode->addNeighbor(n_node, v2); //neighbor relationship is 1-way bcos neighbor does same process
                v += v2;

                klCouplingLookups[klCouplingRevLookups[l]] = -1;
            }
        }
        coarseNode->setSaliency(pow((float)2.0, (float)level+1)*v/coarseNode->getSelfAffinity());
    }// end iterating through coarse k's */
    delete [] klCouplingLookups;
    delete [] klCouplingRevLookups;
    delete [] klCouplingValues;
}



void SwaEdgeAffinityGraphCoarsener::checkForSalientSegments(SwaGraphLayer* fGL)
{
    int iCLSize;
    int gFNoN = fGL->getNumberOfNodes();

    //	go through each node in the finer
    for (int i=0;i<gFNoN;i++)
    {
        SwaNode *node = fGL->getNode(i);
        if (node->getSaliency() < this->cutSalientSegmentCoeff)
        {
            // cut off node iN from the graph.
            iCLSize = node->getNumberOfNeighbors();
            for (int j=0;j<iCLSize;j++)
            {
                SwaNode *n_node = node->getNeighbor(j);
                n_node->removeRevNeighbor(node);
                node->removeNeighbor(j);
            }
            fGL->removeNode(i);
        }
    }
}





float SwaEdgeAffinityGraphCoarsener::computeCoarseAffinity(SwaGraphLayer* cGL, int k, int l, float coeff)
{
    float kI,lI;
    SwaNode* uNode = cGL->getNode(k);
    SwaNode* vNode = cGL->getNode(l);
    kI = uNode->getAverageIntensity();
    lI = vNode->getAverageIntensity();

    return exp(-coeff * fabs(kI - lI));
}


//Function computes interpolation weights between levels
void SwaEdgeAffinityGraphCoarsener::computeInterlevelWeights(SwaGraphLayer *fineGL, bool* B)
{
    float wILinC;
    float reducedWILinC;
    int iCLSize;
    int i,j;
    float v,v2,PkiValue;

    int gFNoN = fineGL->getNumberOfNodes();
    for (i=0; i<gFNoN; i++)
    {
        if (!B[i])
        {
            wILinC = 0.0;
            // first, compute the normalizing term
            SwaNode *node = fineGL->getNode(i);
            std::vector<SwaEdge*> buckets;

            iCLSize = node->getNumberOfNeighbors();

            for (j=0; j<iCLSize; j++)
            {
                if (B[node->getNeighbor(j)->getLayerIndex()])
                {
                    wILinC += node->getWeight(j);
                    buckets.push_back(node->getNeighborLink(j));
                }
            }

            // here we sort based on the weights of the node
            std::sort(buckets.begin(), buckets.end(), sortWeight);
            int bucket = buckets.size();

            //  THIS IS THE REAL SWA CODE
            v2 = wILinC * this->minWeightPercentThresh;
            reducedWILinC = wILinC * this->interpolantCutCoeff;

            v=0.0; // store weights sum

            for (j=0; j<bucket; j++)
            {
                float ijCvalue = buckets[j]->getWeight();

                // if this weight will push it over the percentage
                // threshold then stop
                if ( (v + ijCvalue) > reducedWILinC )
                    break;
                // if this weight is a significant percent of total (even
                // though we have not gone over the percentage), stop
                if ( ijCvalue > v2 )
                     break;
                v += ijCvalue;
            }

            reducedWILinC = wILinC - v;

            // next, go through and add inter-level weight connections
            // for those neighbors that remain
            for (; j<bucket; j++)
            {
                // ijC is the index in i's neighbor list (not the index in the graph)
                int ijC;
                for (ijC=0; ijC<node->getNumberOfNeighbors();)
                {
                    int jN = buckets[j]->getNode()->getLayerIndex();
                    if (jN == node->getNeighborLink(ijC)->getNode()->getLayerIndex())
                        break;
                    ijC++;
                }

                PkiValue = node->getWeight(ijC) / reducedWILinC;
                SwaNode* n_node = node->getNeighbor(ijC);

                // TODO: I don't really understand why this is here... A.Y.C.Chen
                assert(1 == n_node->getNumberOfParents());

                SwaNode* coarse_node = n_node->getParent(0);
                coarse_node->addChild(node, PkiValue);
                node->addParent(coarse_node, PkiValue);
            }
        }
    }
}



SwaGraphLayer* SwaEdgeAffinityGraphCoarsener::createAsSubsetInterlevelCopy(SwaGraphLayer* Gin, bool* B)
{
  const int sizeB = Gin->getNumberOfNodes();
  int newUID = Gin->getMaxUID()+1;

  SwaGraphLayer *cG = new SwaGraphLayer();
    int j=0;
    for(int i=0; i<sizeB; i++)
    {
        SwaNode *node = Gin->getNode(i);
        assert(node->getLayerIndex() < Gin->getNumberOfNodes());
        if(B[node->getLayerIndex()])
        {
            SwaInternalNode *new_node = new SwaInternalNode();
            new_node->setUID(newUID++);
            new_node->setLayerIndex(j);
            node->addParent(new_node, 1.0);
            new_node->addChild(node, 1.0);
            cG->addNode(new_node);
            j++;
        }
    }
   return cG;
}



void SwaEdgeAffinityGraphCoarsener::updateCoarseStatistics(SwaGraphLayer *fGL, SwaGraphLayer *cGL)
{
    int i, k, kChLSize;
    float sum_weights, volCalc, selfAffCalc, intensityAvg;
    float x,y;

    // go through each coarse node and account for the
    // accumulation of self-affinities from the fine level
    //unused: int gFNoN = fGL->getNumberOfNodes();
    int gCNoN = cGL->getNumberOfNodes();
    for (k=0; k<gCNoN; k++)
    {
        SwaNode* cNode = cGL->getNode(k);
        kChLSize = cNode->getNumberOfChildren();

        // self affinity is already 0.0 by default
        sum_weights=0.0;  // v is used to accumulate the edge-weights for normalization
        volCalc=0.0;
        selfAffCalc=0.0;
        intensityAvg=0.0;
        x=0.0;
        y=0.0;

        for (i=0; i<kChLSize; i++)
        {
            SwaNode* cChildNode = cNode->getChild(i);
           float kiCV = cNode->getChildWeight(i);

           sum_weights += kiCV;
           volCalc      += kiCV  *  cChildNode->getVolume();
           selfAffCalc  += kiCV  *  cChildNode->getSelfAffinity();
           intensityAvg += kiCV  *  cChildNode->getAverageIntensity();
           x            += kiCV  *  cChildNode->getX();
           y            += kiCV  *  cChildNode->getY();
        }

        assert(sum_weights > 0.0);
        cNode->addToVolume(volCalc);
        cNode->addToSelfAffinity(selfAffCalc);
        cNode->addToAverageIntensity(intensityAvg/sum_weights);
        cNode->addToX(x/sum_weights);
        cNode->addToY(y/sum_weights);
    }
}

//// SwaEdgeAffinityGraphCoarsener implementation ENDS ////


//// SwaEdgeAffinityGraphCoarsenerRGB implementation BEGINS ////



SwaEdgeAffinityGraphCoarsenerRGB ::  SwaEdgeAffinityGraphCoarsenerRGB(
   int levelMax_)
   : SwaEdgeAffinityGraphCoarsener(levelMax_)
{
   fineAffinityCoeff = 30.0; // 10.0 is the original default
   coarseAffinityCoeffForIntensityAverage = 30.0; // 10.0 is the original default
}




float SwaEdgeAffinityGraphCoarsenerRGB::computeCoarseAffinity(SwaGraphLayer* cGL, int k, int l, float coeff)
{
    float* kI,* lI;
    SwaNode* uNode = cGL->getNode(k);
    SwaNode* vNode = cGL->getNode(l);
    kI = uNode->getAverageColor();
    lI = vNode->getAverageColor();

    float  d = sqrtf( (kI[0]-lI[0])*(kI[0]-lI[0]) +
                      (kI[1]-lI[1])*(kI[1]-lI[1]) +
                      (kI[2]-lI[2])*(kI[2]-lI[2]) );

    return exp(-coeff * d);
}


void SwaEdgeAffinityGraphCoarsenerRGB::updateCoarseStatistics(SwaGraphLayer *fGL, SwaGraphLayer *cGL)
{
    int i, k, kChLSize;
    float sum_weights, volCalc, selfAffCalc, intensityAvg;
    float x,y;
    float colorAvg[3];

    // go through each coarse node and account for the
    // accumulation of self-affinities from the fine level
    //unused: int gFNoN = fGL->getNumberOfNodes();
    int gCNoN = cGL->getNumberOfNodes();
    for (k=0; k<gCNoN; k++)
    {
        SwaNode* cNode = cGL->getNode(k);
        kChLSize = cNode->getNumberOfChildren();

        // self affinity is already 0.0 by default
        sum_weights=0.0;  // v is used to accumulate the edge-weights for normalization
        volCalc=0.0;
        selfAffCalc=0.0;
        intensityAvg=0.0;
        x=0.0;
        y=0.0;
        colorAvg[0] = colorAvg[1] = colorAvg[2] = 0.0;

        for (i=0; i<kChLSize; i++)
        {
            SwaNode* cChildNode = cNode->getChild(i);
           float kiCV = cNode->getChildWeight(i);

           sum_weights += kiCV;
           volCalc      += kiCV  *  cChildNode->getVolume();
           selfAffCalc  += kiCV  *  cChildNode->getSelfAffinity();
           x            += kiCV  *  cChildNode->getX();
           y            += kiCV  *  cChildNode->getY();

           float* childColor = cChildNode->getAverageColor();
           colorAvg[0] += kiCV * childColor[0];
           colorAvg[1] += kiCV * childColor[1];
           colorAvg[2] += kiCV * childColor[2];

        }

        assert(sum_weights > 0.0);

        cNode->addToVolume(volCalc);
        cNode->addToSelfAffinity(selfAffCalc);

        colorAvg[0] /= sum_weights;
        colorAvg[1] /= sum_weights;
        colorAvg[2] /= sum_weights;
        cNode->addToAverageColor(colorAvg);

        cNode->addToX(x/sum_weights);
        cNode->addToY(y/sum_weights);
    }
}




//// SwaEdgeAffinityGraphCoarsenerRGB implementation ENDS ////



} // vpml_generalgraphs

