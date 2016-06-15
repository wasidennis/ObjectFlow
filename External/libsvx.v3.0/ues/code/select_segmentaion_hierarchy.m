function [HIE, T] = select_segmentaion_hierarchy(hie_path, select_num)
% This function is used to select levels in a supervoxel hierarchy and then
% construct a segmentation tree structure by selected levels.

%% HIE to store selected hierarchical segmentation
hie_dir = dir(hie_path);
isub = [hie_dir(:).isdir];
nameFolds = {hie_dir(isub).name}';
nameFolds(ismember(nameFolds,{'.','..'})) = [];

% check number of supervoxels in each subfolder
nameFolds_svnum = zeros(length(nameFolds),1);
for i=1:length(nameFolds)
    [~, nameFolds_svnum(i)] = read_video_supervoxels(...
        fullfile(hie_path,nameFolds{i}));
end

% unique segmentation (exclude duplicate layers in hierarchy)
[~, nameFolds_idx] = unique(nameFolds_svnum); % sort in incresase order
unique_num = length(nameFolds_idx);
message = sprintf('    Number of unique layers: %d', unique_num);
disp(message);

% select hierarchy folders according to nameFolds_idx
% We select the min and max in a hierarchy, and uniformly select others.
if unique_num <= select_num
    select_list = nameFolds_idx;
    select_num = unique_num;
else
    step = floor(unique_num/select_num);
    select_list = nameFolds_idx(1:step:step*(select_num-1)+1);
    select_list(end) = nameFolds_idx(end);
end
message = sprintf('    Select %d layers', select_num);
disp(message);

% build HIE
sv_total = 0;
for i=1:select_num
    HIE(i).id = i;
    HIE(i).sv_map = read_video_supervoxels( ...
        fullfile(hie_path,nameFolds{select_list(i)}));
    
    HIE(i).sv_map = cvlbmap(HIE(i).sv_map);
    HIE(i).sv_map = HIE(i).sv_map + sv_total;
    HIE(i).sv_num = length(unique(HIE(i).sv_map));
    HIE(i).sv_start = sv_total + 1;
    sv_total = sv_total + HIE(i).sv_num;
    HIE(i).sv_end = sv_total;
    HIE(i).path_name = nameFolds{select_list(i)};
    
    message = sprintf('    layer %d: folder %s/ svx %d', i, ...
        HIE(i).path_name , HIE(i).sv_num);
    disp(message);
end


%% Some Useful Data Structure for Tree
message = sprintf('    constructing segmentation tree ...');
disp(message);
% Total sv number in T
T.sv_num = sv_total;

% Id2Hie, a list maps sv id to coorespond layer
T.Id2Hie = zeros(T.sv_num, 1);
for i=1:length(HIE)
    T.Id2Hie(HIE(i).sv_start:HIE(i).sv_end) = i;
end

% Id2Pa, a list maps sv id to its direct parent node
T.Id2Pa = zeros(T.sv_num, 1);
for i=1:T.sv_num
    svmap_i = (HIE(T.Id2Hie(i)).sv_map==i);
    if i >= HIE(2).sv_start   
        T.Id2Pa(i) = unique(HIE(T.Id2Hie(i)-1).sv_map(svmap_i));
    end
end

% Id2Anc, a list of sv id lists mapping to its all ancestor nodes
T.Id2Anc = [];
for i=1:T.sv_num
    tmp_p = T.Id2Pa(i);
    T.Id2Anc{i} = tmp_p;
    while tmp_p ~= 0
        tmp_p = T.Id2Pa(tmp_p);
        T.Id2Anc{i} = [T.Id2Anc{i}; tmp_p];
    end
end

% Id2Size, a list of sv volume size
T.Id2Size = zeros(T.sv_num, 1);
for i=1:T.sv_num
    svmap_i = (HIE(T.Id2Hie(i)).sv_map==i);
    T.Id2Size(i) = sum(svmap_i(:));
end