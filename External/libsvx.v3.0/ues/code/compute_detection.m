function det = compute_detection(video_path, model_name, model_thresh, paths)
% This function is used to compute DPM detections by calling PFF's DPM and
% then transfer to detection energy field.

%% PFF DPM
cd(paths.detection_code);
load(model_name);
[frame_list, I_h, I_w] = read_folder(video_path);
frames_detection = cell(length(frame_list),1);
for i=1:length(frame_list)
    im = imread(fullfile(video_path,frame_list{i}));
    frames_detection{i,1} = imgdetect(im, model, model_thresh);
    message = sprintf('    detected %d frame %d hits', ...
        i, size(frames_detection{i},1));
    disp(message);
end
cd(paths.root);

%% transfer score to detection energy field
det = zeros(I_h, I_w, length(frame_list));
for i=1:length(frames_detection)
    score_base = min(frames_detection{i}(:,6));
    for j=1:size(frames_detection{i},1)
        xmin = floor(frames_detection{i}(j,1));
        ymin = floor(frames_detection{i}(j,2));
        xmax = ceil(frames_detection{i}(j,3));
        ymax = ceil(frames_detection{i}(j,4));
        if xmin < 1
            xmin = 1;
        end
        if ymin < 1
            ymin = 1;
        end
        if xmax > I_w
            xmax = I_w;
        end
        if ymax > I_h
            ymax = I_h;
        end
        score = frames_detection{i}(j,6) - score_base;
        mask = zeros(I_h,I_w);
        mask(ymin:ymax,xmin:xmax) = score;
        det(:,:,i) = det(:,:,i) + mask;
    end
end