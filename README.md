# ObjectFlow
Project webpage: https://sites.google.com/site/yihsuantsai/research/cvpr16-segmentation

Contact: Yi-Hsuan Tsai (ytsai2 at ucmerced dot edu)

## Paper
Object Segmentation via Object Flow

Yi-Hsuan Tsai, Ming-Hsuan Yang and Michael J. Black

IEEE Conference on Computer Vision and Pattern Recognition (CVPR), 2016.

## Overview
* This is the authors' MATLAB implementation described in the above paper. Please cite our paper if you use our code and data for your research.

* This code has been tested on Ubuntu 14.04 and MATLAB 2013b.

## Installation
* Download and unzip the code.

* Install the included caffe branch as instructed at http://caffe.berkeleyvision.org/installation.html

* Install included libraries in the **External** folder if needed (pre-compiled codes are included).

* Download the CNN model at

## Usage
1) put your video data in the **Videos** folder (see examples in this folder)

2) set directories and parameters in **setup_all.m** (suggest to use defaults)

3) run **demo_objectFlow.m** and change some settings based on the data (see comments in the code)

## Note
* This package only contains the code of object segment tracking (without re-estimating optical flow),
and the performacne is a bit worse than the one reported in the paper.

* Currently we use the ground truth of the first frame and propagate to following frames.
If you would like to use other initializations, please replace the ground truth data.

* The current code for generating optical flow is slow, so you can replace it with any other optical flow method to speed up the process.
