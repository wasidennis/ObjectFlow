function [score, BR_map_color] = measure_boundaryrecall_3D(gt_video, sv_video)
% [score, BR_match] = measure_boundaryrecall_3D_v1(gt_video, sv_video)

if size(gt_video) ~= size(sv_video)
    disp('ERROR: Dimension Wrong!');
end

BR_gt = boundary_generator_3D(gt_video);
BR_sv = boundary_generator_3D(sv_video);

BR_match = BR_gt.*BR_sv;

Boundary_recall = size(find(BR_match==1), 1);
Boundary_total = size(find(BR_gt==1), 1);
score = Boundary_recall/Boundary_total;

BR_map = BR_sv;
BR_map = BR_map + BR_match;
BR_missing = BR_gt - BR_match;
BR_missing = BR_missing * 3;
BR_map = BR_map + BR_missing;

BR_map_color = color_BR_3D(BR_map);