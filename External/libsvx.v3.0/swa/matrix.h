/**
 * matrix.h
 * jcorso
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
 *
 * Defines matrix and vector classes.  Defined as template classes
 *
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include <assert.h>

namespace vpml_matrix
{

//  some local defines that make some code more simple
//#define PTR_ADDBYTES(ptr,bytes) (TYPE*)(((unsigned char*)ptr) + bytes)


template <class TYPE>
class Matrix
{
public:
	int numRows,numCols,step; // step is the number of bytes in a row (could be
	                          //  more than sizeof(TYPE)*numCols.  If so,
	                          //  the extra is disregarded.
	TYPE* data;
	TYPE** rows;

	Matrix()
	: numRows(-1),numCols(-1),data(0x0),rows(0x0)
	{}

	virtual ~Matrix()
	{
		if (data) free(data); data = 0x0;
		if (rows) free(rows); rows = 0x0;
	}


	void  fillWithValue(TYPE value)
	{
		for (int y=0;y<numRows;y++)
		{
			TYPE* px = PTR_ADDBYTES(data,y*step);
			for (int x=0;x<numCols;x++)
				*px++ = value;
		}
	}

	TYPE** getRows()
	{
		return rows;
	}

	TYPE** const getRows() const
	{
		return rows;
	}

	TYPE*  getRowPointer(int r)
	{
		assert(r<numRows);
		return rows[r];
	}

	const TYPE*  getRowPointer(int r) const
	{
		assert(r<numRows);
		return rows[r];
	}

	inline TYPE  getValue(const int row, const int col) const
	{
		return rows[row][col];
	}

	inline TYPE& getElement (int row, int col)
	{
		return rows[row][col];
	}

	inline TYPE& getElement (int index)
	// be careful of the "step" when using this function...
	{
		return data[index];
	}


	/**
	 * Changes the size of the matrix.  Existing data in the matrix is gone.
	 */
	void  setDimension(int r, int c)
	{
		numRows = r;
		numCols = c;
		step = numCols*sizeof(TYPE);

		if (data) free(data);
		data = (TYPE*)malloc(sizeof(TYPE)*r*numCols);

		if (rows) free(rows);
		rows = (TYPE**)malloc(sizeof(TYPE*)*numRows);
		for (int i=0; i<numRows; i++)
			rows[i] = PTR_ADDBYTES(data,i*step);
	}


	inline void  setValue(const int row, const int col, const TYPE v)
	{
		rows[row][col] = v;
	}


	/**
	 * This is a matrix element accessor (sugar)
	 */
	TYPE& operator () (int row, int col)
	{
		return getElement(row,col);
	}

private:

	inline TYPE* PTR_ADDBYTES(TYPE* ptr, size_t bytes)
	{
		return (TYPE*)(((unsigned char*)ptr) + bytes);
	}
	inline TYPE* PTR_ADDBYTES(TYPE* ptr, size_t bytes) const
	{
		return (TYPE*)(((unsigned char*)ptr) + bytes);
	}

};




} // end of namespace vpml_matrix



#endif /*MATRIX_H_*/
