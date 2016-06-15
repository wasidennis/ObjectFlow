function stat = evaluation_length(dir_sv_path)
% stat = evaluation_length(dir_sv_path)

[sv_map, sv_list] = read_video_supervoxels(dir_sv_path);
[I_h, I_w, frame_num] = size(sv_map);

sv_num = size(sv_list,1);
matrix = sv_list(:,2:frame_num+1);

score = mean(sum(matrix,2));
stat = [sv_num, score];