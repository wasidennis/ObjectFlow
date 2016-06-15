% learn initial model for colors
pixelNum = ht*wd;
pixelMap = reshape((1:pixelNum),[ht,wd]);

im = dataInfo.videoAll{1};
colors = zeros(pixelNum,3);
for jj = 1:3
    tmp = im(:,:,jj);
    colors(:,jj) = tmp(:);
end

% get pixel
segments = pixelMap;
allIdx = unique(segments);

% determine region for fg GMM
trimap = double(gtEst{1});
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

% GMM for fg and bg
fgColorsS = double(colors(fgIdxS,:));
bgColors = double(colors(bgIdx,:));

[Priors, Mu, Sigma] = EM_init_kmeans(fgColorsS', para.K);
[fgPixModel.Priors, fgPixModel.Mu, fgPixModel.Sigma, ~] = EM(fgColorsS', Priors, Mu, Sigma);

[Priors, Mu, Sigma] = EM_init_kmeans(bgColors', para.K);
[bgPixModel.Priors, bgPixModel.Mu, bgPixModel.Sigma, ~] = EM(bgColors', Priors, Mu, Sigma);
