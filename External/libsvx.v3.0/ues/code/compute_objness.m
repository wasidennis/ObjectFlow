function obj = compute_objness(video_path, win_num, paths)
% This function is used to compute objectness by calling external code,
% then quantize the objectness field.

%% compute objness for each frame in a video
cd(paths.objectness_code);
startup;
[frame_list, I_h, I_w] = read_folder(video_path);
frames_objness = cell(length(frame_list),1);
for i=1:length(frame_list)
    im = imread(fullfile(video_path,frame_list{i}));
    frames_objness{i,1} = runObjectness(im, win_num);
end
cd(paths.root);

%% transfer score to objness energy field
obj = zeros(I_h, I_w, length(frame_list));
for i=1:length(frames_objness)
    for j=1:length(frames_objness{i})
        xmin = floor(frames_objness{i}(j,1));
        ymin = floor(frames_objness{i}(j,2));
        xmax = ceil(frames_objness{i}(j,3));
        ymax = ceil(frames_objness{i}(j,4));
        score = frames_objness{i}(j,5);
        mask = zeros(I_h,I_w);
        mask(ymin:ymax,xmin:xmax) = score;
        obj(:,:,i) = obj(:,:,i) + mask;
    end
end
