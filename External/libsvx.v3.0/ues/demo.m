% This demo script shows an example running UES Motion-ness on video "bus".
% See boxers.m, danceduo.m and dancers.m for UES with different feature
% criteria.

% Full path to folder contains original video frames.
video_path = fullfile(pwd,'..','example','frames_png');

% Full path to folder contains hierarchical segmentations.
hie_path = fullfile(pwd,'..','example','output_gbh'); 

% Number of selected levels in the hierarchy. Choose a subset of levels to
% form a segmentation tree.
hie_select_num = 10; 

% Binary weight: 1/10/100. This parameter controls the general trend of
% the tree slice.
sigma = 10;

% Visualization of middle steps: 0/1.
visflag = 1;

% Full path to folder saves output.
output_path = fullfile(pwd,'..','example','ues_motion');

% Feature Criteria: ('motion'|'objectness'|'detection'). See settings.m for
% parameter setttings of each method. For example, change the DPM detection
% model file.
method = 'motion';

% UES algorithm
ues(video_path, hie_path, output_path, hie_select_num, sigma, method, visflag);
