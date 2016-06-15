% compute unary potentials
unaryPixelPotentials = [];
fgProPixel = cell(nFrames,1);
for ii = 1:nFrames
    % Unary potentials for CNN features  
    testData = subFeats{ii};
    [~, ~, prob] = predict(zeros(size(testData,1),1), sparse(double(testData)), cnnModel, '-b 1 -q');
    fgCNNPro = prob(:,1);
    bgCNNPro = prob(:,2);
    
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

    fgPro = sum(fgPix,2);
    bgPro = sum(bgPix,2);
    
    % Unary potentials for location (distance transform)
    dist = double(bwdist(1-imLarges{ii}));
    pro = dist/(max(dist(:)));    

    pro(pro==0) = 10e-5; pro(pro==1) = 1-10e-5;
    fgLocPro = pro(fgIdxL{ii}-pixelNum*(ii-1));
    bgLocPro = 1-fgLocPro;
    
    % Unary potentials
    tmp = [-log(fgCNNPro)*para.CNNPixelWeight-log(fgPro)*para.colorPixelWeight-log(fgLocPro)*para.locPixelWeight -log(bgCNNPro)*para.CNNPixelWeight-log(bgPro)*para.colorPixelWeight-log(bgLocPro)*para.locPixelWeight];
    unaryPixelPotentials = [unaryPixelPotentials;tmp];
    fgProPixel{ii} = fgPro;
end

% spatial pairwise potentials
[sSource, sDestination] = getSpatialConnections(subPixels, subPixelsNum(end)+1);
sDestination(sSource==0) = [];
sSource(sSource==0) = [];
sSource = sSource-1; sDestination = sDestination-1;

sSqrColourDistance = sum((subColors(sSource+1,:) - subColors(sDestination+1,:)).^ 2,2);
sCentreDistance = sqrt(sum((subCenters(sSource+1,:) - subCenters(sDestination+1,:)).^ 2,2));

sBeta = 0.5/(mean( sSqrColourDistance ./ (sCentreDistance+10e-5) ) + 10e-5);
sPixelValue = exp(-sBeta*sSqrColourDistance)./sCentreDistance;

% temporal pairwise potentials
[tSource, tDestination, tConnections ] = getTemporalConnections(flows, subPixels, subPixelsNum(end)+1);
indS = find(tSource==0);
indD = find(tDestination==0);
ind = [indS;indD];
ind = unique(ind);
tSource(ind) = []; tDestination(ind) = []; tConnections(ind) = [];
tSource = tSource-1; tDestination = tDestination-1;

tSqrColourDistance = sum((subColors(tSource+1,:) - subColors(tDestination+1,:)).^2,2);

tBeta = 0.5 / (mean( tSqrColourDistance .* tConnections ) + 10e-5);
tPixelValue = tConnections .* exp( -tBeta * tSqrColourDistance );

pairPixelPotentials.source = [sSource;tSource];
pairPixelPotentials.destination = [sDestination;tDestination];
pairPixelPotentials.value = [single(sPixelValue*para.spatialPixelWeight);single(tPixelValue*para.temporalPixelWeight)];