function flow = compute_optflow(video_path, paths)
% This function is used to compute optical flow by calling Ce Liu's code.
% Currently, only forward flow is used.

cd(paths.flow_code);
addpath(genpath(paths.flow_code));

% set optical flow parameters
alpha = 0.012;
ratio = 0.75;
minWidth = 20;
nOuterFPIterations = 7;
nInnerFPIterations = 1;
nSORIterations = 30;
para = [alpha,ratio,minWidth,nOuterFPIterations,nInnerFPIterations,nSORIterations];

[frame_list, I_h, I_w] = read_folder(video_path);
flow = zeros(I_h,I_w,2,length(frame_list));
for i=1:length(frame_list)-1
    im1 = im2double(imread(fullfile(video_path,frame_list{i})));
    im2 = im2double(imread(fullfile(video_path,frame_list{i+1})));
    [flow(:,:,1,i), flow(:,:,2,i), ~] = Coarse2FineTwoFrames(im1, im2, para);
end
flow(:,:,:,end) = flow(:,:,:,end-1);

cd(paths.root);