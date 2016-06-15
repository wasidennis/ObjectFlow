function Nystrom_video(PathInput, PathOutput, numOfSvx, numOfSamples, numOfEvecs, sigmaE, sigmaLab, KNN)
% Nystrom_video(path_input, path_output, numOfSvx, numOfSamples, numOfEvecs, sigmaE, sigmaLab)
%
% PathInput:
%       Path to your input '.ppm' frame sequence
%
% PathOutput:
%       Path to store the output files
%
% numOfSvx:
%       number of supervoxels
%
% numOfSamples:
%       number of sample points
%
% numOfEvecs:
%       number of eigenvectors
%
% sigmaE:
%       impact of Euclidean distance
%
% sigmaLab:
%       impact of Lab color space
%
% KNN:
%       0 -> off, only K-means Clustering
%       1 -> on,  take 10% k-means, rest do k-nearest-neighbor
%
% Author: Chenliang Xu, SUNY at Buffalo, USA (chenlian@buffalo.edu)
% Date: 2012-03-27

%% Read frame images
disp('Start reading images');
if PathInput(end) == '/'
    PathInput = PathInput(1:end-1);
end
if PathOutput(end) == '/'
    PathOutput = PathOutput(1:end-1);
end
DirInput = dir(PathInput);
numOfFrames = 0;
for i=1:size(DirInput,1)
    if strfind(DirInput(i,1).name, '.ppm') > 0
        numOfFrames = numOfFrames + 1;
        ImLabSeq(:,:,:,numOfFrames) = RGB2Lab(imread([PathInput,'/',DirInput(i,1).name]));
    end
end
ImHight = size(ImLabSeq, 1);
ImWidth = size(ImLabSeq, 2);
V_total = ImHight * ImWidth * numOfFrames;

%% Sampling points;
disp('Sampling points');
[A_points, B_points, numOfSamples] = Nystrom_sampling(V_total, numOfSamples, ImWidth, ImHight, numOfFrames);

matlabpool
% Matrix A
disp('Construct Matrix A');
A = zeros(numOfSamples, numOfSamples);
for i=1:numOfSamples
    [y_i, x_i, t_i] = V_map_3D(A_points(i), ImWidth, ImHight);
    parfor j=1:numOfSamples
        if i==j
            A(i,j) = 1;
        else
            [y_j, x_j, t_j] = V_map_3D(A_points(j), ImWidth, ImHight);
            E_distance = (x_i-x_j)^2 + (y_i-y_j)^2 + (t_i-t_j)^2;
            Lab_distance = (ImLabSeq(y_i,x_i,1,t_i) - ImLabSeq(y_j,x_j,1,t_j))^2 + ...
                (ImLabSeq(y_i,x_i,2,t_i) - ImLabSeq(y_j,x_j,2,t_j))^2 + ...
                (ImLabSeq(y_i,x_i,3,t_i) - ImLabSeq(y_j,x_j,3,t_j))^2;
            A(i,j) = exp((-1/2)*((E_distance/sigmaE)+(Lab_distance/sigmaLab)));
        end
    end
end
% Matrix B
disp('Construct Matrix B');
B = zeros(numOfSamples, V_total - numOfSamples);
for i=1:numOfSamples
    [y_i, x_i, t_i] = V_map_3D(A_points(i), ImWidth, ImHight);
    parfor j=1:V_total-numOfSamples
        [y_j, x_j, t_j] = V_map_3D(B_points(j), ImWidth, ImHight);
        E_distance = (x_i-x_j)^2 + (y_i-y_j)^2 + (t_i-t_j)^2;
        Lab_distance = (ImLabSeq(y_i,x_i,1,t_i) - ImLabSeq(y_j,x_j,1,t_j))^2 + ...
            (ImLabSeq(y_i,x_i,2,t_i) - ImLabSeq(y_j,x_j,2,t_j))^2 + ...
            (ImLabSeq(y_i,x_i,3,t_i) - ImLabSeq(y_j,x_j,3,t_j))^2;
        B(i,j) = exp((-1/2)*((E_distance/sigmaE)+(Lab_distance/sigmaLab)));
    end
end
matlabpool close

%% Nystrom Computing
disp('Nystrom Computing');
E = nystrom(A, B, numOfEvecs);
clear A B im_Lab_seq;

%% Clustering
if KNN == 0
    disp('K-means');
    opts = statset('Display', 'iter', 'MaxIter', 200);
    IDX = kmeans(E, numOfSvx, 'emptyaction', 'drop', 'onlinephase', 'off', 'Options', opts, 'start', 'cluster');
else
    disp('K-means + knn');
    Sample_idx = randsample(1:V_total, V_total/20);
    E_sample = E(Sample_idx, :);
    opts = statset('Display', 'iter', 'MaxIter', 200);
    [~, Cluster] = kmeans(E_sample, numOfSvx, 'emptyaction', 'singleton','onlinephase', 'off', 'Options', opts, 'start', 'cluster');
    Group = [1:numOfSvx]';
    IDX = knnclassify(E, Cluster, Group);
end

%% Assign Colors
disp('Assign Colors');
Color_sample = randsample(1:255*255*255, numOfSvx);
for i=1:numOfSvx
    [R(i), G(i), B(i)] = V_map_3D(Color_sample(1,i),255,255);
end
ImSegSeq = zeros(ImHight, ImWidth, 3, numOfFrames);
for i=1:numOfSamples
    [y_i, x_i, t_i] = V_map_3D(A_points(i), ImWidth, ImHight);
    ImSegSeq(y_i, x_i, 1, t_i) = R(IDX(i));
    ImSegSeq(y_i, x_i, 2, t_i) = G(IDX(i));
    ImSegSeq(y_i, x_i, 3, t_i) = B(IDX(i));
end
for i=numOfSamples+1:V_total
    [y_i, x_i, t_i] = V_map_3D(B_points(i-numOfSamples), ImWidth, ImHight);
    ImSegSeq(y_i, x_i, 1, t_i) = R(IDX(i));
    ImSegSeq(y_i, x_i, 2, t_i) = G(IDX(i));
    ImSegSeq(y_i, x_i, 3, t_i) = B(IDX(i));
end

% mkdir(dir_output_path);
ImSegSeq = uint8(ImSegSeq);
for i=1:numOfFrames
    PathSave = sprintf('%s%s%05d%s',PathOutput,'/',i,'.ppm');
    imwrite(ImSegSeq(:,:,:,i), PathSave);
end
