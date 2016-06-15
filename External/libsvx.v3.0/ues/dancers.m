% This script shows an example of using UES on video "dancers".

video_path = fullfile(pwd,'examples','dancers','frames_png');
hie_path = fullfile(pwd,'examples','dancers','hie'); 

hie_select_num = 10; 
sigma = 100;
visflag = 1;

%% Motion-ness
output_path = fullfile(pwd,'examples','dancers','motion');
method = 'motion';
ues(video_path, hie_path, output_path, hie_select_num, sigma, method, visflag);

%% Objectness
output_path = fullfile(pwd,'examples','dancers','objectness');
method = 'objectness';
ues(video_path, hie_path, output_path, hie_select_num, sigma, method, visflag);

%% Detection - Person
output_path = fullfile(pwd,'examples','dancers','detection');
method = 'detection';
ues(video_path, hie_path, output_path, hie_select_num, sigma, method, visflag);
