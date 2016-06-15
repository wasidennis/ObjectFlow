% This script shows an example of using UES on video "boxers".

video_path = fullfile(pwd,'examples','boxers','frames_png');
hie_path = fullfile(pwd,'examples','boxers','hie'); 

hie_select_num = 10; 
sigma = 1;
visflag = 1;

%% Motion-ness
output_path = fullfile(pwd,'examples','boxers','motion');
method = 'motion';
ues(video_path, hie_path, output_path, hie_select_num, sigma, method, visflag);

%% Objectness
output_path = fullfile(pwd,'examples','boxers','objectness');
method = 'objectness';
ues(video_path, hie_path, output_path, hie_select_num, sigma, method, visflag);

%% Detection - Person
output_path = fullfile(pwd,'examples','boxers','detection');
method = 'detection';
ues(video_path, hie_path, output_path, hie_select_num, sigma, method, visflag);
