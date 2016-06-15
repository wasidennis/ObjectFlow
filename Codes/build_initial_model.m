function initModel = build_initial_model(dataInfo, dirInfo, para)

gtEst = cell(dataInfo.totalFrame,1);
gtEst{1} = dataInfo.gtMask{1};
[ht,wd] = size(gtEst{1});
initModel.gtEst = gtEst;
initModel.ht = ht;
initModel.wd = wd;

%% build initial model for colors
buildPixelColorGMM_init;
initModel.fgInitModel = fgPixModel; initModel.bgInitModel = bgPixModel;
initModel.fgColorsS_init = fgColorsS; initModel.bgColors_init = bgColors;
initModel.fgColorsS_all = []; initModel.bgColors_all = [];

%% build initial model for CNN features
% load and reshape CNN features
cnnSave = [dirInfo.cnnPath dataInfo.videoName(1:end-1) sprintf('/%05d.mat',1)];
load(cnnSave); cnnFeats{1} = reshapeCNNFeature(feats,pad,wd,ht,para.layers,para.scales);
cnnFeats{2} = cnnFeats{1};

% build model
buildPixelCNNSVM_init;
initModel.cnnModel = cnnModel;
initModel.feats_fg_init = feats_fg; initModel.feats_bg_init = feats_bg;
initModel.cnnFeats = cnnFeats;