function stat = evaluation_chen(path_sv, path_gt)
% stat = evaluation_chen(path_sv, path_gt)
% stat = [sv_num; ue_2D; accu_2D; br_2D; ue_3D; accu_3D; br_3D]

% read the supervoxel segmentation video
[sv_map, sv_list] = read_video_supervoxels(path_sv);
[I_h, I_w, frame_num] = size(sv_map);
% supervoxel number in the video
sv_num = size(sv_list,1);

% read the ground-truth file
[gt_map, gt_list] = read_gt_chen(path_gt, I_w, I_h);
% ground-truth segment number in the video
gt_num = size(gt_list, 1);

ue_2D_matrix = zeros(gt_num, frame_num);
accu_2D_matrix = zeros(gt_num, frame_num);
for i=1:gt_num % test for each segment in the ground-truth map
    for j=1:frame_num % go over the video frame by frame
        if gt_list(i, j+1) == 1 % if the segment exist in that frame
            ue_2D_matrix(i,j) = measure_underseg_2D(gt_map(:,:,j), sv_map(:,:,j), gt_list(i,1));
            [accu_2D_matrix(i,j), ~] = measure_accuracy_2D(gt_map(:,:,j), sv_map(:,:,j), gt_list(i,1));
        end
    end
end

% calculate for every appeared segments
ue_2D = sum(ue_2D_matrix,2)./sum(gt_list(:,2:frame_num+1),2);
accu_2D = sum(accu_2D_matrix,2)./sum(gt_list(:,2:frame_num+1),2);
% take average over all segments, each segment have the same weight
ue_2D = mean(ue_2D)
accu_2D = mean(accu_2D)

br_2D_matrix = zeros(1, frame_num);
for i=1:frame_num
    br_2D_matrix(1,i) = measure_boundaryrecall_2D(gt_map(:,:,i), sv_map(:,:,i));
end
br_2D = mean(br_2D_matrix)


ue_3D_temp = zeros(gt_num,1);
accu_3D_temp = zeros(gt_num,1);
for i=1:gt_num
    ue_3D_temp(i,1) = measure_underseg_3D(gt_map, sv_map, gt_list(i,1));
    accu_3D_temp(i,1) = measure_accuracy_3D(gt_map, sv_map, gt_list(i,1));
end
ue_3D = mean(ue_3D_temp)
accu_3D = mean(accu_3D_temp)
[br_3D, ~] = measure_boundaryrecall_3D(gt_map, sv_map);
br_3D

stat = [sv_num; ue_2D; accu_2D; br_2D; ue_3D; accu_3D; br_3D];