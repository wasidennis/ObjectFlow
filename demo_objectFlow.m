% demo for video object segmentation in the paper of "Video Segmentation via Object Flow,
% Y.-H. Tsai, M.-H. Yang and M. J. Black, CVPR 2016"
clear all;

%% setups
setup_all;
para.seeResult = 1;  % visualize results
para.saveResult = 0; % save binary masks as mat files

%% video data information
% change below for different videos
dataInfo.videoPath = 'Videos/';
dataInfo.videoName = 'aeroplane_01/';
dataInfo.gtName = 'gt/';
dataInfo.videoFormat = 'jpg';
dataInfo.gtFormat = 'jpg';
dataInfo.objId = 1; % choose which object according to the id of ground truth
dataInfo.gtAll = 0; % select '0' if the groundtruths are not availabel for all the frames (but at least require the first one)

%% pre-process data
dataInfo = preprocess_video(dataInfo, dirInfo, para);
inputPath = dataInfo.inputPath;
totalFrame = dataInfo.totalFrame;

%% load groundtruths
gtPath = [inputPath  dataInfo.gtName sprintf('%02d/', dataInfo.objId) '*.' dataInfo.gtFormat];
gtMask = cell(totalFrame,1);
list = dir(gtPath);

if dataInfo.gtAll == 1
    % for complete groundtruths (e.g., segTrack v2 dataset)
    for ff = 1:totalFrame
        tmp = imread([inputPath dataInfo.gtName sprintf('%02d/', dataInfo.objId) list(ff).name]);
        
        % change below according to different ground truth formats
        tmp = rgb2gray(tmp);
        gtMask{ff} = (tmp>128);
    end
else
    % for incomplete groundtruths (e.g., Youtube-Objects dataset)
    for ff = 1:length(list)
        tmp = imread([inputPath dataInfo.gtName sprintf('%02d/', dataInfo.objId) list(ff).name]);
        
        % change below according to different ground truth formats
        frame = str2double(list(ff).name(1:end-4));
        gtMask{frame} = (double(tmp)>128);
    end
end
dataInfo.gtMask = gtMask;

%% build the initial model
tic
fprintf('Build initial models...\n');
onlineModel = build_initial_model(dataInfo, dirInfo, para);

%% track segments from frame t to t+1
onlineModel.union = 0;
onlineModel.inter = 0;
fprintf('Start tracking segments...\n');
for ff = 1:totalFrame-1    
    %% build the online model
    onlineModel.ff = ff;
    onlineModel = build_online_model(onlineModel, dataInfo, dirInfo, para);
    
    %% estimate the object location
    onlineModel.video = dataInfo.videoAll(ff:ff+1); onlineModel.flows = dataInfo.flowsAll(ff); onlineModel.flowsInv = dataInfo.flowsInvAll(totalFrame-ff);
    onlineModel = estimateObjectLoc(onlineModel, para);
    
    %% track object segment 
    % 1) build the multi-level graph
    % 2) compute potentials
    % 3) solved by graph cut
    onlineModel = objectSegmentTracking(onlineModel, para);
    
    %% plot results
    fprintf('Finish segmentating frame %d/%d in %f seconds.\n', ff, totalFrame-1, toc);
    onlineModel = plotResult(onlineModel, dataInfo, dirInfo, para);
end
fprintf('finish segmetnting video: %s obj %d, average IOU: %f.\n\n',dataInfo.videoName(1:end-1), dataInfo.objId, onlineModel.inter/onlineModel.union);

