%% compute mean RGB colors for the video
colors = [];
for ii = 1:nFrames
    im = video{ii};
    color = zeros(pixelNum,3);
    for jj = 1:3
        tmp = im(:,:,jj);
        color(:,jj) = tmp(:);
    end
    colors = [colors;color];
end

centers = zeros(pixelNum,2);
[xx,yy] = meshgrid(1:wd,1:ht);
centers(:,1) = xx(:);
centers(:,2) = yy(:);
centers = repmat(centers, [nFrames 1]);

%% pre-processing data across frames
% compute CNN features for pixels
[xx,yy] = meshgrid(1:512,1:512);
xx = xx(:); yy = yy(:);

fgL = cell(nFrames,1);
fgIdxL = cell(nFrames,1);
subFeats = cell(nFrames,1);
for nn = 1:nFrames
    % get pixel
    segments = pixelMap + pixelNum*(nn-1);
    
    % determine region of interest for segmentation
    trimap = double(imBoxes{nn});
    fgL{nn} = imdilate(trimap,strel('diamond',para.rangeL));
    fgSegments = double(segments).*fgL{nn};
    fgIdx = unique(fgSegments);
    fgIdxL{nn} = fgIdx(2:end);
    
    % CNN features for region of interest
    feats_fg = [];
    for j = 1:length(cnnFeats{nn})
        act = cnnFeats{nn}{j};

        feats_region = act(:,fgIdxL{nn}-pixelNum*(nn-1));
        feats_region = bsxfun(@times, feats_region, 1./(sqrt(sum(feats_region.^2,1))+eps));
        feats_fg = [feats_fg;feats_region];
    end
    subFeats{nn} = feats_fg';
end

% compute RGB colors for the video
subColors = [];
subCenters = [];
for ii = 1:nFrames 
    % colors for region of interest
    subColors = [subColors;colors(fgIdxL{ii},:)];
    subCenters = [subCenters;centers(fgIdxL{ii},:)];
end

%% build location priors by shape
obj1 = imBoxes{1};

% enlarge regions
imLarges = cell(nFrames,1);
range = 2;
keepDoing = 1;
tmp = obj1;
while keepDoing
    imLarges{1} = imdilate(tmp,strel('diamond',1));
    if sum(imLarges{1}(:))>=sum(obj1(:))*range || sum(imLarges{1}(:))==wd*ht
        keepDoing = 0;
    end
    tmp = imLarges{1};
end

% shrink regions
imSmalls = cell(nFrames,1);
range = 1.5;
keepDoing = 1;
tmp = obj1;
while keepDoing
    imSmalls{1} = 1-tmp;
    imSmalls{1} = imdilate(imSmalls{1},strel('diamond',1));
    imSmalls{1} = 1-imSmalls{1};
    if sum(imSmalls{1}(:))*range<sum(obj1(:)) || sum(imSmalls{1}(:))==0
        keepDoing = 0;
        imSmalls{1} = tmp;
    end
    tmp = imSmalls{1};
end

% plus average flow
if sum(obj1(:))==0
    imLarges{2} = imLarges{1};
    imSmalls{2} = imSmalls{1};
else
    [row,col] = find(imLarges{1});
    row = row + round(mean(meanFlows(1,:)));
    col = col + round(mean(meanFlows(2,:)));
    % remove flow ouside of image
    tmp = (col>=1 & col<=wd) & (row>=1 & row<=ht);
    col = col(tmp); row = row(tmp);
    ind = sub2ind([ht,wd],row,col);
    
    obj2 = zeros(size(obj1));
    obj2(ind) = 1;
    imLarges{2} = obj2;
    
    [row,col] = find(imSmalls{1});
    row = row + round(mean(meanFlows(1,:)));
    col = col + round(mean(meanFlows(2,:)));
    % remove flow ouside of image
    tmp = (col>=1 & col<=wd) & (row>=1 & row<=ht);
    col = col(tmp); row = row(tmp);
    ind = sub2ind([ht,wd],row,col);
    
    obj2 = zeros(size(obj1));
    obj2(ind) = 1;
    imSmalls{2} = obj2;
end

%% re-build sub graph
subPixels = cell(nFrames,1);
subPixelsNum = zeros(nFrames+1,1);

for ii = 1:nFrames
    idx = fgIdxL{ii};
    subPixelsNum(ii+1) = length(idx) + subPixelsNum(ii);
    
    segmentsTmp = zeros(size(segments));
    segmentsTmp(idx-pixelNum*(ii-1)) = (1:length(idx))+subPixelsNum(ii);
    subPixels{ii} = uint32(segmentsTmp+1);
end


