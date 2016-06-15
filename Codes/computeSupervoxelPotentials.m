% compute unary potentials
unarySupervoxelPotentials = [];
fgProSupervoxel = cell(nFrames,1);
for ii = 1:nFrames
    % Unary potentials for color
    fgColorsL = double(colors(fgIdxL{ii},:));
    [~,numData] = size(fgColorsL');
    
    fgPxi = zeros(numData,K);
    bgPxi = zeros(numData,K);
    for jj = 1:K
        fgPxi(:,jj) = gaussPDF(fgColorsL', fgPixModel.Mu(:,jj), fgPixModel.Sigma(:,:,jj));
        bgPxi(:,jj) = gaussPDF(fgColorsL', bgPixModel.Mu(:,jj), bgPixModel.Sigma(:,:,jj));
    end
    fgPix_tmp = repmat(fgPixModel.Priors,[numData 1]).*fgPxi;
    bgPix_tmp = repmat(bgPixModel.Priors,[numData 1]).*bgPxi;
    
    fgPix = fgPix_tmp ./ repmat(sum([sum(fgPix_tmp,2),sum(bgPix_tmp,2)],2),[1 K]);
    bgPix = bgPix_tmp ./ repmat(sum([sum(fgPix_tmp,2),sum(bgPix_tmp,2)],2),[1 K]);
    
    fgPro = max(fgPix,[],2);
    bgPro = max(bgPix,[],2);
    
    % Unary potentials for location
    if ii==1
        fgLocPro = flowRatio(fgIdxL{ii}).*objRatio(fgIdxL{ii});
    else
        fgLocPro = flowRatioInv(fgIdxL{ii}-superpixelsNum(1)).*objRatioInv(fgIdxL{ii}-superpixelsNum(1));
    end
    fgLocPro(fgLocPro==0) = 10e-5; fgLocPro(fgLocPro==1) = 1-10e-5;
    bgLocPro = 1-fgLocPro; 
    
    % Unary potentials
    tmp = [-log(fgPro)*para.colorSupervoxelWeight-log(fgLocPro)*para.locSupervoxelWeight -log(bgPro)*para.colorSupervoxelWeight-log(bgLocPro)*para.locSupervoxelWeight];
    unarySupervoxelPotentials = [unarySupervoxelPotentials;tmp];
    fgProSupervoxel{ii} = fgPro;
end

% spatial pairwise potentials
[sSource, sDestination] = getSpatialConnections(subSuperpixels, subSuperpixelsNum(end)+1);
sDestination(sSource==0) = [];
sSource(sSource==0) = [];
sSource = sSource-1; sDestination = sDestination-1;

sSqrColourDistance = sum((subColors(sSource+1,:) - subColors(sDestination+1,:)).^ 2,2);
sCentreDistance = sqrt(sum((subCenters(sSource+1,:) - subCenters(sDestination+1,:)).^ 2,2));

sBeta = 0.5/(mean( sSqrColourDistance ./ (sCentreDistance+10e-5) ) + 10e-5);
sSuperpixelValue = exp(-sBeta*sSqrColourDistance)./sCentreDistance;

pairSupervoxelPotentials.source = sSource;
pairSupervoxelPotentials.destination = sDestination;
pairSupervoxelPotentials.value = single(sSuperpixelValue*para.spatialSupervoxelWeight);