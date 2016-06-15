function [frame_list, I_h, I_w] = read_folder(input_path)

input_dir = dir(input_path);
frame = 0;
for i=1:length(input_dir)
    if ~isempty(strfind(input_dir(i,1).name, '.png')) || ...
            ~isempty(strfind(input_dir(i,1).name, '.ppm')) || ...
            ~isempty(strfind(input_dir(i,1).name, '.bmp')) || ...
            ~isempty(strfind(input_dir(i,1).name, '.jpg'))
        frame = frame + 1;
        frame_list{frame,1} = input_dir(i,1).name;
    end
end

if ~isempty(frame_list)
    first_frame = imread(fullfile(input_path,frame_list{1}));
    [I_h, I_w, ~] = size(first_frame);
else
    disp('No frames (png, ppm, bmp, jpg) in the folder!');
end
