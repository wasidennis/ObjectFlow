function stat = evaluation_segtrack(path_sv, path_gt)
% stat = evaluation_segtrack(path_sv, path_gt)
% stat = [sv_num; ue_2D; accu_2D; br_2D; ue_3D; accu_3D; br_3D]

% read the supervoxel segmentation video
[sv_map, sv_list] = read_video_supervoxels(path_sv);
[I_h, I_w, frame_num] = size(sv_map);

% read the ground-truth file
[gt_map, gt_list] = read_gt_segtrack(path_gt, I_w, I_h);

ue_2D_temp = zeros(1, frame_num);
accu_2D_temp = zeros(1, frame_num);
br_2D_temp = zeros(1, frame_num);
br_map_2D = zeros(I_h, I_w, 3, frame_num);
for i=1:frame_num
    ue_2D_temp(1,i) = measure_underseg_2D(gt_map(:,:,i), sv_map(:,:,i), gt_list(1,1));
    [accu_2D_temp(1,i), ~] = measure_accuracy_2D(gt_map(:,:,i), sv_map(:,:,i), gt_list(1,1));
    [br_2D_temp(1,i), br_map_2D(:,:,:,i)] = measure_boundaryrecall_2D(gt_map(:,:,i), sv_map(:,:,i));
end
ue_2D = mean(ue_2D_temp)
accu_2D = mean(accu_2D_temp)
br_2D = mean(br_2D_temp)

ue_3D = measure_underseg_3D(gt_map, sv_map, gt_list(1,1))
accu_3D = measure_accuracy_3D(gt_map, sv_map, gt_list(1,1))
[br_3D, br_map_3D] = measure_boundaryrecall_3D(gt_map, sv_map);
br_3D

sv_num = size(sv_list,1);
stat = [sv_num; ue_2D; accu_2D; br_2D; ue_3D; accu_3D; br_3D];