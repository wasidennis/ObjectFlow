function score = measure_underseg_3D(gt_map, sv_map, gt_idx)
% score = measure_underseg_3D(gt_map, sv_map, gt_idx)

if size(gt_map) ~= size(sv_map)
    disp('ERROR: Dimension Wrong!');
end

gt_map = double(gt_map);

gt_map(gt_map>gt_idx) = 0;
gt_map(gt_map<gt_idx) = -999999999;
gt_map(gt_map>0) = 0;

cover_map = sv_map + gt_map;
cover_map(cover_map<0) = 0;
sv_on = unique(cover_map);
sv_on(1) = [];

sv_size = 0;
for i=1:size(sv_on,1)
    sv_size = sv_size + size(find(sv_map==sv_on(i)),1);
end

gt_size = size(find(gt_map==0),1);

score = (sv_size-gt_size)/gt_size;