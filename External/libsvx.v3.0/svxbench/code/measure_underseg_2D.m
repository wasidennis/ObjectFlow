function score = measure_underseg_2D(gt_frame, sv_frame, gt_idx)
% score = measure_underseg_2D(gt_frame, sv_frame, gt_idx)

if size(gt_frame) ~= size(sv_frame)
    disp('ERROR: Dimension Wrong!');
end

gt_frame = double(gt_frame);

gt_frame(gt_frame>gt_idx) = 0;
gt_frame(gt_frame<gt_idx) = -999999999;
gt_frame(gt_frame>0) = 0;

cover_frame = sv_frame + gt_frame;
cover_frame(cover_frame<0) = 0;
sv_on = unique(cover_frame);
sv_on(1) = [];

sv_size = 0;
for i=1:size(sv_on,1)
    sv_size = sv_size + size(find(sv_frame==sv_on(i)),1);
end

gt_size = size(find(gt_frame==0),1);

score = (sv_size-gt_size)/gt_size;