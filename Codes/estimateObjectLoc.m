% estimate the object location in the next frame
function onlineModel = estimateObjectLoc(onlineModel, para)
onlineModel.nFrames = 2; onlineModel.imBoxes{1} = onlineModel.gtEst{onlineModel.ff};
    
video = onlineModel.video; flows = onlineModel.flows; supervoxels = onlineModel.supervoxels;
ht = onlineModel.ht; wd = onlineModel.wd;
ff = onlineModel.ff;
K = para.K;
pixelNum = ht*wd;
pixelMap = reshape((1:pixelNum),[ht,wd]);
imBoxes = onlineModel.imBoxes;

fgInitModel = onlineModel.fgInitModel;
bgInitModel = onlineModel.bgInitModel;
%% propagate optical flow to the next frame
propagateFlows;

%% enlarge the segmentation
im1 = video{1}; im2 = video{2};
obj1 = imBoxes{1};

% plus average flow
if ff>1 && sum(obj1(:))==0
    obj2 = imBoxes{2};
else
    [row,col] = find(obj1);
    row = row + round(mean(meanFlows(1,:)));
    col = col + round(mean(meanFlows(2,:)));
    % remove flow ouside of image
    tmp = (col>=1 & col<=wd) & (row>=1 & row<=ht);
    col = col(tmp); row = row(tmp);
    ind = sub2ind([ht,wd],row,col);
    
    obj2 = zeros(size(obj1));
    obj2(ind) = 1;
end

keepDoing = 1;
tmp = obj2;
while keepDoing
    imLarge = imdilate(tmp,strel('diamond',1));
    if sum(imLarge(:))>=sum(obj2(:))*para.rangeSearchEst || sum(imLarge(:))==wd*ht
        keepDoing = 0;
    end
    tmp = imLarge;
end

%% pre-processing
% compute RGB colors
colors = zeros(pixelNum,3);
for jj = 1:3
    tmp = im2(:,:,jj);
    colors(:,jj) = tmp(:);
end

% get pixel
segments = pixelMap;

% determine region of interest for segmentation
fgSegments = double(segments).*imLarge;
fgIdx = unique(fgSegments);
fgIdxL = fgIdx(2:end);

%% use the color model to filter out pixels
% Unary potentials for color
fgColorsL = double(colors(fgIdxL,:));
[~,numData] = size(fgColorsL');

fgPxi = zeros(numData,K);
bgPxi = zeros(numData,K);
for jj = 1:K
    fgPxi(:,jj) = gaussPDF(fgColorsL', fgInitModel.Mu(:,jj), fgInitModel.Sigma(:,:,jj));
    bgPxi(:,jj) = gaussPDF(fgColorsL', bgInitModel.Mu(:,jj), bgInitModel.Sigma(:,:,jj));
end
fgPix_tmp = repmat(fgInitModel.Priors,[numData 1]).*fgPxi;
bgPix_tmp = repmat(bgInitModel.Priors,[numData 1]).*bgPxi;

fgPix = fgPix_tmp ./ repmat(sum([sum(fgPix_tmp,2),sum(bgPix_tmp,2)],2),[1 K]);
bgPix = bgPix_tmp ./ repmat(sum([sum(fgPix_tmp,2),sum(bgPix_tmp,2)],2),[1 K]);

fgPro = sum(fgPix,2);
bgPro = sum(bgPix,2);

%% use the location based on flow to filter out pixels
range = 1.5;
keepDoing = 1;
tmp = obj2;
while keepDoing
    imSmall = 1-tmp;
    imSmall = imdilate(imSmall,strel('diamond',1));
    imSmall = 1-imSmall;
    if sum(imSmall(:))*range<sum(obj2(:)) || sum(imSmall(:))==0
        keepDoing = 0;
        imSmall = tmp;
    end
    tmp = imSmall;
end

% build shape prior (distance transform)
dist = double(bwdist(imSmall));
dist = dist.*imLarge; dist(imLarge==0) = max(dist(:));

pro = 1-dist/(max(dist(:)));
fgLocPro = pro(fgIdxL);

%% build the possible object mask
if isempty(fgPro) || isempty(fgLocPro)
    imBoxes{2} = imLarge;
else
    fgIdxAll = fgIdxL((fgPro+fgLocPro)/2>=0.5);  
    mask = zeros(size(imLarge));
    mask(fgIdxAll) = 1;
    imBoxes{2} = logical(mask);
end
onlineModel.imBoxes{2} = imBoxes{2};
onlineModel.meanFlows = meanFlows;