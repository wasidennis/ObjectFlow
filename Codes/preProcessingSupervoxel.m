%% convert supervoxel to multiple superpixels
superpixels = cell(nFrames,1);
numSuperpixels = zeros(nFrames,1);
for ii = 1:nFrames
    segments = double(supervoxels{ii});
    fgIdx = unique(segments(:)); fgIdx(fgIdx==0) = [];
    
    maskAll = zeros(ht,wd);
    for jj = 1:length(fgIdx)
        tmp = zeros(ht,wd);
        tmp(segments==fgIdx(jj)) = 1;
        [mask num] = bwlabel(tmp,8);
        
        mask = mask + numSuperpixels(ii);
        numSuperpixels(ii) = numSuperpixels(ii)+num;
        
        maskAll(segments==fgIdx(jj)) = mask(segments==fgIdx(jj));
    end
    superpixels{ii} = uint32(maskAll);
end

%% pre-processing data across frames
% build unique ids for superpixels across frames
superpixelsUnique = superpixels;
superpixelsNum = zeros(nFrames,1);
superpixelsNum(1) = max(superpixels{1}(:));
for ii = 2:nFrames
    superpixelsNum(ii) = max(superpixels{ii}(:)) + superpixelsNum(ii-1);
    superpixelsUnique{ii} = superpixelsUnique{ii} + superpixelsNum(ii-1);    
end

% compute mean RGB colors for the video
[colors, centers, sizes] = getSuperpixelStats(video, superpixelsUnique, superpixelsNum(end));

fgL = cell(nFrames,1);
fgIdxL = cell(nFrames,1);
subColors = [];
subCenters = [];
for ii = 1:nFrames
    % get superpixel
    segments = superpixelsUnique{ii};

    % determine region of interest for segmentation
    trimap = double(imBoxes{ii});
    fgL{ii} = imdilate(trimap,strel('diamond',para.rangeL));
    fgSegments = double(segments).*fgL{ii};
    fgIdx = unique(fgSegments);
    fgIdxL{ii} = fgIdx(2:end);
    
    % colors for region of interest
    subColors = [subColors;colors(fgIdxL{ii},:)];
    subCenters = [subCenters;centers(fgIdxL{ii},:)];
end

%% re-build sub graph
subSuperpixels = cell(nFrames,1);
subSuperpixelsNum = zeros(nFrames,1);

idx = fgIdxL{1};
subSuperpixelsNum(1) = length(idx);
segments = superpixelsUnique{1};
segmentsTmp = zeros(size(segments));
for jj = 1:length(idx)
    segmentsTmp(segments==idx(jj)) = jj;
end
subSuperpixels{1} = uint32(segmentsTmp+1);

for ii = 2:nFrames
    idx = fgIdxL{ii};
    subSuperpixelsNum(ii) = length(idx) + subSuperpixelsNum(ii-1);
    segments = superpixelsUnique{ii};
    segmentsTmp = zeros(size(segments));
    for jj = 1:length(idx)
        segmentsTmp(segments==idx(jj)) = jj + subSuperpixelsNum(ii-1);
    end
    subSuperpixels{ii} = uint32(segmentsTmp+1);
end
subSuperpixelsNum = [0;subSuperpixelsNum];

%% compute ratio of superpixel flows
% for the current frame
xFlow = flows{1}(:,:,2); yFlow = flows{1}(:,:,1);
xFlow = double(xFlow(:)); yFlow = double(yFlow(:));

flowRatio = zeros(numSuperpixels(1),1);
objRatio = zeros(numSuperpixels(1),1);
segments = double(supervoxels{1}).*imBoxes{1};
for ii = 1:numSuperpixels(1)
    ind = find(superpixels{1}==ii);
    [row,col] = ind2sub([ht,wd],ind);
    
    colNew = col + round(xFlow(ind)); rowNew = row + round(yFlow(ind));
    colNew(colNew<1) = 1; rowNew(rowNew<1) = 1;
    colNew(colNew>wd) = wd; rowNew(rowNew>ht) = ht;
    indNew = sub2ind([ht,wd],rowNew,colNew);
    
    flowRatio(ii) = sum(supervoxels{2}(indNew) == supervoxels{1}(ind)) / length(supervoxels{1}(ind));
    objRatio(ii) = sum(segments(ind)>0) / length(ind);
end

% for the next frame
xFlow = flowsInv{1}(:,:,2); yFlow = flowsInv{1}(:,:,1);
xFlow = double(xFlow(:)); yFlow = double(yFlow(:));

flowRatioInv = zeros(numSuperpixels(2),1);
objRatioInv = zeros(numSuperpixels(2),1);
segments = double(supervoxels{1}).*imBoxes{1};
for ii = 1:numSuperpixels(2)
    indNew = find(superpixels{2}==ii);
    [rowNew,colNew] = ind2sub([ht,wd],indNew);
    
    col = colNew + round(xFlow(indNew)); row = rowNew + round(yFlow(indNew));
    col(col<1) = 1; row(row<1) = 1;
    col(col>wd) = wd; row(row>ht) = ht;
    ind = sub2ind([ht,wd],row,col);
    
    flowRatioInv(ii) = sum(supervoxels{2}(indNew) == supervoxels{1}(ind)) / length(supervoxels{2}(indNew));
    objRatioInv(ii) = sum(segments(ind)>0) / length(ind);
end

