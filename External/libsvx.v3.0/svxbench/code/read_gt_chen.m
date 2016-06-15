function [gt_map, gt_list] = read_gt_chen(path_input, I_w, I_h)
% [gt_map, gt_list] = read_gt_chen(path_input, I_w, I_h)
% path_input: the path to ground-truth of Chen's data set
% I_w: desired image width of the ground-truth, which should be same as the
%   segmentation results
% I_h: desired image height
% gt_map: ground-truth frames as a 3D volume
% gt_list: ground-truth list of size: number of ground-truth segment x
%   (number of frames + 1), first column is ground-truth segment index, 
%   following columns are the status of the ground-truth segments in video
%   frames.
%   1 -- exsit, 0 -- not exsit
%   Note: we ignore the background index, which is 0


if path_input(end) == '/'
    path_input = path_input(1:end-1);
end
dir_input = dir(path_input);

frame = 0;
for i=3:size(dir_input,1)
    gt_im = imread([path_input, '/', dir_input(i,1).name]);
    gt_im = imresize(gt_im, [I_h I_w], 'nearest');
    frame = frame + 1;
    gt_map(:,:,frame) = gt_im;
end

gt_map = double(gt_map);

gt_list = zeros(size(unique(gt_map),1), frame+1);
gt_list(:,1) = unique(gt_map);
gt_list(1,:) = [];

for i=1:frame
    gt_list(:,i+1) = ismember(gt_list(:,1), gt_map(:,:,i));
end