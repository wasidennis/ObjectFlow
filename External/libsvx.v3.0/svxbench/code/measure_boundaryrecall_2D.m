function [score, BR_map_color] = measure_boundaryrecall_2D(gt_frame, sv_frame)
% score = measure_boundaryrecall_2D(gt_frame, sv_frame)

if size(gt_frame) ~= size(sv_frame)
    disp('ERROR: Dimension Wrong!');
end

BR_gt = boundary_generator(gt_frame);
BR_sv = boundary_generator(sv_frame);
Boundary_total = size(find(BR_gt==1), 1);

% % blur
% H = fspecial('gaussian');
% BR_gt = imfilter(BR_gt, H, 'replicate');

BR_match = BR_gt.*BR_sv;

Boundary_recall = size(find(BR_match==1), 1);

% % blur
% Boundary_recall = sum(sum(BR_match));

score = Boundary_recall/Boundary_total;

BR_map = BR_sv;
BR_map = BR_map + BR_match;
BR_missing = BR_gt - BR_match;
BR_missing = BR_missing * 3;
BR_map = BR_map + BR_missing;

BR_map_color = color_BR_2D(BR_map);