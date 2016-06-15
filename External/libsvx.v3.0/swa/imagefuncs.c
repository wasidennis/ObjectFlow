/**
 * imagefuncs.c
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
 * imagefuncs defines all the c function calls that we make in the case
 *  that we are not using the IPP library and must define our own image 
 *  processing routines.  Here, these are a subset of all those IPP calls 
 *  available.
 * 
 * imagefuncs also defines any image processing routines that are not available 
 *  in IPP and must be written by hand.  These are interleaved.
 * 
 * The prototypes for these are defines in the images.h header file.
 * 
 * Functions are defined in alphabetical order.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <limits.h>

#define C_DEFINES_ONLY
#include "images.h"
#undef C_DEFINES_ONLY

inline float* FPTR_ADDBYTES(float* ptr, size_t bytes)
{
	return (float*)(((unsigned char*)ptr) + bytes);
}
inline short* SPTR_ADDBYTES(short* ptr, size_t bytes)
{
	return (short*)(((unsigned char*)ptr) + bytes);
}

/////////////  FUNCTIONS WITH NO IPP COUNTERPART





/**
 * Compute the cumulative image in dst (previously allocated) from src.
 * The dstRoi is assumed to be one pixel larger in width and height than the
 *  inputted srcRoi
 */
int imageCumSum_8u32s_C1R(unsigned char* src, int srcStep, 
	    	              int* dst, int dstStep,
		                  ImageSize srcRoi, ImageSize dstRoi)
{
	int x,y;
	
	assert(dstRoi.width == srcRoi.width + 1);
	assert(dstRoi.height == srcRoi.height + 1);
	
	imageSet_32s_C1R(0,dst,dstStep,dstRoi);
	
	for (y=0;y<srcRoi.height;y++)
	{
		unsigned char* s = src + y*srcStep;
		int* d = (int*)(((unsigned char*)dst) + (y+1)*dstStep); // ptr arith :(
		int last=0;
		for (x=0;x<srcRoi.width;x++)
		{
			last += *s++;
			*d++ = last;
		}
	}
	
	for (x=1;x<=srcRoi.width;x++)
	{
		int* d = dst + x;
		int last=0;
		for (y=1;y<=srcRoi.height;y++)
		{
			last += *d;
			*d = last;
			d = (int*)((unsigned char*)d + dstStep);
		}
	}
	
	return 0;
}

//////////////////  END FUNCTIONS WITH NO IPP COUNTERPART


#ifndef USE_IPP

int imageAbsDiff_8u_C1R(unsigned char* src1, int step1,
		          unsigned char* src2, int step2,
		          unsigned char* dst, int step, ImageSize size)
{
	int x,y;
	unsigned char* s1,*s2,*d;
	
	for (y=0;y<size.height;y++)
	{
		s1 = src1 + y*step;
		s2 = src2 + y*step;
		d  = dst  + y*step;
		
		for (x=0;x<size.width;x++,s1++,s2++)
		{
			*d++ = (*s1>*s2)?*s1-*s2:*s2-*s1;
		}
	}
	return 0;
}


inline int imageCmp_8u_l(unsigned char a, unsigned char b) {return a<b;}
inline int imageCmp_8u_le(unsigned char a, unsigned char b) {return a<=b;}
inline int imageCmp_8u_eq(unsigned char a, unsigned char b) {return a==b;}
inline int imageCmp_8u_ge(unsigned char a, unsigned char b) {return a>=b;}
inline int imageCmp_8u_g(unsigned char a, unsigned char b) {return a>b;}

int imageCompare_8u_C1R(unsigned char* src1, int step1,
			            unsigned char* src2, int step2,
				        unsigned char* dst, int step,
				        ImageSize size, ImageCmpOp op)
{
	int (*cmp)(unsigned char, unsigned char) =0x0;
	int x,y;
	unsigned char* u1;
	unsigned char* u2;
	unsigned char* d;
	
	switch (op)
	{
	case ippCmpLess:
		cmp = imageCmp_8u_l;
		break;
		
	case ippCmpLessEq:
		cmp = imageCmp_8u_le;
		break;
		
	case ippCmpEq:
		cmp = imageCmp_8u_eq;
		break;
		
	case ippCmpGreaterEq:
		cmp = imageCmp_8u_ge;
		break;
		
	case ippCmpGreater:
		cmp = imageCmp_8u_g;
		break;
	}
	
	for (y=0;y<size.height;y++)
	{
		u1 = src1 + y*step1;
		u2 = src2 + y*step2;
		d  = dst + y*step;
		for (x=0;x<size.width;x++)
			*d++ = (cmp(*u1++,*u2++) == 1)?255:0;
	}
	
	return 0;
}




int imageConvert_8u32f_C1R(unsigned char* src, int srcStep,
			          float* dst, int dstStep, ImageSize size)
{
	int x,y;
	unsigned char* u;
	float* f;
	
	for (y=0;y<size.height;y++)
	{
		u = src + y*srcStep;
		f = FPTR_ADDBYTES(dst,y*dstStep);
		for (x=0;x<size.width;x++)
			*f++ = (float)(*u++);
	}

	return 0;
}




int imageConvValid_32f_C1R(float* src, int srcStep, ImageSize srcSize,
		                   float* krn, int krnStep, ImageSize krnSize,
		                   float* dst, int dstStep)
// this is crude implementation
// assume kernel is square
// we assume the pointers are valid and the center of the first kernel dropping
//  is at krn->width/2
{
	int sy,sx,ky,kx;
	float* pd;
	float* ps;
	float* pss;
	float* ks;
	float r;
	const int kw2 = krnSize.width / 2;
	
	for (sy=kw2;sy<srcSize.height-kw2;sy++)
	{
		ps = FPTR_ADDBYTES(src,sy*srcStep)+kw2;
		pd = FPTR_ADDBYTES(dst,(sy-kw2)*dstStep);
		for (sx=kw2;sx<srcSize.width-kw2;sx++)
		{
			// for this sx,sy point in src, get a pointer top upper-left corner
			r=0;
			
			for (ky=0;ky<krnSize.height;ky++)
			{
				pss = FPTR_ADDBYTES(ps,(ky-kw2)*srcStep);
				pss -= kw2;
				ks = FPTR_ADDBYTES(krn,ky*krnStep);
				
				for (kx=0;kx<krnSize.width;kx++)
					r += *pss++**ks++;
			}
			
			*pd++ = r;
			ps++;
		}
	}
	
	return 0;
}







unsigned char* imageMalloc_8u_C1(int w, int h, int* step)
{
	unsigned char* p = malloc(sizeof(unsigned char)*w*h);
	*step = w;
	return p;
}


int imageMinMax_16s_C1R(short* src, int srcStep, ImageSize size,
		          short* vMin, short* vMax)
{
	int x,y;
	short* s;
	
	*vMin = SHRT_MAX;
	*vMax = SHRT_MIN;
	
	for (y=0;y<size.height;y++)
	{
		s = SPTR_ADDBYTES(src,y*srcStep);
		for (x=0;x<size.width;x++,s++)
		{
			if (*s > *vMax)
				*vMax = *s;
			if (*s < *vMin)
				*vMin = *s;
		}
	}
	
	return 0;
}



int imageSet_8u_C1R(unsigned char value, unsigned char* dst, int dstStep,
		ImageSize size)
// this overwrites the memory even in the extra pixels beyond the width
//  defined by step
{
	memset(dst,value,size.height*dstStep);
	return 0;
}


int imageSet_32s_C1R(int value, int* dst, int dstStep, ImageSize size)
// if value is 0, then overwriting happens as above for 8u
{
	if (value == 0)
	{
		memset(dst,0,size.height*dstStep);
	}
	else
	{
		int x,y;
		
		for (y=0;y<size.height;y++)
		{
			int* d = dst + y*dstStep;
			for (x=0;x<size.width;x++)
			{
				*d = value;
				d++;
			}
		}
	}
	
	return 0;
}


int imageSqr_32f_C1IR(float* data, int step, ImageSize size)
{
	int x,y;
	float* f;
	
	for (y=0;y<size.height;y++)
	{
		f = FPTR_ADDBYTES(data,y*step);
		for (x=0;x<size.width;x++,f++)
			*f = *f**f;
	}

	return 0;
}


int imageSum_8u_C1R(unsigned char* data, int step, ImageSize size, double* v)
{
	int x,y;
	unsigned char* f;
	
	*v = 0;
	
	for (y=0;y<size.height;y++)
	{
		f = data + y*step;
		for (x=0;x<size.width;x++)
			*v += *f++;
	}

	return 0;
}


int imageSum_32f_C1R(float* data, int step, ImageSize size, double* v, ImageHintAlgorithm hint)
{
	int x,y;
	float* f;
	
	*v = 0;
	
	for (y=0;y<size.height;y++)
	{
		f = FPTR_ADDBYTES(data,y*step);
		for (x=0;x<size.width;x++)
			*v += *f++;
	}

	return 0;
}



#endif  // USE_IPP



