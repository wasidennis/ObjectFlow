function dataInfo = preprocess_video(dataInfo, dirInfo, para)
%% load video data
dataInfo.inputPath = [dataInfo.videoPath dataInfo.videoName];
inputPath = dataInfo.inputPath;

list = dir([inputPath '*.' dataInfo.videoFormat]);
dataInfo.totalFrame = length(list);
totalFrame = dataInfo.totalFrame;

videoAll = cell(totalFrame,1);
for ff = 1:totalFrame
    videoAll{ff} = imread([inputPath '/' list(ff).name]);
end
dataInfo.videoAll = videoAll;

%% compute optical flows
dataInfo = compute_optical_flow(dataInfo, dirInfo);

%% compute CNN features
extract_CNN_features(dataInfo, dirInfo, para);

%% compute supervoxels
extract_supervoxels(dataInfo, dirInfo, para);

