/** A wrapper function for the maxflow-v3.0 algorithm by Michael Rubinstein
% 
% Copyright (c) 2013, Anestis Papazoglou
% All rights reserved.
% 
% Redistribution and use in source and binary forms, with or without
% modification, are permitted provided that the following conditions are
% met: 
% 
% 1. Redistributions of source code must retain the above copyright notice,
%    this list of conditions and the following disclaimer. 
% 2. Redistributions in binary form must reproduce the above copyright
%    notice, this list of conditions and the following disclaimer in the
%    documentation and/or other materials provided with the distribution. 
% 
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
% "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
% TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
% PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
% OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
% EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
% PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
% PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
% LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
% NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
% SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "mex.h"
#include "maxflow-v3.0/graph.h"

#define USAGE_NOTIFICATION "Error: incorrect function call\n\n" \
	"USAGE:\t[ flow, labels ] = maxflowmex( A, T )\n\n" \
	"\tA: a structure with 3 fields:\n" \
	"\t\tA.source: a Nx1 uint32 real valued array denoting the source node of an edge.\n" \
	"\t\tA.destination: a Nx1 uint32 real valued array denoting the destination node of an edge\n" \
	"\t\tA.potential: a Nx2 single real valued array denoting the pairwise potential of an edge\n" \
	"\tT: a Mx2 single real valued array denoting the unary potential of a node\n"

void mexFunction(int nlhs,	/* number of expected outputs */
	mxArray *plhs[],					/* mxArray output pointer array */
	int nrhs, 								/* number of inputs */
	const mxArray *prhs[]			/* mxArray input pointer array */)
{
	unsigned int nodes;
	unsigned int pairs;
	bool *labels;
	unsigned int *source;
	unsigned int *destination;
	float *pairValue;
	float *unaryValue;
	float *flow;
	mxArray *sourceMxArray;
	mxArray *destinationMxArray;
	mxArray *pairValueMxArray;

	/** Input validation */
	if( nrhs != 2 )
		mexErrMsgTxt( USAGE_NOTIFICATION );
	
	const mxArray *A = prhs[ 0 ];
	const mxArray *T = prhs[ 1 ];
	
	if( !mxIsStruct( A ) )
	{
		mexErrMsgTxt( USAGE_NOTIFICATION );
	}
	else
	{
		sourceMxArray = mxGetField( A, 0, "source" );
		destinationMxArray = mxGetField( A, 0, "destination" );
		pairValueMxArray = mxGetField( A, 0, "value" );
		
		if( mxGetClassID( sourceMxArray ) != mxUINT32_CLASS ||
			mxGetClassID( destinationMxArray ) != mxUINT32_CLASS ||
			mxGetClassID( pairValueMxArray ) != mxSINGLE_CLASS )
		{
			mexErrMsgTxt( USAGE_NOTIFICATION );
		}
		
		if( mxIsComplex( sourceMxArray ) ||
			mxIsComplex( destinationMxArray ) ||
			mxIsComplex( pairValueMxArray ) )
		{
			mexErrMsgTxt( USAGE_NOTIFICATION );
		}
		
		pairs = mxGetNumberOfElements( sourceMxArray );
		if( pairs != mxGetNumberOfElements( destinationMxArray ) ||
			mxGetN( pairValueMxArray ) != 2 )
		{
			mexErrMsgTxt( USAGE_NOTIFICATION );
		}
	}
	
	if( mxGetClassID( T ) != mxSINGLE_CLASS )
	{
		mexErrMsgTxt( USAGE_NOTIFICATION );
	}
	else
	{
		if( mxIsComplex( T ) )
			mexErrMsgTxt( USAGE_NOTIFICATION );
	
		nodes = mxGetM( T );

		if( mxGetN( T ) != 2 )
			mexErrMsgTxt( USAGE_NOTIFICATION );
	}
	/** End of input validation */
	
	source = ( unsigned int * )mxGetData( sourceMxArray );
	destination = ( unsigned int * )mxGetData( destinationMxArray );
	pairValue = ( float * )mxGetData( pairValueMxArray );
	unaryValue = ( float * )mxGetData( T );
	
	typedef Graph< float, float, float > GraphType;
	GraphType *graph = new GraphType( nodes, pairs );
	graph->add_node( nodes );
	
	/** Add pairwise potentials */
	for( int edge = 0; edge < pairs; edge++ )
		graph->add_edge( source[ edge ], destination[ edge ], pairValue[ edge ], pairValue[ edge + pairs ] );
	
	for( int node = 0; node < nodes; node++ )
		graph->add_tweights( node, unaryValue[ node ], unaryValue[ node + nodes ] );
	
	/** Create outputs */
	plhs[ 0 ] = mxCreateNumericMatrix( 1, 1, mxSINGLE_CLASS, mxREAL );
	plhs[ 1 ] = mxCreateLogicalMatrix( nodes, 1 );
	
	flow = ( float * )mxGetData( plhs[ 0 ] );
	labels = ( bool * )mxGetData( plhs[ 1 ] );
	
	*flow = graph->maxflow();
	
	for( int node = 0; node < nodes; node++ )
		labels[ node ] = graph->what_segment( node );
	
	delete graph;
}

