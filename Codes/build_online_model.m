function onlineModel = build_online_model(onlineModel, dataInfo, dirInfo, para)
%% load and reshape CNN features
onlineModel.cnnFeats{1} = onlineModel.cnnFeats{2};
cnnSave = [dirInfo.cnnPath dataInfo.videoName(1:end-1) sprintf('/%05d.mat',onlineModel.ff+1)];
load(cnnSave); onlineModel.cnnFeats{2} = reshapeCNNFeature(feats,pad,onlineModel.wd,onlineModel.ht,para.layers,para.scales);

%% load supervoxels
supervoxelSave = [dirInfo.supervoxelPath dataInfo.videoName(1:end-1) sprintf('/%05d.mat',onlineModel.ff)];
load(supervoxelSave);
onlineModel.supervoxels = supervoxelsAll;

if onlineModel.ff > 1
    ht = onlineModel.ht; wd = onlineModel.wd;
    ff = onlineModel.ff;
    K = para.K;
    pixelNum = ht*wd;
    pixelMap = reshape((1:pixelNum),[ht,wd]);
    
    gtEst = onlineModel.gtEst;
    cnnFeats = onlineModel.cnnFeats;
    
    %% build the online model for CNN features
    buildPixelCNNSVM;
    onlineModel.cnnModel = cnnModel;
    
    %% build the online model for colors
    buildPixelColorGMM;
    onlineModel.bgInitModel = bgPixModel;
    onlineModel.bgColors_all = bgColors_all;
end