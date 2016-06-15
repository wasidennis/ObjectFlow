% learn online model for colors
im = dataInfo.videoAll{ff};
colors = zeros(pixelNum,3);
for jj = 1:3
    tmp = im(:,:,jj);
    colors(:,jj) = tmp(:);
end

% get pixel
segments = pixelMap;
allIdx = unique(segments);

% determine region for fg GMM
trimap = double(gtEst{ff});
tmp = 1-trimap;
fg = imdilate(tmp,strel('diamond',para.rangeS));
fgS = 1-fg;
fgSegments = double(segments).*fgS;
fgIdx = unique(fgSegments);
fgIdxS = fgIdx(2:end);

% randon sample region for bg GMM
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

% GMM for bg
bgColors = double(colors(bgIdx,:));
bgColors_all = [onlineModel.bgColors_all;bgColors];
tmpNum = size(bgColors_all,1);
rnd = randperm(tmpNum);
% num = min(round(size(onlineModel.bgColors_init,1)/2),tmpNum);
% bgColors = [onlineModel.bgColors_init;bgColors_all(rnd(1:num),:)];

num = min(round(size(onlineModel.bgColors_init,1)/2),tmpNum);
bgColors_all = bgColors_all(rnd(1:num),:);
bgColors = [onlineModel.bgColors_init;bgColors_all];

[Priors, Mu, Sigma] = EM_init_kmeans(bgColors', K);
[bgPixModel.Priors, bgPixModel.Mu, bgPixModel.Sigma, ~] = EM(bgColors', Priors, Mu, Sigma);
