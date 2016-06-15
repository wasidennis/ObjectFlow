% % learn online model for CNN features
segments = pixelMap;
allIdx = unique(segments);

% determine region for fg SVM
trimap = double(gtEst{ff});
tmp = 1-trimap;
fg = imdilate(tmp,strel('diamond',para.rangeS));
fgS = 1-fg;
fgSegments = double(segments).*fgS;
fgIdx = unique(fgSegments);
fgIdxS = fgIdx(2:end);

% randon sample region for bg SVM
fgL = imdilate(trimap,strel('diamond',para.rangeS));
fgSegments = double(segments).*fgL;
fgIdx = unique(fgSegments);
fgIdxL = fgIdx(2:end);

keepDoing = 1;
tmp = trimap;
while keepDoing
    imLarge = imdilate(tmp,strel('diamond',1));
    if sum(imLarge(:))>=sum(trimap(:))*para.rangeSearch || sum(imLarge(:))==wd*ht
        keepDoing = 0;
    end
    tmp = imLarge;
end
bg = imLarge;

bgL = 1-bg;
fgSegments = double(segments).*bgL;
bgIdx = unique(fgSegments);
bgIdxL = bgIdx(2:end);
allIdx([bgIdxL;fgIdxL]) = [];
bgIdx = allIdx;

% learn SVM
feats_fg = [];
feats_bg = [];
for j = 1:length(cnnFeats{1})
    % fg
    feats_region = cnnFeats{1}{j}(:,fgIdxS);
    feats_region = bsxfun(@times, feats_region, 1./(sqrt(sum(feats_region.^2,1))+eps));
    feats_fg = [feats_fg;feats_region];
    
    % bg
    feats_region = cnnFeats{1}{j}(:,bgIdx);
    feats_region = bsxfun(@times, feats_region, 1./(sqrt(sum(feats_region.^2,1))+eps));
    feats_bg = [feats_bg;feats_region];
end

% random sample features
rnd = randperm(size(feats_fg,2));
num = min(round(size(onlineModel.feats_fg_init,2)),size(feats_fg,2));
feats_fg = feats_fg(:,rnd(1:num));

rnd = randperm(size(feats_bg,2));
num = min(round(size(onlineModel.feats_bg_init,2)),size(feats_bg,2));
feats_bg = feats_bg(:,rnd(1:num));

% learn SVM model
trainLabel = [ones(size(onlineModel.feats_fg_init,2)+size(feats_fg,2),1);zeros(size(feats_bg,2),1)];
trainData = [onlineModel.feats_fg_init';feats_fg';feats_bg'];
cnnModel = train(trainLabel, sparse(double(trainData)), '-s 0 -B 1 -q');

% [~, ~, prob] = predict(zeros(size(trainData,1),1), sparse(double(trainData)), initModel, '-b 1');
