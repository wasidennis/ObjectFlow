%% add paths
addpath(genpath('External/GMM-GMR-v2.0'));
addpath(genpath('External/maxflow'));
addpath(genpath('External/flow_code_v2'));
addpath(genpath('External/liblinear-2.01'));
addpath(genpath('External/pdollar_toolbox'));
addpath(genpath('Util'));
addpath(genpath('Codes'));

%% data paths
dirInfo.resultPath = 'Result/';
if ~exist(dirInfo.resultPath,'dir'), mkdir(dirInfo.resultPath); end;

dirInfo.dataPath = 'Data/';
if ~exist(dirInfo.dataPath,'dir'), mkdir(dirInfo.dataPath); end;

dirInfo.flowPath = [dirInfo.dataPath 'opticalFlow/'];
if ~exist(dirInfo.flowPath,'dir'), mkdir(dirInfo.flowPath); end;

dirInfo.supervoxelPath = [dirInfo.dataPath 'supervoxel/'];
if ~exist(dirInfo.supervoxelPath,'dir'), mkdir(dirInfo.supervoxelPath); end;

dirInfo.cnnPath = [dirInfo.dataPath 'cnn/'];
if ~exist(dirInfo.cnnPath,'dir'), mkdir(dirInfo.cnnPath); end;

dirInfo.caffe_folder = 'caffe-cedn-dev';

%% parameters
para.level = 15;
para.rangeS = 1;
para.rangeL = 0;
para.rangeSearch = 3.5;
para.rangeSearchEst = 3;
para.K = 5;
para.layers = [3, 6, 10, 14, 18];
para.scales = [1, 2, 4, 8, 16];

% weight for superpixels
para.colorSupervoxelWeight = 1;
para.locSupervoxelWeight = 1;
para.spatialSupervoxelWeight = 2;

para.unarySupervoxelWeight = 15;

% weight for pixels
para.CNNPixelWeight = 3;
para.colorPixelWeight = 1;
para.locPixelWeight = 2;
para.spatialPixelWeight = 3;
para.temporalPixelWeight = 0.2;
para.unaryPixelWeight = 1;

% weight between superpixels and pixels
para.supervoxel2pixelWeight = 5;
para.pixel2supervoxelWeight = 5;