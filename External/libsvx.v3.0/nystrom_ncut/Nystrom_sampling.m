function [A_points, B_points, numOfSamples] = Nystrom_sampling(V_total, numOfSamples, ImWidth, ImHight, numOfFrames)

% Pick randomly from first, middle, and last frames
numOfSamples = floor(numOfSamples/3);
frame_middle = floor(numOfFrames/2) + 1;
Points_first = 1:(ImWidth * ImHight);
Points_middle = (ImWidth * ImHight * (frame_middle - 1) + 1):(ImWidth * ImHight * frame_middle);
Points_last = (ImWidth * ImHight * (numOfFrames - 1) + 1):(ImWidth * ImHight * numOfFrames);
A_points = cat(2, randsample(Points_first, numOfSamples), ...
    randsample(Points_middle, numOfSamples), ...
    randsample(Points_last, numOfSamples));
A_points = sort(A_points);

numOfSamples = size(A_points, 2);
B_points = 1:V_total;
B_points(A_points) = [];
A_points = A_points.';
B_points = B_points.';


% % Total random and pick all the frames
% Points = randperm(V_total)';
% A_points = Points(1:numOfSamples);
% B_points = Points(numOfSamples+1:V_total);
% A_points = sort(A_points);
% B_points = sort(B_points);