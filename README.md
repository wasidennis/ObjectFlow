# ObjectFlow
Project webpage: https://sites.google.com/site/yihsuantsai/research/cvpr16-segmentation <br />
Contact: Yi-Hsuan Tsai (ytsai2 at ucmerced dot edu)

## Paper
Video Segmentation via Object Flow <br />
Yi-Hsuan Tsai, Ming-Hsuan Yang and Michael J. Black <br />
IEEE Conference on Computer Vision and Pattern Recognition (CVPR), 2016.

## Overview
* This is the authors' MATLAB implementation described in the above paper. Please cite our paper if you use our code and model for your research.

* This code has been tested on Ubuntu 14.04 and MATLAB 2013b.

## Installation
* Download and unzip the code.

* Install the attached caffe branch, as instructed at http://caffe.berkeleyvision.org/installation.html.

* Download the CNN model for feature extraction at https://dl.dropboxusercontent.com/u/73240677/CVPR16/pascal_segmentation.zip, then unzip the model folder under the **caffe-cedn-dev/examples** folder.

* Install included libraries in the **External** folder if needed (pre-compiled codes are already included).

## Usage
* Put your video data in the **Videos** folder (see examples in this folder).

* Set directories and parameters in `setup_all.m` (suggest to use defaults).

* Run `demo_objectFlow.m` and change settings if needed based on your video data (see the script for further details).

## Note
* Currently this package only contains the implementation of object segment tracking without re-estimating optical flow and the performacne is a bit worse than the one reported in the paper.

* For initialization, currently we use the ground truth of the first frame and propagate to following frames. If you prefer to use other initializations, please replace the ground truth data.

## Hint

* The current implementation for generating optical flow is slow, so you can replace it with other optical flow methods to speed up the process.

## Log

* 06/2016: code released
* 09/2016: evaluation method updated
