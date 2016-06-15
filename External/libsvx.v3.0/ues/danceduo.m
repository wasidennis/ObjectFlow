% This script shows an example of using UES on video "danceduo".

video_path = fullfile(pwd,'examples','danceduo','frames_png');
hie_path = fullfile(pwd,'examples','danceduo','hie'); 

hie_select_num = 10; 
sigma = 10;
visflag = 1;

%% Motion-ness
output_path = fullfile(pwd,'examples','danceduo','motion');
method = 'motion';
ues(video_path, hie_path, output_path, hie_select_num, sigma, method, visflag);

%% Objectness
output_path = fullfile(pwd,'examples','danceduo','objectness');
method = 'objectness';
ues(video_path, hie_path, output_path, hie_select_num, sigma, method, visflag);

%% Detection - Person
output_path = fullfile(pwd,'examples','danceduo','detection');
method = 'detection';
ues(video_path, hie_path, output_path, hie_select_num, sigma, method, visflag);
