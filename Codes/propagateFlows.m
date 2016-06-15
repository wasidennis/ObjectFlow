% propagate the flow to the next frame
im1 = video{1}; im2 = video{2};
ind = find(imBoxes{1}(:)==1);

% optical flow
xFlow = flows{1}(:,:,2); yFlow = flows{1}(:,:,1);
xFlow = double(xFlow(:)); yFlow = double(yFlow(:));

% superpixels inside the mask
segments = double(supervoxels{1});
mask = segments.*imBoxes{1};
idxSP = unique(mask(:));
idxSP = idxSP(2:end);

% compute the average flow of each superpixel
colNew = []; rowNew = [];
meanFlows = [];
for jj = 1:length(idxSP);
    tmp = find(mask(:)==idxSP(jj));
    indSP = ismember(tmp,ind);
    
    if sum(indSP)/length(indSP) < 0.5, continue; end;
    indSP = tmp(indSP);
    meanFlow = zeros(2,1);
    meanFlow(1) = mean(yFlow(indSP)); meanFlow(2) = mean(xFlow(indSP));
    
    [row,col] = ind2sub([ht,wd],indSP);
    colNew = [colNew;round(col+meanFlow(2))]; rowNew = [rowNew;round(row+meanFlow(1))];
    meanFlows = [meanFlows meanFlow];
end

% remove flow ouside of image
tmp = (colNew>=1 & colNew<=wd) & (rowNew>=1 & rowNew<=ht);
colNew = colNew(tmp); rowNew = rowNew(tmp);

% current segmentation based on flow
mask = zeros(ht,wd); mask = mask(:);
ind = sub2ind([ht,wd],rowNew,colNew);
mask(ind) = 1; mask = reshape(mask,[ht,wd]);
flowMask = mask;
