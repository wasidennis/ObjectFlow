function [sv_map, sv_num] = read_video_supervoxels(input_path)
% [sv_map, sv_num] = read_video_supervoxels(input_path)
% input_path: the direcotry of the output segmentation frames encoded in
%   rgb color space, each segment has its unique color
% sv_map: supervoxel index map with size: I_h x I_w x frame_num, which is a
%   3D video volume
% sv_num: number of supervoxels

[frame_list, I_h, I_w] = read_folder(input_path);
sv_map = zeros(I_h, I_w, length(frame_list));
for i=1:length(frame_list)
    sv_im = imread(fullfile(input_path,frame_list{i}));
    sv_map(:,:,i) = rgb2idx_image(sv_im);
end
sv_num = size(unique(sv_map),1);
