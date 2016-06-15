/**
 *  graphs.cpp
 * jcorso
 *
 * Implementation file for the various graph definitions for Graph-Shifts
 *
 * RELEASED WITH THE SWA PART OF LIBSVX.
 *
 * THIS CODE IS RELEASED UNDER THE MIT-LIKE LICENSE INCLUDED
 * WITH THE SOURCE CODE.
 *
 * CONTACT jcorso@buffalo.edu FOR ANY QUESTIONS.
 */

#include <limits.h>

#include "graphs.h"
#include "imagegraphs.h"

namespace vpml_graphs
{

using namespace std;

////////////////////////////////////////////////////////////////
//// Classes for building hard hierarchies (as in GS & BMA) ////
////               --- definition BEGINS ---                ////
////////////////////////////////////////////////////////////////

bool
ltNodeComparator :: operator()(const Node* s1, const Node* s2) const
{
    return s1->getUID() < s2->getUID();
}




//////////////   GraphLayer


GraphLayer :: ~GraphLayer ()
{
    for (std::vector<Node*>::iterator i = nodes.begin();
         i<nodes.end(); i++)
    {
        delete *i;
    }
    nodes.clear();
}


void
GraphLayer ::  addEdge(Node* a, Node* b, real weight, int length)
{
    Edge* edge;

    edge = new Edge(a, weight, length);
    b->addNeighbor(edge);

    edge = new Edge(b, weight, length);
    a->addNeighbor(edge);
}

void
GraphLayer ::  appendToFile(FILE* fp)
//  TODO
{
    fprintf(stderr,"GraphLayer::appendToFile (%s::%d) called but not yet implemented\n",__FILE__,__LINE__);
    fprintf(fp,"%d\n",getNumberOfNodes());
}


void
GraphLayer ::  reserveStorage(int n)
{
    nodes.reserve(n);
}




//////////////   End of GraphLayer





//////////////   GraphHierarchy

GraphHierarchy :: ~GraphHierarchy()
{
    for (std::vector<GraphLayer*>::iterator i = layers.begin();
         i<layers.end(); i++)
    {
        delete *i;
    }
    layers.clear();
}

void
GraphHierarchy ::  addLayer(GraphLayer* newLayer)
{
    layers.push_back(newLayer);
}


GraphLayer*
GraphHierarchy ::  getLayer(int index)
{
    assert(index < (int)layers.size());
    return layers[index];
}


int
GraphHierarchy ::  getNumberOfLayers() const
{
    return layers.size();
}

//////////////   End of GraphHierarchy


////////////////////////////////////////////////////////////////
//// Classes for building hard hierarchies (as in GS & BMA) ////
////                --- definition ENDS ---                 ////
////////////////////////////////////////////////////////////////






////////////////////////////////////////////////////////////////
//// Classes for building soft hierarchies (as in SWA)      ////
////               --- definition BEGINS ---                ////
////////////////////////////////////////////////////////////////

bool
ltSwaNodeComparator :: operator()(const SwaNode* s1, const SwaNode* s2) const
{
    return s1->getUID() < s2->getUID();
}


//////////////    SwaCommonNode
SwaCommonNode::SwaCommonNode()
{
   x = -1.0;
   y = -1.0;
   intensity = 0.0;
   labelIndex = 0;
   layerIndex =-1;
   saliency = 0.0;
   selfAffinity = 0.0;
   volume = 0.0;
   color[0] = color[1] = color[2] = 0.0;

}


SwaCommonNode::~SwaCommonNode()
{
    for (std::vector<SwaEdge*>::iterator i = neighbors.begin();
         i<neighbors.end(); i++)
    {
        delete *i;
    }
    neighbors.clear();
}


int
SwaCommonNode::addNeighbor(SwaNode* newNeighbor, float w)
{
    neighbors.push_back(new SwaEdge(newNeighbor,w));
    return 0;
}

int
SwaCommonNode::addNeighbor(SwaEdge* newNeighbor)
{
    neighbors.push_back(newNeighbor);
    return 0;
}

SwaNode* const SwaCommonNode::getConstNeighbor(int index) const
{
    assert(index < (int)neighbors.size());
    return neighbors[index]->getNode();
}


SwaNode* SwaCommonNode::getNeighbor(int index) const
{
    assert(index < (int)neighbors.size());
    return neighbors[index]->getNode();
}

SwaEdge* SwaCommonNode::getNeighborLink(int index) const
{
    assert(index < (int)neighbors.size());
    return neighbors[index];
}

const Weight& SwaCommonNode ::  getWeight(int index) const
{
    assert(index < (int)neighbors.size());
    return neighbors[index]->getWeight();
}

void SwaCommonNode::clearNeighbors()
{
   neighbors.clear();
}

void SwaCommonNode::removeNeighbor(int index)
{
    assert(index < (int)neighbors.size());
    neighbors.erase(neighbors.begin()+index);
}

void SwaCommonNode::removeRevNeighbor(SwaNode* inode)
{
    int lsize = neighbors.size();
    for (int i=0; i<lsize; i++)
   {
       SwaEdge *edge = neighbors.at(i);
       SwaNode *node = edge->getNode();
       if (node == inode)
       {
          neighbors.erase(neighbors.begin()+i);
          break;
      }
   }
}

//////////////   End of SwaCommonNode


//////////////    LeafNode
std::set<SwaNode*, ltSwaNodeComparator> SwaLeafNode::leafHolder;

SwaLeafNode ::  SwaLeafNode()
{
}


SwaLeafNode :: ~SwaLeafNode()
{
}


int SwaLeafNode::addParent(SwaNode* newParent, float w)
{
    parents.push_back(new SwaEdge(newParent,w));
    return 0;
}

std::set<SwaNode*, ltSwaNodeComparator> &
SwaLeafNode :: getSwaLeaves()
{
    leafHolder.clear();
    leafHolder.insert(this);
    return leafHolder;
}


SwaNode* const SwaLeafNode::getParent(int parentIndex) const
{
    assert(parentIndex < (int)parents.size());
    return parents[parentIndex]->getNode();
}


float SwaLeafNode::getParentWeight(int parentIdx) const
{
    assert(parentIdx < (int)parents.size());
    return parents[parentIdx]->getWeight();
}


void
SwaLeafNode::setParent(SwaNode* newParent, int parentIndex)
{
    //IMBUGBUG - we shuld eventually remove this function. leave for now.
}
//////////////   End of SwaLeafNode


//////////////   InternalNode
SwaInternalNode::SwaInternalNode() : SwaLeafNode()
{

}

SwaInternalNode :: ~SwaInternalNode()
{
    children.clear();
    leaves.clear();
}


int SwaInternalNode::addChild(SwaNode* newChild, float w)
{
    children.push_back(new SwaEdge(newChild,w));
    return 0;
}


SwaNode* SwaInternalNode ::  getChild(int childIndex) const
{
    assert(childIndex < (int)children.size());
    return children[childIndex]->getNode();
}


float SwaInternalNode::getChildWeight(int index) const
{
    assert(index < (int)children.size());
    return children[index]->getWeight();
}


void SwaInternalNode::mergeIntoLeafSet(SwaNode* n)
{
    leaves.insert(n);
}


void
SwaInternalNode ::  mergeIntoLeafSet(std::set<SwaNode*,ltSwaNodeComparator> &S)
{
    leaves.insert(S.begin(),S.end());
}


void
SwaInternalNode ::  cacheInvalidateAll()
{
}
//////////////   End of InternalNode


//////////////   Start SwaGraphLayer
SwaGraphLayer::SwaGraphLayer ()
{
    id = -1;
    maxNumCouplings = 0;
    maxUID = -1;
}


SwaGraphLayer::~SwaGraphLayer ()
{
    for (std::vector<SwaNode*>::iterator i = nodes.begin();
         i<nodes.end(); i++)
    {
        delete *i;
    }
    nodes.clear();
}

void
SwaGraphLayer ::  appendToFile(FILE* fp)
{
    fprintf(fp,"%d\n",getNumberOfNodes());
}


void SwaGraphLayer::checkForSalientSegments(float cutCoeff)
{
    int num_nodes = getNumberOfNodes();
    for (int i=0; i<num_nodes; i++)
    {
        SwaNode* node = getNode(i);
        if (node->getSaliency() < cutCoeff)
            node->clearNeighbors();
    }
}


void SwaGraphLayer::computeStats()
{
   maxNumCouplings = -INT_MAX;
   minNumCouplings = INT_MAX;
   avgNumCouplings = 0.0;
   maxNumChildren = -INT_MAX;
   minNumChildren = INT_MAX;
   avgNumChildren = 0.0;
   maxUID = -INT_MAX+1;
   int num_nodes = getNumberOfNodes();

   for(int i=0; i<num_nodes; i++)
   {
       SwaNode* node = getNode(i);
       int num_n = node->getNumberOfNeighbors();
       int num_c = node->getNumberOfChildren();

       if (num_n > maxNumCouplings)
           maxNumCouplings = num_n;
       if (num_n < minNumCouplings)
           minNumCouplings = num_n;
       avgNumCouplings += num_n;

       if (num_c > maxNumChildren)
           maxNumChildren = num_c;
       if (num_c < minNumChildren)
           minNumChildren = num_c;
       avgNumChildren += num_c;

       if (node->getUID() > maxUID)
          maxUID = node->getUID();
   }
   avgNumCouplings = avgNumCouplings/num_nodes;
   avgNumChildren = avgNumChildren/num_nodes;

    return;
}


void SwaGraphLayer::reserveStorage(int n)
{
    nodes.reserve(n);
}


void SwaGraphLayer::removeNode(int index)
{
    assert(index < (int)nodes.size());
    nodes.erase(nodes.begin()+index);
}


bool* SwaGraphLayer::selectBlockPixels(int level, int volSortingStart,
                                    float stronglyConnectedCoeff)
{
    float wIJinC;
    float wIJ;
    int num_nodes, iCLSize;
    float volMin,volMax,vol,volAlpha;
    std::list<int> bucketCur;

    num_nodes = getNumberOfNodes();
    bool *B = new bool[num_nodes];
    for (int i=0; i<num_nodes; i++)
       B[i] = false;     //initialize B[i] value to false

    //  go through and find the "block pixels"
    if (level < volSortingStart)
    {
        // always add the first node to the representative set
        B[0] = true;
        for (int i=1; i<num_nodes; i++)
        {
            wIJinC = 0.0;
            wIJ = 0.0;

            SwaNode* node = getNode(i);
            iCLSize = node->getNumberOfNeighbors();

            for (int j=0; j<iCLSize; j++)
            {
                SwaNode* n_node = node->getNeighbor(j);
                int ijCL   = n_node->getLayerIndex();
                float ijCV = node->getWeight(j);

                if (B[ijCL])
                    wIJinC += ijCV;
                wIJ += ijCV;
            }

            if (wIJinC < stronglyConnectedCoeff*wIJ)
            {
                B[i] = true;
                continue;
            }

            // what if this node isn't strongly connected to anything?
            if (wIJ < 0.000000001)
            {
                B[i] = true;
            }

        }
    }
    // For higher-layer nodes, (1) sort them (roughly) by volume in decreasing order, then
    // (2) find the representative set of "block pixels" to serve as the next layer.
    else
    {
        const int volBucketNum = 30;
        std::vector<int*> buckets;
        int *b_sizes = (int*)calloc(sizeof(int), volBucketNum);
        int* bucket;
        int bckt_idx, b_size;

        // (1) sort them (roughly) by volume in decreasing order

        // initialize the volume buckets and their corresponding sizes to zero
        for (int i=0; i<volBucketNum; i++)
        {
            bucket = (int*)calloc(sizeof(int), num_nodes);
            buckets.push_back(bucket);
        }

        // compute the min and max of the node volumes
        volMin = FLT_MAX;
        volMax = -FLT_MAX;
        for (int i=0; i< num_nodes; i++)
        {
            SwaNode* node = getNode(i);
            vol = node->getVolume();
            volMin = (vol < volMin) ? vol : volMin;
            volMax = (vol > volMax) ? vol : volMax;
        }

        volAlpha = (volBucketNum-1) / (volMax-volMin);

        //populate the arrays buckets, b_sizes and bucket
        for (int i=0; i<num_nodes; i++)
        {
            SwaNode* node = getNode(i);
            vol = node->getVolume();
            bckt_idx = (int)round((vol-volMin)*volAlpha);
            bucket = buckets[bckt_idx];
            b_size = b_sizes[bckt_idx]++;
            bucket[b_size] = i;
        }

        // (2) find the representative set of "block pixels" to serve as the next layer.
        for (int i=volBucketNum-1; i>=0; i--)
        {
            bucket =  buckets[i];
            int bucket_len = b_sizes[i];
            for (int k=0; k<bucket_len; k++)
            {
                int iN = (int)bucket[k];
                wIJinC = 0.0;
                wIJ = 0.0;

                SwaNode* node = getNode(iN);
                iCLSize = node->getNumberOfNeighbors();
                for (int j=0; j<iCLSize; j++)
                {
                    int ijCL   = node->getNeighbor(j)->getLayerIndex();
                    float ijCV = node->getWeight(j);

                    if (B[ijCL])
                        wIJinC += ijCV;
                    wIJ += ijCV;
                }

                // make this <= instead of < in the case a segment is cut off from graph
                if (wIJinC <= stronglyConnectedCoeff*wIJ)
                {
                    B[iN] = true;
                }
            }
        }
    }

//FILE* fp = fopen ("output.log","a");
//fprintf(fp, "Currently in level %d\n", level);
//fclose(fp);
    return B;
}
//////////////   End of SwaGraphLayer


//////////////   SwaGraphHierarchy
SwaGraphHierarchy::SwaGraphHierarchy(bool b)
{
   should_cut_salient_segs = b;
   volSortingStart = 3; //20;
}

SwaGraphHierarchy :: ~SwaGraphHierarchy()
{
    for (std::vector<SwaGraphLayer*>::iterator i = layers.begin();
         i<layers.end(); i++)
    {
        delete *i;
    }
    layers.clear();
}

void
SwaGraphHierarchy::addLayer(SwaGraphLayer* newLayer)
{
    layers.push_back(newLayer);
}


SwaGraphLayer* SwaGraphHierarchy::getLayer(int index)
{
    assert(index < (int)layers.size());
    return layers[index];
}


int
SwaGraphHierarchy ::  getNumberOfLayers() const
{
    return layers.size();
}

/** Given a scale sC (level of the hierarchy), returns an integer array, one entry for each pixel,
 * representing the label it is taking at scale sC.
 *
 * Commented by A.Y.C.Chen
 */
int* SwaGraphHierarchy::getSegmentsFromScale(int sC)
{
    int t_on = clock(); // timer before calling func
    VI *coarseVI, *fineVI, *tempVI;
    int* L;      // the labels for the final result
    float* Lmax; // the best membership for each element
    int Llength = getLayer(0)->getNumberOfNodes();

    SwaGraphLayer *thisLayer = getLayer(sC);

    coarseVI = new VI(Llength);
    fineVI   = new VI(Llength);

    L    = (int*)calloc(Llength, sizeof(int));
    Lmax = (float*)calloc(Llength, sizeof(float));

    for (int i=0; i<Llength; i++)
    {
        L[i]=-1;
        Lmax[i]=-1.0;
    }

    int thisLayer_numNodes = thisLayer->getNumberOfNodes();
    for (int segNum=0; segNum<thisLayer_numNodes; segNum++)
    {
        //coarseVI->reset();
        delete coarseVI; coarseVI=NULL;
        coarseVI = new VI(Llength);
        coarseVI->add(segNum, 1.0);

        // start at sC and go down toward 0
        // implement the interpolation rule u(i) = sum(k) p(i,k) U(k)
        for (int j=sC; j>0; j--)
        {
           SwaGraphLayer *cLayer = getLayer(j);
           //fineVI->reset();
           delete fineVI; fineVI=NULL;
           fineVI = new VI(Llength);

           for (int k=coarseVI->nextSetBit(0); k>=0 && k<(int)coarseVI->getBitSize(); k=coarseVI->nextSetBit(k+1))
           {
               // go through each child connected to this node, comp value
               int kChLSize = cLayer->getNode(k)->getNumberOfChildren();
               for (int i=0;i<kChLSize;i++)
               {
                   fineVI->add(cLayer->getNode(k)->getChild(i)->getLayerIndex(),
                              cLayer->getNode(k)->getChildWeight(i) * coarseVI->val[k]);
               }
           }

            tempVI = coarseVI;
            coarseVI = fineVI;
            fineVI = tempVI;
        } // close of scale for loop for this segment

        // the coarseVI now holds the finest level
        // take the values from the fineVI and plop them into the image

        for (int k=coarseVI->nextSetBit(0); k>=0 && k<(int)coarseVI->getBitSize(); k=coarseVI->nextSetBit(k+1))
        {
            if (coarseVI->val[k] > Lmax[k])
            {
                Lmax[k] = coarseVI->val[k];
                L[k] = segNum;
            }
        }
    }
    delete coarseVI;
    delete fineVI;
    free(Lmax);
    int t_off = clock(); // timer after all is done
    cout << "Layer " << sC << " segments computed in "<< (static_cast<float>(t_off-t_on))/CLOCKS_PER_SEC << " seconds" << endl;

    return L;
}


void SwaGraphHierarchy::invalidateAllCaches()
{
    for (unsigned int i=1;i<layers.size();i++)
    {
        for (int j=0;j<layers[i]->getNumberOfNodes();j++)
        {
            layers[i]->getNode(j)->cacheInvalidateAll();
        }
    }
}
//////////////   End of SwaGraphHierarchy


//////////////   VI class
VI::VI(int i)
{
   //idx.reserve(i);
   this->bsize = i;
   val = (float*)calloc(i, sizeof(float));
   idx = (bool*)calloc(i, sizeof(bool));
   //initialize the bitset...
   //for(int j=0; j<this->bsize; j++)
      //idx[j] = false;

}

VI::~VI()
{
  //idx.clear();
  free(idx);
  free(val);
  val=NULL;
}

void VI::add(unsigned int i, float f)
{
    idx[i]=true;
    val[i] += f;
}

// returns the index number of the next "true" segment, commented by A.Y.C.Chen
int VI::nextSetBit(int i)
{
   int retval=-1;
   for (int j=i; j<(int)this->bsize; j++)
      if (idx[j]==true)
      {
         retval = j;
         break;
      }
   return retval;
}

void VI::reset()
{
   for(int i=0; i<(int)this->bsize; i++)
   {
       val[i] = 0.0;
       idx[i] = false;
   }
//for(int i=nextSetBit(0); i>=0; i=nextSetBit(i+1))   idx.reset();
}
//////////////   End of VI

////////////////////////////////////////////////////////////////
//// Classes for building soft hierarchies (as in SWA)      ////
////                 --- definition ENDS ---                ////
////////////////////////////////////////////////////////////////






/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
//
//              Global Functions
//
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
bool sortWeight(SwaEdge* a, SwaEdge* b)
{
   return a->getWeight() < b->getWeight();
}






} // namespace vpml_generalgraphs
