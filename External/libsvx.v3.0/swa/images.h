/**
 * Last Revision Information (full log at bottom of file)
 *
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
 *  MOST of the functionality in the images classes are not used at 
 *  all in libsvx (rather just the container members for the pixel 
 *  data are used and IO).  
 *
 * The images header file defines the image classes.  All the image classes
 *  are defined here.
 *
 */

#ifndef IMAGES_H
#define IMAGES_H

#include <stdio.h>
#include <stdlib.h>


#ifndef C_DEFINES_ONLY

extern "C"
{

#endif /*!C_DEFINES_ONLY*/

//  now define the global function names for the image library calls
// the naming scheme for these images is directly defined after the IPP scheme
#ifdef USE_IPP

#  include <ipp.h>

	typedef IppiSize ImageSize;
	typedef IppiRect ImageRect;

#define ImageHintAlgorithm IppHintAlgorithm
#define ImageCmpOp IppCmpOp

#  define imageAbsDiff_8u_C1R     ippiAbsDiff_8u_C1R
#  define imageCompare_8u_C1R     ippiCompare_8u_C1R
#  define imageConvert_8u32f_C1R  ippiConvert_8u32f_C1R
#  define imageConvValid_32f_C1R  ippiConvValid_32f_C1R
#  define imageMalloc_8u_C1       ippiMalloc_8u_C1
#  define imageMinMax_16s_C1R     ippiMinMax_16s_C1R
#  define imageSet_8u_C1R         ippiSet_8u_C1R
#  define imageSet_32s_C1R        ippiSet_32s_C1R
#  define imageSqr_32f_C1IR       ippiSqr_32f_C1IR
#  define imageSum_32f_C1R        ippiSum_32f_C1R
#  define imageSum_8u_C1R         ippiSum_8u_C1R


#else

	typedef struct tagImageSize {
		int width;
		int height;
	} ImageSize;

	typedef struct tagImageRect {
		int x;
		int y;
		int width;
		int height;
	} ImageRect;

	typedef enum {
	    ippCmpLess,
	    ippCmpLessEq,
	    ippCmpEq,
	    ippCmpGreaterEq,
	    ippCmpGreater
	} ImageCmpOp;

	typedef enum {
		ippAlgHintNone,
		ippAlgHintFast,
		ippAlgHintAccurate
	} ImageHintAlgorithm;

	int            imageAbsDiff_8u_C1R(unsigned char* src1, int step1,
			          unsigned char* src2, int step2,
			          unsigned char* dst, int step, ImageSize size);
	int            imageCompare_8u_C1R(unsigned char* src1, int step1,
				          unsigned char* src2, int step2,
				          unsigned char* dst, int step,
				          ImageSize size, ImageCmpOp op);
	int            imageConvert_8u32f_C1R(unsigned char* src, int srcStep,
			          float* dst, int dstStep, ImageSize size);
	int            imageConvValid_32f_C1R(float* src, int srcStep, ImageSize srcSize,
			                   float* krn, int krnStep, ImageSize krnSize,
			                   float* dst, int dstStep);
	unsigned char* imageMalloc_8u_C1(int w, int h, int* step);
	int            imageMinMax_16s_C1R(short* src, int srcStep,ImageSize size,
			          short* vMin, short* vMax);
	int            imageSet_8u_C1R(unsigned char value, unsigned char* dst,
		                           int dstStep, ImageSize size);
	int            imageSet_32s_C1R(int value, int* dst, int dstStep, ImageSize size);
	int            imageSqr_32f_C1IR(float* data, int step, ImageSize size);
	int            imageSum_8u_C1R(unsigned char* data, int step, ImageSize size, double* v);
	int            imageSum_32f_C1R(float* data, int step, ImageSize size, double* v, ImageHintAlgorithm hint);


#endif
// these are some C-functions for image processing routines that do not
//  have any IPP counterparts (that I could find)
// they are also defined in the imagefuncs.c source file
// but in a different section of it

	//XXX this is called ippiIntegral
int imageCumSum_8u32s_C1R(unsigned char* src, int srcStep,
	    	              int* dst, int dstStep,
		                  ImageSize srcRoi, ImageSize dstRoi) ;

    //XXXX this is also implemented in IPP, but I havent had the time to wrap it


#ifndef C_DEFINES_ONLY

}  // closes the !C_DEFINES_ONLY extern clause


#include <map>


namespace vpml_images
{

typedef unsigned char byte;

struct Colormap
{
	byte map[256][3];
	void loadFromFile(const char* fn);
	byte& operator () (int index, int band)
	{
		return map[index][band];
	}
	const byte& operator () (int index, int band) const
	{
		return map[index][band];
	}
};

struct ByteComparator
{
  bool operator()(const byte s1, const byte s2) const
  {
    return s1 < s2;
  }
};

class Bytemap : public std::map<byte, byte, ByteComparator>
{
public:
	void loadFromFile(const char* fn);
};



/** The scalarImage2 is the standard grayscale image class
 * Generally data is stored as unsigned chars, but the class is templated
 *  in case the user requires a specific type of image.  The most common
 *  templates are actually instantiated in images.cpp.
 * Since it is likely the consumer of this class will want to use the Intel
 *  IPP libraries, it is recommended that this class be considered a container
 *  for the data rather than anything more complex.  Any image processing
 *  class passed to the optimized image processing libraries will need to know
 *  the specific format of the underlying data stored here.  And, in this
 *  class, we do not know that format...
 * We give constructors that can take an outside already allocated data buffer.
 *  Often the optimized librares require word-aligned boundaries, etc...
 *  Global function calls are given to make the allocation of such objects easy.
 */
template <class T_PX>
class _scalarImage2
{
private:
	bool manage_buffer;  // am I managing my own data buffer
public:
	int step;     // distance in bytes between two consecutive rows
	ImageSize size;  // for calls to image processing functions
	T_PX*  data;  // the underlying data buffer
	T_PX** rows;  // a two dimensional array access to the data


	// these static variables are instantiated and assigned on a per
	//  template value basis in the images.cpp file
	static double MAX_PIXEL_VALUE;

	_scalarImage2(int w, int h, int step=-1, T_PX* buffer=0x0);
	virtual ~_scalarImage2();

	// Functions that never have a type-specific implementation
	inline ImageSize& getSize() {return size;}
	inline const ImageSize& getSize() const {return size;}
	inline int   getStep() {return step;}
	inline int   getStep() const {return step;}
	inline int   getWidth() const {return size.width;}
	inline int   getHeight() const {return size.height;}
	T_PX*        getPixelPointer(int x, int y);
	const T_PX*  getPixelPointer(int x, int y) const;
	             /** gets a pointer to the pixel in first column of row y */
	T_PX*        getRowPointer(int y);
	const T_PX*  getRowPointer(int y) const;
	inline T_PX  getValue(const int index) const { return data[index]; }
	inline T_PX  getValue(const int x, const int y) const {return rows[y][x];}
	inline void  setValue(const int x, const int y, const T_PX v)  {rows[y][x]=v;}
	inline int   W() const {return size.width;}
	inline int   H() const {return size.height;}
	inline bool& Manage() {return manage_buffer;}
	inline const bool& Manage() const {return manage_buffer;}

	// Functions that sometimes have a type-specific implementation but
	// also have a generic implementation
	void  copyFromAndDivideBy(const _scalarImage2<T_PX>*, T_PX value);
	void  divideBy(T_PX value);
	void  fillFromImage(const _scalarImage2<T_PX>* src);
	void  fillWithValue(T_PX value);
	void  getMinMax(T_PX* minValue, T_PX* maxValue);
	float getSum(const int border=0) const;
	void  multiplyBy(T_PX value);
	void  replaceWithMax(T_PX value);
	void  saveToFile(const char* fn) const;
	void  saveToFile_Scale(const char* fn) const;

	void  alphaBlendWithImage(const _scalarImage2<T_PX>* img, float alpha);

	// Functions that require a type-specific implementation and have no
	//  generic implementation (will be a fatal call during debug mode);
	void* makeCumulativeImage() const;


#ifdef IMAGE_ZLIB_CAPABILITY
	// if we have access to zlib at compilation, we want to include the ability
	//  to compress and uncompress our pixel data here
protected:
	bool compressed;
	unsigned char* data_compressed;
	unsigned long  len_compressed;
	
public:
	void compressImage();
	void uncompressImage();	
#endif


	T_PX& operator [] (int index)
	{
		return data[index];
	}

	const T_PX& operator [] (int index) const
	{
		return data[index];
	}

private:
	inline T_PX* PTR_ADDBYTES(T_PX* ptr, size_t bytes)
	{
		return (T_PX*)(((byte*)ptr) + bytes);
	}
	inline T_PX* PTR_ADDBYTES(T_PX* ptr, size_t bytes) const
	{
		return (T_PX*)(((byte*)ptr) + bytes);
	}
};

/**
 * Note that partial specialization of the templates is used to provide
 *  pixel-type specific (optimized) functionality for a function subset.
 */

// some typedefs on the different types of scalarImage2
typedef _scalarImage2<byte> ScalarImage2;
typedef _scalarImage2<short> ScalarImage2S;
typedef _scalarImage2<int> ScalarImage2I;
typedef _scalarImage2<float> ScalarImage2F;


#include "matrix.h"
using namespace vpml_matrix;

template <class T_PX>
class _rgbImage2 : public Matrix<T_PX>
{
public:
	ImageSize size;  // for calls to image processing functions

	// these static variables are instantiated and assigned on a per
	//  template value basis in the images.cpp file
	static double MAX_PIXEL_VALUE;

	_rgbImage2(int w, int h);
	virtual ~_rgbImage2();

	// Functions that never have a type-specific implementation
	inline int   getWidth() const {return size.width;}
	inline int   getHeight() const {return size.height;}
	inline T_PX  getValue(const int x, const int y,const int b) const
	                { return Matrix<T_PX>::getValue(y,3*x+b); }
	inline void  setValue(const int x, const int y, const int b, const T_PX v)
	                { Matrix<T_PX>::setValue(y,3*x+b,v); }
	inline int   W() const {return size.width;}
	inline int   H() const {return size.height;}

	// Functions that sometimes have a type-specific implementation but
	// also have a generic implementation
	void  saveToFile(const char* fn) const;
	void  alphaBlendWithImage(const _rgbImage2<T_PX>* img, float alpha);

	// These functions are overloaded from the super-class Matrix, but
	//  they are not virtual.  i.e., in order for them to be used properly
	//  the compiler's going to need to know that you have an image you are
	//  dealing with (which is most likely always going to be the case)
	/** Return ref to the red object there... */
	T_PX& operator () (int x, int y)
	{
		return Matrix<T_PX>::getElement(y,3*x);
	}
	/** return ref to whichever band you need (red=0,green=1,blue=2) */
	T_PX& operator () (int x, int y, int band)
	{
		return Matrix<T_PX>::getElement(y,3*x+band);
	}

};

typedef _rgbImage2<byte> RGBImage2;



//  Some global image functions
RGBImage2*  apply_colormap(const Colormap& map, const ScalarImage2*);
ScalarImage2*  apply_bytemap(Bytemap& map, const ScalarImage2*);
void convertRGBtoLAB(const RGBImage2* RGB, ScalarImage2*& A, ScalarImage2*& B);
ScalarImage2* convolve(const ScalarImage2* I, const ScalarImage2F*);
ScalarImage2F* convolve(const ScalarImage2F* I, const ScalarImage2F*);
ScalarImage2F* magnitude(const ScalarImage2F* A, const ScalarImage2F* B, bool normalize=false, bool doSqrt=false);
ScalarImage2F* normalize(const ScalarImage2F* I);

// convolution functions via the FFTW library
#ifdef IMAGE_FFTW_CAPABILITY

ScalarImage2F* convolve_fft(ScalarImage2F* I, ScalarImage2F*);

#endif


//  Templated Global Image Functions
template <class T_PX>
ScalarImage2* imboolGreaterThan(_scalarImage2<T_PX>* A, _scalarImage2<T_PX>* B);
template <class T_PX>
_scalarImage2<T_PX>* newImageOneMinus(_scalarImage2<T_PX>* I);


/** Load an image from the disk (infer type from the extension) and
 * instantiate a grayscale byte image. Scaling and converting of data will
 * be done (when the code gets written to do it)
 */
ScalarImage2* load_image_gray(const char *fn);
RGBImage2*    load_image_rgb(const char *fn);
int           save_image_gray(const char *fn,const ScalarImage2* image);
int           save_png_gray(FILE *fp,const ScalarImage2* image);
int           save_image_rgb(const char *fn,const RGBImage2* image);
int           save_png_rgb(FILE *fp,const RGBImage2* image);




/** These definitions handle image file types for reading and writing */
typedef enum
{	UNSUPPORTED,PPM,PNG} e_image_file_type;

e_image_file_type get_image_file_type(const char*fn);


// some other definitions, structures, and functions used by the images

extern bool operator == (const ImageSize& A, const ImageSize& B);



}  // end of namespace vpml_images;




#endif // !C_DEFINES_ONLY



#endif /*IMAGES_H*/
