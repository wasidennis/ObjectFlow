function [score, cover_frame] = measure_accuracy_2D(gt_frame, sv_frame, gt_idx)
% [score, cover_frame] = measure_accuracy_2D(gt_frame, sv_frame, gt_idx)

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

count = 0;
sv_size = 0;
for i=1:size(sv_on,1)
    area_in = size(find(cover_frame==sv_on(i)),1);
    area_total = size(find(sv_frame==sv_on(i)),1);
    if area_in >= area_total - area_in
        sv_size = sv_size + area_in;
        count = count + 1;
        sv_good(count,1) = sv_on(i);
    end
end

gt_size = size(find(gt_frame==0),1);
score = sv_size/gt_size;

for i=1:count
    cover_frame(cover_frame==sv_good(i)) = 999999999;
end
cover_frame(cover_frame<999999999) = 0;
cover_frame(cover_frame>0) = 255;