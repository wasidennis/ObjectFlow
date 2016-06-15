function [sv_map, sv_list] = read_video_supervoxels(path_input)
% [sv_map, sv_list] = read_video_supervoxels(dir_input_path)
% path_input: the direcotry of the output segmentation frames encoded into
%   rgb color space, each segment has its unique color
% sv_map: supervoxel index map with size: I_h x I_w x frame_num, which is a
%   3D video volume
% sv_list: supervoxel status list with size: supervoxel number x 
%   (frame number + 1), first column is supervoxel index, following columns
%   are the binary status of each supervoxel in that frame
%   1 - exist, 0 - not exist


if path_input(end) == '/'
    path_input = path_input(1:end-1);
end
dir_input = dir(path_input);

frame = 0;
for i=3:size(dir_input,1)
    if strfind(dir_input(i,1).name, '.p') > 0
        sv_im = imread([path_input,'/',dir_input(i,1).name]);
        frame = frame + 1;
        sv_map(:,:,frame) = rgb2idx_image(sv_im);
    end
end

sv_list = zeros(size(unique(sv_map),1), frame+1);
sv_list(:,1) = unique(sv_map);

for i=1:frame
    sv_list(:,i+1) = ismember(sv_list(:,1), sv_map(:,:,i));
end