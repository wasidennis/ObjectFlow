function draw_video_detection(det, output_path)

%% frames separately
frames_savepath = fullfile(output_path,'frames_detection');
if ~exist(frames_savepath, 'dir')
    mkdir(frames_savepath);
end

for i=1:size(det,3)
    framename = sprintf('%s/%05d.png', frames_savepath, i);
    imagesc(det(:,:,i));
    axis image
    axis off
    saveas(gcf, framename);
end

%% video as a volume
% video_savepath = fullfile(output_path,'video_detection');
% if ~exist(video_savepath, 'dir')
%     mkdir(video_savepath);
% end
% 
% maxscore = max(max(max(det)));
% 
% for i=1:size(det,3)
%     framename = sprintf('%s/%05d.png', video_savepath, i);
%     imagesc(det(:,:,i));
%     axis image
%     axis off
%     caxis([0 maxscore])
%     saveas(gcf, framename);
% end

