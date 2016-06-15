function draw_video_flow(flow, output_path, paths)
% This function is used to visualize optical flow by calling Ce Liu's code.

cd(paths.flow_code);
addpath(genpath(paths.flow_code));

savepath = fullfile(output_path,'video_flow');
if ~exist(savepath, 'dir')
    mkdir(savepath);
end

for i=1:size(flow,4)
    framename = sprintf('%s/%05d.png', savepath, i);
    imflow = flowToColor(flow(:,:,:,i));
    imwrite(imflow, framename);
end

cd(paths.root);