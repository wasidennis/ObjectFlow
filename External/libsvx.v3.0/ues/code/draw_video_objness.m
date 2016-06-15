function draw_video_objness(video_objness, output_path)

%% frames separately
frames_savepath = fullfile(output_path,'frames_objness');
if ~exist(frames_savepath, 'dir')
    mkdir(frames_savepath);
end

for i=1:size(video_objness,3)
    framename = sprintf('%s/%05d.png', frames_savepath, i);
    imagesc(video_objness(:,:,i));
    axis image
    axis off
    saveas(gcf, framename);
end

%% video as a volume
% video_savepath = fullfile(output_path,'video_objness');
% if ~exist(video_savepath, 'dir')
%     mkdir(video_savepath);
% end
% 
% maxscore = max(max(max(video_objness)));
% 
% for i=1:size(video_objness,3)
%     framename = sprintf('%s/%05d.png', video_savepath, i);
%     imagesc(video_objness(:,:,i));
%     axis image
%     axis off
%     caxis([0 maxscore])
%     saveas(gcf, framename);
% end

