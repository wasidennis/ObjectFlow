/**
 * images.cpp
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
 * Implementation File for the image classes.
 */

#include "images.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <limits.h>
#ifdef __LINUX
#include <values.h>
#endif

#include "png.h"
#include "zlib.h"

#ifdef IMAGE_FFTW_CAPABILITY
#include "fftw3.h"
#endif


namespace vpml_images
{

//  some local defines that make some code more simple
//#define PTR_ADDBYTES(ptr,bytes) (T_PX*)(((byte*)ptr) + bytes)


/**
 * Construct an image object.
 */
template<class T_PX>
_scalarImage2<T_PX>::_scalarImage2(int w, int h, int step, T_PX* buffer)
{
	this->size.width = w;
	this->size.height = h;

	if (buffer==0x0)
	{
		this->data = (T_PX*)malloc(sizeof(T_PX)*w*h);
		this->step = w*sizeof(T_PX);
		this->manage_buffer = true;
	}
	else
	{
		this->data = buffer;
		this->step = step;
		this->manage_buffer = false;
	}

	// now we link up the rows matrix
	rows = (T_PX**)malloc(sizeof(T_PX*)*h);
	for (int i=0; i<h; i++)
		rows[i] = PTR_ADDBYTES(this->data,i*this->step);

#ifdef IMAGE_ZLIB_CAPABILITY
	compressed = false;
	data_compressed = 0x0;		
	len_compressed = -1;  // this is actually an unsigned variable...
#endif

}

template<class T_PX> _scalarImage2<T_PX>::~_scalarImage2()
{
	if ( (this->data) && (this->manage_buffer))
	{
		free(this->data);
		this->data = 0x0;
	}
	if (this->rows)
	{
		free(this->rows);
		this->rows = 0x0;
	}

#ifdef IMAGE_ZLIB_CAPABILITY
	if ( (this->compressed) && (this->data_compressed) )
	{
		free(this->data_compressed);
		this->data_compressed = 0x0;
	}
#endif

}

template<class T_PX>
void  _scalarImage2<T_PX> :: copyFromAndDivideBy(const _scalarImage2<T_PX>* src, T_PX value)
{
	assert(src->size == size);

	for (int y=0;y<H();y++)
	{
		T_PX* pf = getRowPointer(y);
		const T_PX* ps = src->getRowPointer(y);

		for (int x=0;x<W();x++,pf++,ps++)
			*pf = *ps / value;
	}
}

template<class T_PX>
void  _scalarImage2<T_PX> :: divideBy(T_PX value)
{
	for (int y=0;y<H();y++)
	{
		T_PX* pf = getRowPointer(y);
		for (int x=0;x<W();x++,pf++)
			*pf /= value;
	}
}


template<class T_PX>
void  _scalarImage2<T_PX> :: fillFromImage(const _scalarImage2<T_PX>* src)
{
	assert(src->size == size);

	for (int y=0;y<size.height;y++)
	{
		T_PX* px = PTR_ADDBYTES(this->data,y*this->step);
		T_PX* srcpx = PTR_ADDBYTES(src->data,y*src->step);
		for (int x=0;x<size.width;x++)
			*px++ = *srcpx++;
	}

}

template<>
void  _scalarImage2<byte> :: fillFromImage(const _scalarImage2<byte>* src)
{
	assert(src->size == size);

	memcpy(data,src->data,step*size.height);
}


template<class T_PX>
void  _scalarImage2<T_PX> :: fillWithValue(T_PX value)
{
	for (int y=0;y<size.height;y++)
	{
		T_PX* px = PTR_ADDBYTES(this->data,y*this->step);
		for (int x=0;x<size.width;x++)
			*px++ = value;
	}

}

template<>
void  _scalarImage2<byte> :: fillWithValue(byte value)
{
	memset(data,value,step*size.height);
}


template<class T_PX>
T_PX*  _scalarImage2<T_PX> :: getPixelPointer(int x, int y)
{
	return PTR_ADDBYTES(this->data,y*this->step) + x;
}


template<class T_PX>
const T_PX*  _scalarImage2<T_PX> :: getPixelPointer(int x, int y) const
{
	return PTR_ADDBYTES(this->data,y*this->step) + x;
}


template<class T_PX>
T_PX*  _scalarImage2<T_PX> :: getRowPointer(int y)
{
	return rows[y];
}

template<class T_PX>
const T_PX*  _scalarImage2<T_PX> :: getRowPointer(int y) const
{
	return rows[y];
}


template<class T_PX>
void
_scalarImage2<T_PX> ::  getMinMax(T_PX* minValue, T_PX* maxValue)
{
	T_PX minv=(T_PX)(_scalarImage2<T_PX>::MAX_PIXEL_VALUE);
	T_PX maxv=0;

	for (int y=0;y<this->getHeight();y++)
	{
		T_PX* d = PTR_ADDBYTES(this->data,y*this->step);
		for (int x=0;x<this->getWidth();x++)
		{
			if (*d > maxv)
				maxv = *d;
			if (*d < minv)
				minv = *d;
			d++;
		}
	}

	*minValue = minv;
	*maxValue = maxv;
}


template<class T_PX>
float  _scalarImage2<T_PX> :: getSum(const int border) const
{
	float sum=0;

	for (int y=border;y<H()-border;y++)
	{
		const T_PX* pf = getPixelPointer(border,y);
		for (int x=border;x<W()-border;x++,pf++)
			sum += *pf;
	}

	return sum;
}


template<class T_PX>
void*  _scalarImage2<T_PX> :: makeCumulativeImage() const
{
	assert(0);
	return 0x0;
}


template<>
void*  _scalarImage2<byte> :: makeCumulativeImage() const
{
	ScalarImage2I* O = new ScalarImage2I(this->getWidth()+1,this->getHeight()+1);
	imageCumSum_8u32s_C1R(this->data,this->step,O->data,O->step,
			this->size,O->size);
	return O;
}


template<class T_PX>
void  _scalarImage2<T_PX> :: multiplyBy(T_PX value)
{
	for (int y=0;y<H();y++)
	{
		T_PX* pf = getRowPointer(y);
		for (int x=0;x<W();x++,pf++)
			*pf *= value;
	}
}



template<class T_PX>
void  _scalarImage2<T_PX> :: replaceWithMax(T_PX value)
{
	for (int y=0;y<size.height;y++)
	{
		T_PX* px = PTR_ADDBYTES(this->data,y*this->step);
		for (int x=0;x<size.width;x++,px++)
			*px = (*px > value) ? *px : value;
	}
}


template<class T_PX>
void  _scalarImage2<T_PX> :: saveToFile(const char* fn) const
// the generic version of this function simply clamps the pixels
{
	ScalarImage2* T = new ScalarImage2(this->getWidth(),this->getHeight());
	for (int y=0;y<this->H();y++)
	{
		byte* t = T->data + y * T->step ;
		T_PX* d = PTR_ADDBYTES(this->data,y*this->step);
		for (int x=0;x<this->W();x++)
			*t++ = (byte)(*d++);
	}

	save_image_gray(fn,T);

	delete T;
}

template<>
void  _scalarImage2<byte> :: saveToFile(const char* fn) const
{
	save_image_gray(fn,this);
}

template<class T_PX>
void  _scalarImage2<T_PX> :: saveToFile_Scale(const char* fn) const
// scale the pixels by min and max to the [0,255] range
{
	T_PX minv=(T_PX)(_scalarImage2<T_PX>::MAX_PIXEL_VALUE);
	T_PX maxv=0;

	for (int y=0;y<this->getHeight();y++)
	{
		T_PX* d = PTR_ADDBYTES(this->data,y*this->step);
		for (int x=0;x<this->getWidth();x++)
		{
			if (*d > maxv)
				maxv = *d;
			if (*d < minv)
				minv = *d;
			d++;
		}
	}
	//printf("saveToFile_Scale: %f min and %f max\n",(float)minv,(float)maxv);

	maxv = maxv - minv;  // handles the case that minv is nonzero
	ScalarImage2* T = new ScalarImage2(this->getWidth(),this->getHeight());
	for (int y=0;y<this->getHeight();y++)
	{
		byte* t = T->data + y * T->step;
		T_PX* d = PTR_ADDBYTES(this->data,y*this->step);
		for (int x=0;x<this->getWidth();x++)
			*t++ = (byte)(255* (((float)(*d++)) - minv)/maxv);
	}

	save_image_gray(fn,T);

	delete T;
}

template<class T_PX>
void  _scalarImage2<T_PX> :: alphaBlendWithImage(const _scalarImage2<T_PX>* img, float alpha)
{
    assert(img->size == size);

    for (int y=0;y<H();y++)
    {
        T_PX* po = this->getRowPointer(y);
        const T_PX* pb = img->getRowPointer(y);

        for (int x=0;x<W();x++,po++,pb++)
            *po = (*po * alpha) + (*pb * (1-alpha));
    }
}



////  _scalarImage2<T_PX>  functions utilizing ZLIB capabilities

#ifdef IMAGE_ZLIB_CAPABILITY

template<class T_PX>
void _scalarImage2<T_PX> :: compressImage()
{
	if (compressed)
		return;
	compressed=true;

	unsigned long lenbound = compressBound(step*size.height);

	data_compressed = (unsigned char*)malloc(lenbound);
	len_compressed = lenbound;
	compress(data_compressed,&len_compressed,(unsigned char*)data,step*size.height);

	if (len_compressed != lenbound)
		data_compressed = (byte*)realloc(data_compressed,lenbound);

	// leaves rows allocated (but need to relink in uncompressed) but kill data
	// completely disregard whether or not we're managing the memory
	free(data); 
}

template<class T_PX>
void _scalarImage2<T_PX> :: uncompressImage()
{
	if (!compressed)
		return;
	compressed=false;

	unsigned long ulen = step*size.height;
	data=(T_PX*)malloc(ulen);
	uncompress((unsigned char*)data,&ulen,data_compressed,len_compressed);

	free(data_compressed);
	len_compressed = -1;

	// link up the row variables again.
	for (int i=0; i<size.height; i++)
		rows[i] = PTR_ADDBYTES(this->data,i*this->step);
}


#endif





////////////  RGB Image2


template <class T_PX>
_rgbImage2<T_PX> :: _rgbImage2(int w, int h)
{
	Matrix<T_PX>::setDimension(h,3*w);
	size.height = h;
	size.width = w;
}


template <class T_PX>
_rgbImage2<T_PX> :: ~_rgbImage2()
{
}


template <class T_PX>
void  _rgbImage2<T_PX> :: saveToFile(const char* fn) const
{
	assert(0);
}

template <>
void  _rgbImage2<byte> :: saveToFile(const char* fn) const
{
	save_image_rgb(fn,this);
}

template<class T_PX>
void  _rgbImage2<T_PX> :: alphaBlendWithImage(const _rgbImage2<T_PX>* img, float alpha)
{
    assert(img->size == this->size);
    for (int y=0;y<H();y++)
    {
        T_PX* po = this->getRowPointer(y);
        const T_PX* pb = img->getRowPointer(y);

        for (int x=0;x<W()*3;x++,po++,pb++)
            *po = (*po * alpha) + (*pb * (1-alpha));
    }
}






////////////  Global image Functions  and type conversions

RGBImage2*  apply_colormap(const Colormap& map, const ScalarImage2* G)
{
	RGBImage2* C = new RGBImage2(G->W(),G->H());

	for (int y=0;y<G->H();y++)
		for (int x=0;x<G->W();x++)
		{
			byte g = G->getValue(x,y);
			(*C)(x,y  ) = map(g,0);
			(*C)(x,y,1) = map(g,1);
			(*C)(x,y,2) = map(g,2);
		}

	return C;
}


ScalarImage2*  apply_bytemap(Bytemap& map, const ScalarImage2* G)
{
	ScalarImage2* C = new ScalarImage2(G->W(),G->H());

	for (int y=0;y<G->H();y++)
		for (int x=0;x<G->W();x++)
		{
			byte g = G->getValue(x,y);
			C->setValue(x,y,map[g]);
		}

	return C;
}



void convertRGBtoLAB(const RGBImage2* RGB, ScalarImage2*& A, ScalarImage2*& B)
{
	const int w = RGB->W();
	const int h = RGB->H();
	const float XYZn[3] = {1.f,1.f,1.f};
	const float _13 = 1.f/3.f;
	const float rs=255.f/100.f;

	A = new ScalarImage2(w,h);
	B = new ScalarImage2(w,h);

	for (int j=0;j<h;j++)
		for (int i=0;i<w;i++)
		{
			float r,g,b;
			r = ((float)(RGB->getValue(i,j,0))) / 255.f;
			g = ((float)(RGB->getValue(i,j,1))) / 255.f;
			b = ((float)(RGB->getValue(i,j,2))) / 255.f;

			float x,y,z;
			x = 0.412453f*r + 0.35758f*g + 0.180423f*b;
			y = 0.212671f*r + 0.71516f*g + 0.072169f*b;
			z = 0.019334f*r + 0.119193f*g + 0.950227f*b;

		    g = 500.f * (powf(x / XYZn[0], _13) - powf(y / XYZn[1], _13));
		    b = 200.f * (powf(y / XYZn[1], _13) - powf(z / XYZn[2], _13));

		    // do a conversion to map the a and b from -50 : 50 to 0 255
		    //
		    r = rs*(g+50.f);
		    r = (((r < 0) ? 0 : r) > 255) ? 255 : r;
		    A->setValue(i,j,(byte)r);
		    r = rs*(b+50.f);
		    r = (((r < 0) ? 0 : r) > 255) ? 255 : r;
		    B->setValue(i,j,(byte)r);
		}
}



/** Convolve I by K.
 * Convolution is done in floating point on a temporary image.
 * Outputs an image of the same size as I.  Think 'same' in Matlab.
 *
 */
ScalarImage2* convolve(const ScalarImage2* I, const ScalarImage2F* K)
{
	ScalarImage2* R = new ScalarImage2(I->W(),I->H());

	ScalarImage2F* fI = new ScalarImage2F(I->W(),I->H());
	ScalarImage2F* fT = new ScalarImage2F(I->W(),I->H());
	fT->fillWithValue(0.0f);

	for (int y=0;y<I->H();y++)
	{
		float* pfI = fI->getPixelPointer(0,y);
		const byte* pI = I->getPixelPointer(0,y);
		for (int x=0;x<I->W();x++)
			*pfI++ = ((float)(*pI++)) / 255.0f ;
	}

	//fI->saveToFile_Scale("/tmp/fI.png");

	imageConvValid_32f_C1R(fI->data,fI->getStep(),fI->getSize(),
	                       K->data,K->getStep(),K->getSize(),
	                       fT->data+(int)((fT->W()+1)*(K->W()/2)),fT->getStep());

	//fT->saveToFile_Scale("/tmp/fT.png");

	float maxv=-1e20,minv=1e20;
	for (int y=0;y<fT->H();y++)
	{
		float* d = fT->getPixelPointer(0,y);
		for (int x=0;x<fT->W();x++)
		{
			if (*d > maxv)
				maxv = *d;
			if (*d < minv)
				minv = *d;
			d++;
		}
	}
	//printf("min and max of convolved image are %f,%f\n",minv,maxv);
	maxv = maxv - minv;

	for (int y=0;y<I->H();y++)
	{
		float* pfI = fT->getPixelPointer(0,y);
		byte* pR = R->getPixelPointer(0,y);
		for (int x=0;x<I->W();x++)
		{
			int r = (int)(((*pfI++ - minv)/maxv) * 255.f);
			*pR++ = ((r < 0)?0:r)>255?255:r;
		}
	}

	delete fI;
	delete fT;


	return R;
}


/** Convolve I by K.
 * Convolution is done in floating point on a temporary image.
 * Outputs an image of the same size as I.  Think 'same' in Matlab.
 *
 */
ScalarImage2F* convolve(const ScalarImage2F* fI, const ScalarImage2F* K)
{
	ScalarImage2F* fT = new ScalarImage2F(fI->W(),fI->H());
	fT->fillWithValue(0.0f);

	imageConvValid_32f_C1R(fI->data,fI->getStep(),fI->getSize(),
	                       K->data,K->getStep(),K->getSize(),
	                       fT->data+(int)((fT->W()+1)*(K->W()/2)),fT->getStep());

	return fT;
}



#ifdef IMAGE_FFTW_CAPABILITY

// This definition shoud be plain malloc if you want to use the standard 
//  versions or fftwf_malloc if you want to use theirs (which will guarantee
//  alignment).
// Same with free
#define FFTWF_MALLOC malloc
#define FFTWF_FREE   free 

/** Convolve I by K
 * ASSUME: the I is the larger of the two images.  We need to pad the K
 *  with zeros before we compute the FFT on it.
 * ASSUME: Kernel K_ is square and odd-sized (2*w+1).
 */
ScalarImage2F* convolve_fft(ScalarImage2F* I, ScalarImage2F* K_)
{
	int w,h,cw;

	w = I->W();
	h = I->H();
	cw = 2*(w/2+1);

	// copy the image data over to the fftw allocated buffer by row to allow
	//  for steps in the image storage.
	float* Idata = (float*)FFTWF_MALLOC(sizeof(float)*w*h);
	for (int y=0;y<h;y++)
		memcpy(&(Idata[y*w]),I->getRowPointer(y),sizeof(float)*w);

	float* Kdata;
	Kdata = (float*)FFTWF_MALLOC(sizeof(float)*w*h);

	//printf("convolution with kernel size %d %d\n",K_->W(),K_->H());

	if ((w != K_->W()) || (h != K_->H()))
	{
		// copy over the K_into this image of zeros
		memset(Kdata,0,sizeof(float)*w*h);

		// Padding must be done carefully to wrap around such that the center 
		//  of the kernel is at 0,0 in the upper-left.  This is due to the cyclic
		//  definition of the fft+convolution in the library.  Information is 
		//  available at 
		// http://developer.download.nvidia.com/compute/cuda/1_1/Website/projects/convolutionFFT2D/doc/convolutionFFT2D.pdf
		//  among other places
		// This first one is wrong is just naively pads.
#if 0
		for (int y=0;y<K_->H();y++)
		{
			const float* k_ptr = K_->getRowPointer(y);
			float* kptr = K->getRowPointer(y);
			memcpy(kptr,k_ptr,sizeof(float)*K_->W());
		}
#endif

		const int c = K_->W()/2;
		const int kh = K_->H();
		// first chunk the bottom-right of the kernel up to the top-left
		int ky,y;
		for (ky=c,y=0;ky<kh;ky++,y++)
		{
			const float* k_ptr = K_->getPixelPointer(c,ky);
			float* kptr = &(Kdata[y*w]);
			memcpy(kptr,k_ptr,sizeof(float)*(c+1));
		}

		// second chunk the top-right of the kernel to the bottom-left
		for (ky=0,y=h-c;ky<c;ky++,y++)
		{
			const float* k_ptr = K_->getPixelPointer(c,ky);
			float* kptr = &(Kdata[y*w]);
			memcpy(kptr,k_ptr,sizeof(float)*(c+1));
		}

		// third chunk the bottom-left of the kernel to the top-right
		for (ky=c,y=0;ky<kh;ky++,y++)
		{
			const float* k_ptr = K_->getRowPointer(ky);
			float* kptr = &(Kdata[y*w+w-c]);
			memcpy(kptr,k_ptr,sizeof(float)*(c));
		}

		// fourth chunk the top-left of the kernel to the bottom-right
		for (ky=0,y=h-c;ky<c;ky++,y++)
		{
			const float* k_ptr = K_->getRowPointer(ky);
			float* kptr = &(Kdata[y*w+w-c]);
			memcpy(kptr,k_ptr,sizeof(float)*(c));
		}
		
	}
	else
	{
		memcpy(Kdata,K_->getRowPointer(0),sizeof(float)*w*h);
	}

	fftwf_plan fwdI,fwdK,invIK;

	fftwf_complex* Ifft = (fftwf_complex*)FFTWF_MALLOC(sizeof(fftwf_complex)*h*cw);
	fftwf_complex* Kfft = (fftwf_complex*)FFTWF_MALLOC(sizeof(fftwf_complex)*h*cw);
	fftwf_complex* IKfft = (fftwf_complex*)FFTWF_MALLOC(sizeof(fftwf_complex)*h*cw);

	fwdI = fftwf_plan_dft_r2c_2d(h,w,Idata, Ifft, FFTW_ESTIMATE);
	fftwf_execute(fwdI);
	fwdK = fftwf_plan_dft_r2c_2d(h,w,Kdata, Kfft, FFTW_ESTIMATE);
	fftwf_execute(fwdK);

	// now we take the .* of the images
	float scale = 1.0f / (w * h);
	const int bound = h*(w/2+1);
	for (int i=0;i<bound;i++)
	{
		IKfft[i][0] = (Ifft[i][0]*Kfft[i][0] - Ifft[i][1]*Kfft[i][1]) * scale;
		IKfft[i][1] = (Ifft[i][0]*Kfft[i][1] + Ifft[i][1]*Kfft[i][0]) * scale;
	}

	float* IKdata = (float*)FFTWF_MALLOC(sizeof(float)*w*h);
	invIK = fftwf_plan_dft_c2r_2d(h,w,IKfft,IKdata, FFTW_ESTIMATE);
	fftwf_execute(invIK);

	ScalarImage2F* IK = new ScalarImage2F(w,h);
	memcpy(IK->getRowPointer(0),IKdata,sizeof(float)*w*h);

	fftwf_destroy_plan(fwdI);
	fftwf_destroy_plan(fwdK);
	fftwf_destroy_plan(invIK);
	FFTWF_FREE(Ifft);
	FFTWF_FREE(Kfft);
	FFTWF_FREE(IKfft);
	FFTWF_FREE(Idata);
	FFTWF_FREE(Kdata);
	FFTWF_FREE(IKdata);

	return IK;
}

#endif



ScalarImage2F* normalize(const ScalarImage2F* A)
{
	ScalarImage2F* R = new ScalarImage2F(A->W(),A->H());

	float maxv=-1e20,minv=1e20;
	for (int y=0;y<R->H();y++)
	{
		const float* d = A->getPixelPointer(0,y);
		for (int x=0;x<R->W();x++)
		{
			if (*d > maxv)
				maxv = *d;
			if (*d < minv)
				minv = *d;
			d++;
		}
	}
	//printf("min and max of convolved image are %f,%f\n",minv,maxv);
	maxv = maxv - minv;

	for (int y=0;y<R->H();y++)
	{
		float* pR = R->getPixelPointer(0,y);
		const float* pA = A->getPixelPointer(0,y);
		for (int x=0;x<R->W();x++)
		{
			*pR++ = (*pA++ - minv)/maxv;
		}
	}

	return R;
}

ScalarImage2F* magnitude(const ScalarImage2F* A, const ScalarImage2F* B, 
                         bool normalize, bool doSqrt)
{
	ScalarImage2F* R = new ScalarImage2F(A->W(),A->H());

	for (int y=0;y<A->H();y++)
	{
		const float* pA = A->getPixelPointer(0,y);
		const float* pB = B->getPixelPointer(0,y);
		float* pR = R->getPixelPointer(0,y);
		for (int x=0;x<A->W();x++,pA++,pB++)
			*pR++ = *pA**pA+*pB**pB;
	}

	if(doSqrt)
	{
		for (int y=0;y<A->H();y++)
		{
			float* pR = R->getPixelPointer(0,y);
			for (int x=0;x<A->W();x++,pR++)
				*pR = sqrtf(*pR);
		}
	}

	// normalize the max magnitude to value 1
	if (normalize)
	{
		float maxv=-1e20,minv=1e20;
		for (int y=0;y<R->H();y++)
		{
			float* d = R->getPixelPointer(0,y);
			for (int x=0;x<R->W();x++)
			{
				if (*d > maxv)
					maxv = *d;
				if (*d < minv)
					minv = *d;
				d++;
			}
		}
		//printf("min and max of convolved image are %f,%f\n",minv,maxv);
		maxv = maxv - minv;

		for (int y=0;y<R->H();y++)
		{
			float* pR = R->getPixelPointer(0,y);
			for (int x=0;x<R->W();x++)
			{
				*pR = (*pR - minv)/maxv;
				pR++;
			}
		}
	}

	return R;
}







/////////////   Global But templated Image Functions

template <class T_PX>
ScalarImage2* imboolGreaterThan(_scalarImage2<T_PX>* A, _scalarImage2<T_PX>* B)
{
	ScalarImage2* N = new ScalarImage2(A->W(),B->H());
	for (int y=0;y<A->H();y++)
	{
		T_PX* pA = A->getPixelPointer(0,y);
		T_PX* pB = B->getPixelPointer(0,y);
		byte* pN = N->getPixelPointer(0,y);
		for (int x=0;x<A->W();x++,pA++,pB++,pN++)
		{
			*pN = (*pA>*pB)?255:0;
		}
	}

	return N;
}

template <class T_PX>
_scalarImage2<T_PX>* newImageOneMinus(_scalarImage2<T_PX>* I)
{
	_scalarImage2<T_PX>* N = new _scalarImage2<T_PX>(I->W(),I->H());
	for (int y=0;y<I->H();y++)
	{
		T_PX* pI = I->getRowPointer(y);
		T_PX* pN = N->getRowPointer(y);
		for (int x=0;x<I->W();x++,pI++,pN++)
		{
			*pN = (T_PX)1.0 - *pI;
		}
	}

	return N;
}
//////////// Implemented Functions Prototypes

template ScalarImage2* imboolGreaterThan<byte> (_scalarImage2<byte>* A, _scalarImage2<byte>* B);
template ScalarImage2* imboolGreaterThan<short> (_scalarImage2<short>* A, _scalarImage2<short>* B);
template ScalarImage2* imboolGreaterThan<int> (_scalarImage2<int>* A, _scalarImage2<int>* B);
template ScalarImage2* imboolGreaterThan<float> (_scalarImage2<float>* A, _scalarImage2<float>* B);

template _scalarImage2<byte>* newImageOneMinus<byte>(_scalarImage2<byte>* I);
template _scalarImage2<short>* newImageOneMinus<short>(_scalarImage2<short>* I);
template _scalarImage2<int>* newImageOneMinus<int>(_scalarImage2<int>* I);
template _scalarImage2<float>* newImageOneMinus<float>(_scalarImage2<float>* I);




//////////// Image IO Functions
e_image_file_type get_image_file_type(const char*fn)
{
	char* c;

	c = (char*)strrchr(fn, '.') ;
	if (!c)
		return UNSUPPORTED;
	c++;
	if (!strcmp(c, "png"))
		return PNG;
	else if (!strcmp(c, "ppm"))
		return PPM;
	else
		return UNSUPPORTED;
}

#define PNG_FREE_ALL_BUT_ROWS  PNG_FREE_PLTE |\
                               PNG_FREE_TRNS |\
                               PNG_FREE_HIST |\
                               PNG_FREE_ICCP |\
                               PNG_FREE_PCAL |\
                               PNG_FREE_SCAL |\
                               PNG_FREE_SPLT |\
                               PNG_FREE_TEXT |\
                               PNG_FREE_UNKN

ScalarImage2* load_image_gray(const char *fn)
{
	e_image_file_type T = get_image_file_type(fn);

	if (T == PNG)
	{
		png_byte header[8]; // 8 is the maximum size that can be checked
		png_structp png_ptr;
		png_infop info_ptr;

		/* open file and test for it being a png */
		FILE *fp = fopen(fn, "rb");
		if (!fp)
		{
			fprintf(stderr,"[read_png_file] File %s could not be opened for reading\n", fn);
			return 0x0;
		}
		fread(header, 1, 8, fp);
		if (png_sig_cmp(header, 0, 8))
		{
			fprintf(stderr,"[read_png_file] File %s is not recognized as a PNG file", fn);
			return 0x0;
		}

		/* initialize stuff */
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
		NULL);

		if (!png_ptr)
		{
			fprintf(stderr,"[read_png_file] png_create_read_struct failed");
			return 0x0;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			fprintf(stderr,"[read_png_file] png_create_info_struct failed");
			return 0x0;
		}

		if (setjmp(png_jmpbuf(png_ptr)))
		{
			fprintf(stderr,"[read_png_file] Error during init_io");
			return 0x0;
		}

		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, 8);

		png_read_info(png_ptr, info_ptr);

		int width = info_ptr->width;
		int height = info_ptr->height;

		//printf("info_ptr dimensions:  %d %d\n",width,height);

		if (info_ptr->bit_depth == 16)
			png_set_strip_16(png_ptr);
		if (info_ptr->color_type == PNG_COLOR_TYPE_RGB ||
			info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
			png_set_rgb_to_gray_fixed(png_ptr, 1, -1, -1);

		png_set_interlace_handling(png_ptr);

		// this call updates the info_ptr's members based on our calls...
		png_read_update_info(png_ptr, info_ptr);

		/* read file */
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			fprintf(stderr,"[read_png_file] Error during read_image");
			return 0x0;
		}

		int step;
		byte* buffer = imageMalloc_8u_C1(width,height,&step);

		byte** row_pointers = (byte**) malloc(sizeof(byte*) * height);
		for (int y=0; y<height; y++)
			row_pointers[y] = buffer + y*step;

		png_read_image(png_ptr, row_pointers);

		png_read_end(png_ptr,0x0);

		ScalarImage2* newImage = new ScalarImage2(width,height,step,buffer);
		// this will tell the newImage class to delete the memory
		// even though it did not allocate it.
		newImage->Manage() = true;

		png_destroy_read_struct(&png_ptr, &info_ptr, 0x0);

		free(row_pointers);

		fclose(fp);

		return newImage;
	}

	return 0x0;
}



RGBImage2* load_image_rgb(const char *fn)
{
	e_image_file_type T = get_image_file_type(fn);

	if (T == PNG)
	{
		png_byte header[8]; // 8 is the maximum size that can be checked
		png_structp png_ptr;
		png_infop info_ptr;

		/* open file and test for it being a png */
		FILE *fp = fopen(fn, "rb");
		if (!fp)
		{
			fprintf(stderr,"[read_png_file] File %s could not be opened for reading\n", fn);
			return 0x0;
		}
		fread(header, 1, 8, fp);
		if (png_sig_cmp(header, 0, 8))
		{
			fprintf(stderr,"[read_png_file] File %s is not recognized as a PNG file", fn);
			return 0x0;
		}

		/* initialize stuff */
		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
		NULL);

		if (!png_ptr)
		{
			fprintf(stderr,"[read_png_file] png_create_read_struct failed");
			return 0x0;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			fprintf(stderr,"[read_png_file] png_create_info_struct failed");
			return 0x0;
		}

		if (setjmp(png_jmpbuf(png_ptr)))
		{
			fprintf(stderr,"[read_png_file] Error during init_io");
			return 0x0;
		}

		png_init_io(png_ptr, fp);
		png_set_sig_bytes(png_ptr, 8);

		png_read_info(png_ptr, info_ptr);

		int width = info_ptr->width;
		int height = info_ptr->height;

		//printf("info_ptr dimensions:  %d %d\n",width,height);

		if (info_ptr->bit_depth == 16)
			png_set_strip_16(png_ptr);
		if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY ||
		    info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
		    png_set_gray_to_rgb(png_ptr);
		if (info_ptr->color_type & PNG_COLOR_MASK_ALPHA)
		    png_set_strip_alpha(png_ptr);

		png_set_interlace_handling(png_ptr);

		// this call updates the info_ptr's members based on our calls...
		png_read_update_info(png_ptr, info_ptr);

		/* read file */
		if (setjmp(png_jmpbuf(png_ptr)))
		{
			fprintf(stderr,"[read_png_file] Error during read_image");
			return 0x0;
		}

		RGBImage2* newImage = new RGBImage2(width,height);

		png_read_image(png_ptr, newImage->getRows());

		png_read_end(png_ptr,0x0);

		png_free_data(png_ptr, info_ptr, PNG_FREE_ALL_BUT_ROWS,-1);

		fclose(fp);

		return newImage;
	}

	return 0x0;
}



int save_image_gray(const char *fn,const ScalarImage2* image)
{
	e_image_file_type T = get_image_file_type(fn);

	if (T == PNG)
	{
		FILE *fp = fopen(fn, "wb");
		if (!fp)
		{
			fprintf(stderr,"[write_png_file] File %s could not be opened for writing", fn);
			return 0x0;
		}

		save_png_gray(fp,image);

	    fclose(fp);

	    return 0;
	}
	else
	{
		fprintf(stderr,"Writing to %s not supported\n",fn);
		return 1;
	}
}

int save_png_gray(FILE* fp,const ScalarImage2* image)
{
	png_structp  png_ptr;
	png_infop    info_ptr;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
			(png_voidp) NULL,
			(png_error_ptr) NULL,
			(png_error_ptr) NULL);
	if (!png_ptr)
		return -1;

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
		return -1;
	}

	png_init_io(png_ptr, fp);

	png_set_IHDR(png_ptr, info_ptr, image->getWidth(), image->getHeight(), 8, PNG_COLOR_TYPE_GRAY,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, info_ptr);

	png_write_image(png_ptr,image->rows);

	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	return 0;
}



int save_image_rgb(const char *fn,const RGBImage2* image)
{
	e_image_file_type T = get_image_file_type(fn);

	if (T == PNG)
	{
		FILE *fp = fopen(fn, "wb");
		if (!fp)
		{
			fprintf(stderr,"[write_png_file] File %s could not be opened for writing", fn);
			return 0x0;
		}

		save_png_rgb(fp,image);

	    fclose(fp);

	    return 0;
	}
	else
	{
		fprintf(stderr,"Writing to %s not supported\n",fn);
		return 1;
	}
}


int save_png_rgb(FILE* fp,const RGBImage2* image)
{
	png_structp  png_ptr;
	png_infop    info_ptr;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
			(png_voidp) NULL,
			(png_error_ptr) NULL,
			(png_error_ptr) NULL);
	if (!png_ptr)
		return -1;

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
		return -1;
	}

	png_init_io(png_ptr, fp);

	png_set_IHDR(png_ptr, info_ptr,
			image->getWidth(), image->getHeight(),
			8, PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
			PNG_FILTER_TYPE_DEFAULT);

	png_write_info(png_ptr, info_ptr);

	png_write_image(png_ptr,const_cast<byte**>(image->getRows()));

	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	return 0;
}


bool operator == (const ImageSize& A, const ImageSize& B)
{
	return ((A.width == B.width) && (A.height == B.height));
}


/** A colormap is a file that has 256 lines with 4 integers per line.
 * Each line is index red green blue.  The index is disregarded
 */
void
Colormap :: loadFromFile(const char* fn)
{

	FILE* f = fopen(fn,"r");

	if (!f)
	{
		fprintf(stderr,"error while opening %s for reading\n",fn);
		return;
	}

	for (int i=0;i<256;i++)
		fscanf(f,"%*d %hhd %hhd %hhd\n",map[i],map[i]+1,map[i]+2);

	fclose(f);
}


/**
 * A bytemap is a file that has K lines with 2 bytes per line.
 */
void
Bytemap :: loadFromFile(const char* fn)
{
	FILE* f = fopen(fn,"r");

	if (!f)
	{
		fprintf(stderr,"error while opening %s for reading\n",fn);
		return;
	}

	while (!feof(f))
	{
		int a,b;
		fscanf(f,"%d %d\n",&a,&b);
		(*this)[(byte)a] = (byte)b;
	}

	fclose(f);
}


//////////// Implemented Prototypes AND Type Specific Images
// must be after all of the function bodies (why?)
template class _scalarImage2<byte> ;
template class _scalarImage2<short> ;
template class _scalarImage2<int> ;
template class _scalarImage2<float> ;
template class _rgbImage2<byte> ;
template class Matrix<byte> ;

// we also specialize the MAX_PIXEL_VALUE for each instantiated template
template <> double _scalarImage2<byte>::MAX_PIXEL_VALUE = 255;
template <> double _scalarImage2<short>::MAX_PIXEL_VALUE = SHRT_MAX;
template <> double _scalarImage2<int>::MAX_PIXEL_VALUE = INT_MAX;
// template <> double _scalarImage2<float>::MAX_PIXEL_VALUE = MAXFLOAT;
template <> double _scalarImage2<float>::MAX_PIXEL_VALUE = LONG_MAX;
template <> double _rgbImage2<byte>::MAX_PIXEL_VALUE = 255;




} // namespace vpml_images

