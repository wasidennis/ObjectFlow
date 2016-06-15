function stat = explained_variation(path_sv, path_ppm)
% stat = explained_variation(path_sv, path_ppm)

[sv_map, sv_list] = read_video_supervoxels(path_sv);
[I_h, I_w, frame_num] = size(sv_map);
sv_num = size(sv_list,1);

dir_ppm = dir(path_ppm);
ppm = zeros(I_h,I_w,3,frame_num);
for i=3:size(dir_ppm,1)
    ppm(:,:,:,i-2) = imread([path_ppm,'/',dir_ppm(i,1).name]);
end

ppm = double(ppm);

% global points set
points_set = reshape(ppm(:,:,:,1),I_h*I_w,3);
for i=2:frame_num
    points_set = cat(1, points_set, reshape(ppm(:,:,:,i), I_h*I_w, 3));
end

% global mean
mean_global = mean(points_set);
% denominator
denominator = sum(sum(bsxfun(@minus, points_set, mean_global).^2,2));

% compute for every sv
molecular = zeros(sv_num,1);
sv_points_num = zeros(sv_num,1);
parfor i=1:sv_num
    subpoints_idx = find(sv_map==sv_list(i,1));
    sv_points_num(i) = size(subpoints_idx,1);
    molecular(i) = sum((mean(points_set(subpoints_idx,:)) - mean_global).^2) * sv_points_num(i);
end

score = sum(molecular)/denominator

stat = [sv_num; score];