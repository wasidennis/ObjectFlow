function onlineModel = objectSegmentTracking(onlineModel, para)
video = onlineModel.video; flows = onlineModel.flows; flowsInv = onlineModel.flowsInv;
supervoxels = onlineModel.supervoxels;
ht = onlineModel.ht; wd = onlineModel.wd;
ff = onlineModel.ff;
K = para.K;
pixelNum = ht*wd;
pixelMap = reshape((1:pixelNum),[ht,wd]);

nFrames = onlineModel.nFrames; imBoxes = onlineModel.imBoxes; gtEst = onlineModel.gtEst;
cnnFeats = onlineModel.cnnFeats; meanFlows = onlineModel.meanFlows;
%% potentials for supervoxel level
preProcessingSupervoxel;

fgPixModel = onlineModel.fgInitModel;
bgPixModel = onlineModel.bgInitModel;
computeSupervoxelPotentials;

%% potentials for pixel level
preProcessingPixel;

cnnModel = onlineModel.cnnModel;
computePixelPotentials;

%% potentials between supervoxels and pixels
computePairwisePotentials;

%% solve by graph cut
unaryPotentials = [unarySupervoxelPotentials*para.unarySupervoxelWeight; unaryPixelPotentials*para.unaryPixelWeight];

pairPotentials.source = [pairSupervoxelPotentials.source; pairPixelPotentials.source + numSuperpixelNode; relationPotentials.source];
pairPotentials.destination = [pairSupervoxelPotentials.destination; pairPixelPotentials.destination + numSuperpixelNode; relationPotentials.destination];
pairPotentials.value = [repmat(pairSupervoxelPotentials.value,1,2); repmat(pairPixelPotentials.value,1,2); relationPotentials.value];

[~,labels] = maxflow_mex_optimisedWrapper(pairPotentials, single(unaryPotentials));
onlineModel.supervoxelLabels = labels(1:numSuperpixelNode);
onlineModel.pixelLabels = labels(numSuperpixelNode+1:end);

%% pixel level labels
segments = subPixels{2}-1;
idx = subPixelsNum(2)+1:subPixelsNum(3);

mask = zeros(size(segments));
mask(fgIdxL{2}-pixelNum) = onlineModel.pixelLabels(idx);

% assign the mask for the next frame
gtEst{ff+1} = mask;

% check estimated mask
tmp = 1-double(gtEst{ff+1});
tmp = 1 - imdilate(tmp,strel('diamond',para.rangeS));
if sum(tmp(:)) == 0
    gtEst{ff+1} = imBoxes{2};
    mask = gtEst{ff+1};
end
onlineModel.gtEst = gtEst;
onlineModel.mask = mask;