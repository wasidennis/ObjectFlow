/**
 * graphs.h
 * created by jcorso; augmented by aychen.
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
 * Class definitions for representing graphs and nodes for the Graph-Shifts code.
 *  (Originally taken from the graph classes in the BMA project)
 *
 * Quick properties:
 *  1)  This is a hard graph hierarchy (each node has only one parent).
 *
 * Since the vpml_graphshifts namespace will be long to type, you can write
 *  namespace GS = vpml_graphshifts;
 * in other source files to save some space if necessary.
 *
 */

#ifndef GRAPHS_H
#define GRAPHS_H

// C libraries
#include <cstdio>
#include <cmath>
#include <cassert>
#include <cfloat>
#include <cstdlib>

#ifdef __LINUX
#include <values.h>
#endif

// C++ libraries
#include <iostream>
#include <vector>
#include <set>
#include <bitset>
#include <list>

// VPML libraries
#include "images.h"

// headers files from current project
#include "settings.h"



namespace vpml_graphs
{
// original typedef in GS and BMA code
typedef unsigned int uint;
typedef unsigned char byte;

// additional typedef in the SWA code
typedef float Weight;


////////////////////////////////////////////////////////////////
//// Classes for building hard hierarchies (as in GS & BMA) ////
////               --- declaration BEGINS ---               ////
////////////////////////////////////////////////////////////////

class Edge;
class Node;
struct ltNodeComparator
{
  bool operator()(const Node* s1, const Node* s2) const;
};
typedef std::set<Node*, ltNodeComparator> NodeSet;
typedef std::set<Node*, ltNodeComparator>::iterator NodeSetIterator;

/** The following set of classes: Node, Edge, CommonNode, LeafNode, and InternalNode
 * are for building hard hierarchies (where each node has only 1 parent and children
 * and parent ) serves as the backbone for other classes, such as the VNodes used in Albert's VGS.
 *
 */

/** The Node class defines the interface for all nodes in the graph
 * through some pure virtual functions
 */
class Node
{
public:
    virtual ~Node(){};

    //  static structural functions
    virtual int           getUID() const=0;
    virtual const real&   getWeight(int neighborIndex) const=0;

    virtual Node* const   getParent() const=0; // different from SWA's getParent(int parentIdx)

    virtual int           getNumberOfChildren() const=0;
    virtual NodeSet&      getChildren()=0;
    virtual NodeSet&      getLeaves()=0;

    virtual int           getNumberOfNeighbors() const=0;
    virtual Node*         getNeighbor(int neighborIndex) const=0;
    virtual Node* const   getConstNeighbor(int neighborIndex) const=0;


    // static structural functions that are not used in SwaNode
    virtual int           getLengthToNeighbor(int neighborIndex) const=0;
    virtual void          setLengthToNeighbor(int neighborIndex, int length)=0;
    virtual int           addLengthToNeighbor(int neighborIndex, int length)=0;
    virtual int           getLengthToNeighbor(const Node* const neighbor) const=0;
    virtual void          setLengthToNeighbor(Node* const neighbor, int length)=0;
    virtual int           addLengthToNeighbor(Node* const neighbor, int length)=0;

    virtual int           getNeighborIndex(const Node* const neighbor) const=0;


    // These functions alter the graph dynamically
    virtual int     addChild(Node* newChild)=0; // SWA's addChild takes 2 parameters
    virtual int     addNeighbor(Node* newNeighbor,real weight)=0;
    virtual int     addNeighbor(Edge* newNeighbor)=0; //Swa's neighbor cannot be added without weights
    virtual void    removeChild(Node* child)=0; // not used in SWA
    virtual void    setParent(Node* newParent)=0;  // SWA's setParent takes 2 parameters

    //  attributes of the nodes
    virtual byte    getIntensity() const=0;
    virtual byte    getLabelIndex() const=0;
    virtual void    setLabelIndex(byte l_)=0;
    virtual int     getMass() const=0;
    virtual void    setMass(int m_)=0;
    virtual float   getX() const=0;
    virtual float   getY() const=0;
    virtual float   getZ() const=0;


    //unary energy cache at nodes
    virtual void    allocateUECache(int size)=0;
    virtual real    getUECache(int index)=0;
    virtual void    setUECache(int index, real value)=0;
    virtual real    addUECache(int index, real value)=0;
    //binary energy cache at nodes for HOPS ver.2
    virtual void    allocateBECache(int size)=0;
    virtual real    getBECache(int index)=0;
    virtual void    setBECache(int index, real value)=0;
    virtual real    addBECache(int index, real value)=0;
    //general purpose histogram, used in VGS ver.2, to be added

    // some handy operator overloading
    int operator == (const Node& n) const
    {
        return (getUID() == n.getUID());
    }
    int operator != (const Node& n) const
    {
        return (getUID() != n.getUID());
    }


};


/** A standard edge in the graph will point to another node and carry a
 * real-valued weight.
 * It also stores the length of the boundary between the two nodes.
 */
class Edge
{
friend class CommonNode;

protected:
    Node*  node;
    real   weight;
    int    length;  // added so that we can store the length of the boundary
                    // between the two nodes that this Edge connects
public:
    Edge(Node* n, real w=1.0f, int length_=1) : node(n), weight(w), length(length_) {}
    inline real&         addToWeight(real w) {weight += w; return weight;}
    inline int           addToLength(int l) {length += l; return length;} // l is +-

    inline int           getLength() const {return length;};
    inline Node*         getNode() const {return node;}
    inline const real&   getWeight() const {return weight;}

    inline Node*         setNode(Node* n) {node = n; return node;}
    inline int           setLength(int length_) {length=length_; return length;}
    inline real          setWeight(real w) {weight = w; return weight;}
};


class GraphLayer
{
private:

    /** Nodes are internally represented as a vector, which facilitates
     * growing or shrinking (sometimes helpful during instantiation).
     * Parents, Neighbors, and Children are stored as pointers so that we don't
     *  have to worry about problems arising from changing a graph's node ordering.
     */
    std::vector<Node*> nodes;

public:
    virtual ~GraphLayer();

    virtual inline void       addNode(Node* N) {nodes.push_back(N);};
    virtual void              addEdge(Node* a, Node* b, real weight, int length);
    virtual Node*             getNode(int nodeIndex)
                                  {assert(nodeIndex < (int)nodes.size());
                                  return nodes[nodeIndex];};
    virtual const Node*       getNode(int nodeIndex) const
                                  {assert(nodeIndex < (int)nodes.size());
                                  return nodes[nodeIndex];};
    virtual Node* const       getNodeConst(int nodeIndex)
                                  {assert(nodeIndex < (int)nodes.size());
                                  return nodes[nodeIndex];};
    virtual const Node* const getNodeConst(int nodeIndex) const
                                  {assert(nodeIndex < (int)nodes.size());
                                  return nodes[nodeIndex];};
    virtual inline int        getNumberOfNodes() const { return nodes.size(); };

    virtual void              appendToFile(FILE* f);
    virtual void              reserveStorage(int n);

    virtual Node& operator [] (int index)
    {
        return *(nodes[index]);
    }

    virtual const Node& operator [] (int index) const
    {
        return (const Node&)(*(nodes[index]));
    }

};


class GraphHierarchy
{
private:
    std::vector<GraphLayer*> layers;

public:
    virtual ~GraphHierarchy();

    virtual void                 addLayer(GraphLayer* newLayer);
    virtual GraphLayer*          getLayer(int index);
    virtual int                  getNumberOfLayers() const;

    virtual GraphLayer& operator [] (int i)
    {
        return *(layers[i]);
    }

    virtual const GraphLayer& operator [] (int i) const
    {
        return (const GraphLayer&)(*(layers[i]));
    }

};

////////////////////////////////////////////////////////////////
//// Classes for building hard hierarchies (as in GS & BMA) ////
////                --- declaration ENDS ---                ////
////////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////
//// Classes for building soft hierarchies (as in SWA)      ////
////               --- declaration BEGINS ---               ////
////////////////////////////////////////////////////////////////

class SwaNode;
class SwaEdge;
struct ltSwaNodeComparator
{
  bool operator()(const SwaNode* s1, const SwaNode* s2) const;
};
typedef std::set<SwaNode*, ltSwaNodeComparator> SwaNodeSet;

/** The SwaNode class defines the interface for all nodes in the graph
 * ...through some pure virtual functions
 */
class SwaNode : public Node
{
private:
    // dummy variables for overriding the inherited Node functions
    NodeSet nodeSet;
    Node* node;
public:
    virtual ~SwaNode(){ };

    //  static structural functions
    virtual int              getUID() const=0; //
    virtual const Weight&    getWeight(int neighborIndex) const=0; //
    virtual int              getNumberOfParents() const=0;
    virtual SwaNode* const   getParent(int parentIdx) const=0;
    virtual int              getNumberOfChildren() const=0; //
    virtual SwaNodeSet&      getSwaLeaves()=0;
    virtual SwaNode*         getChild(int childIndex) const=0;
    virtual int              getNumberOfNeighbors() const=0; //
    virtual SwaNode*         getNeighbor(int neighborIndex) const=0;
    virtual SwaNode* const   getConstNeighbor(int neighborIndex) const=0;


    // These functions alter the graph dynamically
    virtual int     addChild(SwaNode* newChild, float weight)=0;
    virtual int     addNeighbor(SwaNode* newNeighbor,float weight)=0;
    virtual int     addParent(SwaNode* newParent,float weight)=0;
    virtual void    setParent(SwaNode* newParent, int parentIndex=0)=0;
    virtual void    setWeight(int neighborIndex, Weight weight)=0;


    //  attributes of the nodes
    virtual float   getNormalizedIntensity() const=0;
    virtual byte    getLabelIndex() const=0;
    virtual float   getX() const=0; //
    virtual float   getY() const=0; //
    virtual float   getZ() const=0; //
    virtual int     getMinX() const=0;
    virtual int     getMinY() const=0;
    virtual int     getMinZ() const=0;
    virtual int     getMaxX() const=0;
    virtual int     getMaxY() const=0;
    virtual int     getMaxZ() const=0;

    virtual void    cacheInvalidateAll()=0;

    virtual void    setLabelIndex(byte l_)=0;

    //SWA supported functions
    virtual void   addToAverageIntensity(float v)=0;
    virtual void   addToSelfAffinity(float v)=0;
    virtual void   addToVolume(float val)=0;
    virtual void   addToX(float val)=0;
    virtual void   addToY(float val)=0;
    virtual void   addToZ(float val)=0;
    virtual void   clearNeighbors()=0;
    virtual float  getAverageIntensity()=0;
    virtual float  getChildWeight(int childIndex) const=0;
    virtual int    getLayerIndex() const=0;  //id of node in the layer
    virtual SwaEdge*  getNeighborLink(int neighborIndex) const=0;
    virtual float  getParentWeight(int parentIdx) const=0;
    virtual float  getSaliency()=0;
    virtual float  getSelfAffinity()=0;
    virtual float  getVolume()=0;
    virtual void   removeNeighbor(int n_index)=0;
    virtual void   removeRevNeighbor(SwaNode* n)=0;
    virtual void   setLayerIndex(int l)=0;
    virtual void   setSaliency(float s)=0;

    // override unused functions inherited from Node
    Node* const       getParent() const {assert(0); return node;};
    virtual NodeSet&  getChildren() {assert(0); return nodeSet;};
    virtual NodeSet&  getLeaves() {assert(0); return nodeSet;};

    virtual int       getLengthToNeighbor(int neighborIndex) const {assert(0); return -1;};
    virtual void      setLengthToNeighbor(int neighborIndex, int length) {assert(0); };
    virtual int       addLengthToNeighbor(int neighborIndex, int length) {assert(0); return -1;};
    virtual int       getLengthToNeighbor(const Node* const neighbor) const {assert(0); return -1;};
    virtual void      setLengthToNeighbor(Node* const neighbor, int length) {assert(0); };
    virtual int       addLengthToNeighbor(Node* const neighbor, int length) {assert(0); return -1;};
    virtual int       getNeighborIndex(const Node* const neighbor) const {assert(0); return -1;};

    virtual int       addChild(Node* newChild) {assert(0); return -1;};
    virtual int       addNeighbor(Node* newNeighbor,real weight) {assert(0); return -1;};
    virtual int       addNeighbor(Edge* newNeighbor) {assert(0); return -1;};
    virtual void      removeChild(Node* child) {assert(0); };
    virtual void      setParent(Node* newParent) {assert(0); };
    virtual byte      getIntensity() const {assert(0); return -1;};
    virtual int       getMass() const {assert(0); return -1;};
    virtual void      setMass(int m_) {assert(0); };

    virtual void      allocateUECache(int size) {assert(0); };
    virtual real      getUECache(int index) {assert(0); return -1.0;};
    virtual void      setUECache(int index, real value) {assert(0); };
    virtual real      addUECache(int index, real value) {assert(0); return -1.0;};
    virtual void      allocateBECache(int size) {assert(0); };
    virtual real      getBECache(int index) {assert(0); return -1.0;};
    virtual void      setBECache(int index, real value) {assert(0); };
    virtual real      addBECache(int index, real value) {assert(0); return -1.0;};


    // CVPR2012 Functions added to include color vector at each node
    virtual const float* getNormalizedColor() const =0;
    virtual void     setColor(float* c_) =0;
    virtual void     addToAverageColor(float* val) =0;
    virtual float* getAverageColor() =0;

};



/** A standard edge in the graph will point to another node and carry a
 * real-valued weight.
 */
class SwaEdge
{
protected:
    SwaNode*  node;
    Weight weight;
public:
    SwaEdge(SwaNode* n, Weight w=1.0f) : node(n), weight(w) {}
    inline Weight&       addToWeight(Weight w) {weight += w; return weight;}
    inline const Weight& getWeight() const {return weight;}
    inline SwaNode*         getNode() const {return node;}

    inline Weight        setWeight(Weight w) {weight = w; return weight;}
    inline SwaNode*         setNode(SwaNode* n) {node = n; return node;}

   //SWA supported functions
   //these are friend operators, so that they can take 2 arguments
   //note that the less than operator is different from ==
   friend bool operator<(const SwaEdge &a, const SwaEdge &b)
   {
        printf("I am getting called...\n");
      return a.weight < b.weight;
   }

   friend bool operator==(const SwaEdge &a, const SwaEdge &b)
   {
      return a.node->getUID() == b.node->getUID();
   }
};



/** The SwaCommonNode defines the basic Node class representation for this project.
 * Any representational member that is dependent on the type of node will be
 * stored in some sub-class
 */
class SwaCommonNode : public SwaNode
{
protected:
    int uid;  /** A unique index for this node.  Not always the index in the node array
               *   unless the programmer wants to make it such! */
    std::vector<SwaEdge*> neighbors;

    // attributes
    float x,y,z;
    float saliency, selfAffinity, volume, intensity;
    int layerIndex;
    byte labelIndex;

    // CVPR2012
    float color[3];

public:
    SwaCommonNode();
    ~SwaCommonNode();

    virtual int           addNeighbor(SwaNode* newNeighbor, float w=1.0f);
            int           addNeighbor(SwaEdge* newNeighbor);
    virtual int           getUID() const {return uid;};
    virtual SwaNode*         getNeighbor(int neighborIndex) const;
    virtual SwaNode* const   getConstNeighbor(int neighborIndex) const;
    virtual int           getNumberOfNeighbors() const {return neighbors.size();};
    virtual const Weight& getWeight(int neighborIndex) const;
    inline  void          setUID(int UID) {uid = UID;}
    virtual void          setWeight(int neighborIndex, Weight w)
                                   {neighbors[neighborIndex]->setWeight(w);}

    // attribute functions
    inline virtual float   getX() const {return x;};
    inline virtual float   getY() const {return y;};
    inline virtual float   getZ() const {return z;};

    inline virtual float   getNormalizedIntensity() const {return intensity;};
    inline virtual byte    getLabelIndex() const {return labelIndex;};
    inline void            setX(float x_) {x=x_;};
    inline void            setY(float y_) {y=y_;};
    inline void            setZ(float z_) {z=z_;};
    inline void            setIntensity(float i_) {intensity=i_;};
    inline virtual void    setLabelIndex(byte l_) {labelIndex=l_;};

    // CVPR2012
    inline virtual const float* getNormalizedColor() const {return color;};
    inline virtual void    setColor(float* c_) {color[0] = c_[0]; color[1] = c_[1]; color[2] = c_[2];};

    //  Pure Virtual methods that still need to be defined
    //  we define them here as empty functions so that the SwaCommonNode
    //   can actually be used as a standard graph node (with no notion
    //   of hierarchy, which the remaining virtuals specify).
    virtual int    addChild(SwaNode* newChild, float w=1.0f)    { return -1; }
    virtual int    addParent(SwaNode* newParent, float w=1.0f)  { return -1; }
    virtual SwaNode*  getChild(int childIndex) const            { return 0x0; }
    virtual int    getNumberOfChildren() const               {return 0;};
    virtual int    getNumberOfParents() const                {return 0;};
    virtual SwaNode* const getParent(int parentIdx=0) const     {return 0x0;};
    virtual void   setParent(SwaNode* newParent, int Index=0)   { };


    //additional functions to supprt SWA
    virtual void   addToAverageIntensity(float val)       { intensity += val; }
    virtual void   addToSelfAffinity(float val)           { selfAffinity += val; }
    virtual void   addToVolume(float val)                 { volume += val; }
    virtual void   addToX(float val)                      { x += val; }
    virtual void   addToY(float val)                         { y += val; }
    virtual void   addToZ(float val)                         { z += val; }
    virtual void   clearNeighbors();
    virtual int    getLayerIndex() const                   { return layerIndex; }
    virtual float  getAverageIntensity()                  { return intensity; }
    virtual float  getChildWeight(int childIndex) const   { return 0.0; };
    virtual SwaEdge*  getNeighborLink(int neighborIndex) const;
    virtual float  getParentWeight(int parentIdx) const   { return 0.0; };
    virtual float  getSaliency()                          { return saliency; }
    virtual float  getSelfAffinity()                      { return selfAffinity; }
    virtual float  getVolume()                            { return volume; }
    virtual void   removeNeighbor(int n_index);
    virtual void   removeRevNeighbor(SwaNode* node);
    virtual void   setLayerIndex(int l)                    { layerIndex = l; }
    virtual void   setSaliency(float s)                   { saliency = s; }

    // CVPR2012
    virtual void     addToAverageColor(float* val) {color[0] += val[0]; color[1] += val[1]; color[2] += val[2];};
    virtual float*   getAverageColor() { return color; }
};


/**
 * This is a node with no children.
 * The parents are stored as an array rather than a vector because of the
 *  general rule that there will be a fixed number of parents known at node
 *  construction time.  If this is not the case, then you can either add a new
 *  parent (causing realloc'ing of the array) or write a class to use vector
 */
class SwaLeafNode : public SwaCommonNode
{
private:
    static std::set<SwaNode*, ltSwaNodeComparator> leafHolder;

protected:
   std::vector<SwaEdge*> parents;

public:
    SwaLeafNode();
    virtual ~SwaLeafNode();

    virtual int         addChild(SwaNode* newChild, float w=1.0)    { return -1; }
    virtual SwaNodeSet&  getSwaLeaves();
    virtual int         addParent(SwaNode* newParent, float w=1.0);
    virtual int         getNumberOfChildren() const { return 0;}
    virtual int         getNumberOfParents() const  { return parents.size(); }
    virtual SwaNode* const getParent(int parentIdx=0) const;
    virtual float       getParentWeight(int parentIdx) const;

    inline virtual int getMinX() const {return (int)x;};
    inline virtual int getMinY() const {return (int)y;};
    inline virtual int getMinZ() const {return (int)z;};
    inline virtual int getMaxX() const {return (int)x;};
    inline virtual int getMaxY() const {return (int)y;};
    inline virtual int getMaxZ() const {return (int)z;};

    virtual void setParent(SwaNode* newParent, int parentIndex=0);
    virtual void cacheInvalidateAll() { }
};


/** A standard internal node in a soft graph-hierarchy.
 * Sub-Classes from a leaf node to add the children capabilities.
 * The parent capabilities are already added by the SwaLeafNode
 *
 */
class SwaInternalNode : public SwaLeafNode
{
protected:
    std::vector<SwaEdge*> children;  // much more dynamic than parents...
                                  // so, we waste the 8 bytes per Node
    std::set<SwaNode*, ltSwaNodeComparator> leaves;
    int minx,miny,minz,maxx,maxy,maxz;

    static const int numMoments=3;

public:
    SwaInternalNode();
    virtual ~SwaInternalNode();

    virtual int   addChild(SwaNode* newChild, float w=1.0f);
    virtual int   getNumberOfChildren() const {return children.size();}
    virtual SwaNode* getChild(int childIndex) const;
    virtual SwaNodeSet& getSwaLeaves(){return leaves;}

    inline virtual int     getMinX() const {return minx;};
    inline virtual int     getMinY() const {return miny;};
    inline virtual int     getMinZ() const {return minz;};
    inline virtual int     getMaxX() const {return maxx;};
    inline virtual int     getMaxY() const {return maxy;};
    inline virtual int     getMaxZ() const {return maxz;};
    inline void setMinX(int x) {minx = x;}
    inline void setMinY(int y) {miny = y;}
    inline void setMinZ(int z) {minz = z;}
    inline void setMaxX(int x) {maxx = x;}
    inline void setMaxY(int y) {maxy = y;}
    inline void setMaxZ(int z) {maxz = z;}

    virtual void    mergeIntoLeafSet(SwaNode*);
    virtual void    mergeIntoLeafSet(std::set<SwaNode*, ltSwaNodeComparator>& S);

    virtual void cacheInvalidateAll();

    //additional functions to support SWA
    virtual float  getChildWeight(int childIndex) const;
};


class SwaGraphLayer : public GraphLayer
{
protected:

    /** Nodes are internally represented as a vector, which facilitates
     * growing or shrinking (sometimes helpful during instantiation).
     * Parents, Neighbors, and Children are stored as pointers so that we don't
     *  have to worry about problems arising from changing a graph's node ordering.
     */
    std::vector<SwaNode*> nodes;

    // variables added in the SWA implementation
    int id;
    int maxUID;
    int maxNumCouplings;
    int minNumCouplings;
    double avgNumCouplings;
    int maxNumChildren;
    int minNumChildren;
    double avgNumChildren;

public:
    SwaGraphLayer();
    ~SwaGraphLayer();

    // functions that overwrite the ones defined in GraphHierarchy
    inline void          addNode(SwaNode* N) {nodes.push_back(N);};

    SwaNode*             getNode(int nodeIndex)
                              {assert(nodeIndex < (int)nodes.size());
                               return nodes[nodeIndex];};
    const SwaNode*       getNode(int nodeIndex) const
                              {assert(nodeIndex < (int)nodes.size());
                               return nodes[nodeIndex];};
    SwaNode* const       getNodeConst(int nodeIndex)
                              {assert(nodeIndex < (int)nodes.size());
                               return nodes[nodeIndex];};
    const SwaNode* const getNodeConst(int nodeIndex) const
                              {assert(nodeIndex < (int)nodes.size());
                               return nodes[nodeIndex];};
    inline int           getNumberOfNodes() const { return nodes.size(); };

    void                 appendToFile(FILE* f);
    void                 reserveStorage(int n);



    //functions added in the SWA implementation
    void     computeStats();
    void     checkForSalientSegments(float cutCoeff);
    int      getID()                 { return id; }
    int      getMaxNumCouplings()    { return maxNumCouplings; }
    int      getMinNumCouplings()    { return minNumCouplings; }
    double   getAvgNumCouplings()    { return avgNumCouplings; }
    int      getMaxNumChildren()    { return maxNumChildren; }
    int      getMinNumChildren()    { return minNumChildren; }
    double   getAvgNumChildren()    { return avgNumChildren; }
    int      getMaxUID()             { return maxUID; }
    void     removeNode(int i);
    bool*    selectBlockPixels(int level, int volSortingStart, float stronglyConnectedCoeff);
    void     setID(int i)            { id = i; }
    void     setMaxUID(int i)        { maxUID = i; }

    // override unused functions inherited from GraphLayer
    virtual inline void       addNode(Node* N) {assert(0); };
    virtual void              addEdge(Node* a, Node* b, real weight, int length) {assert(0); };

};


class SwaGraphHierarchy : public GraphHierarchy
{
protected:
    std::vector<SwaGraphLayer*> layers;

    // variables added in the SWA implementation
    int volSortingStart;
    bool should_cut_salient_segs;

public:
    SwaGraphHierarchy(bool should_cut_salient_segs=false);
    ~SwaGraphHierarchy();

    // functions that overwrite the ones defined in GraphHierarchy
    void                 addLayer(SwaGraphLayer* newLayer);
    SwaGraphLayer*       getLayer(int index);
    int                  getNumberOfLayers() const;


    // functions added in the SWA implementation
    int*                 getSegmentsFromScale(int sC);
    void                 invalidateAllCaches();
    bool                 shouldCutSalientSegmentFlag()    { return should_cut_salient_segs; }
    int                  getVolSortingStart()             { return volSortingStart; }

    // override unused functions inherited from GraphLayer
    virtual void                 addLayer(GraphLayer* newLayer) {assert(0); };
};

class VI
{
public:
   size_t bsize;
   float* val;
   //std::bitset<1048576> idx;  //2^20
   //std::vector<bool> idx;     //special implementation of bitset!
   bool* idx;

   VI(const int i);
   ~VI();
   void add(unsigned int i, float f);
   size_t getBitSize()  { return bsize; }
   int nextSetBit(int i);
   void reset();
};

////////////////////////////////////////////////////////////////
//// Classes for building soft hierarchies (as in SWA)      ////
////               --- declaration ENDS ---                 ////
////////////////////////////////////////////////////////////////






//// global functions down here
bool sortWeight(SwaEdge* a, SwaEdge* b);


} // namespace vpml_graphs


#endif /*GRAPHS_H*/
