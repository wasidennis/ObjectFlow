function treeify(sourcePath)
% This function modifies a hierarchical segmentation (e.g. SWA is a
% network) to a tree structure.
% Authors: Spencer Whitt and Chenliang Xu
% Contact: {swhitt,chenlian}@buffalo.edu
% Date: Nov. 2013


    cd(sourcePath);

    swa_path = '.';
    dirs = list_dirs(swa_path);
    %vid = cellfun(@load_video, dirs, 'UniformOutput', false);
    basevid = load_video(dirs{1});

    basevidvec = reshape(basevid, [], 1);
    write_images(basevid, 0);

    for i=2:length(dirs)
        % Get videos of interest
        %basevid = vid{i-1};
        currvid = load_video(dirs{i});

        % Reformat them into vectors of pixels
        currvidvec = reshape(currvid, [], 1);

        % Get the most overlapped sv for each sv
        lut = accumarray(basevidvec, currvidvec, [], @mode);

        % Generate subsequent hierarchy level from previous hierarchy level
        % by replacing each supervoxel id with the one listed in lut
        currvid = arrayfun(@(x)lut(x), basevid);

        write_images(currvid, i-1);

        basevid = currvid;
        basevidvec = currvidvec;
    end
end

function write_images(vid, i)
    vid = cvlbmap(vid);
    colors = randsample(1:255*255*255, max(vid(:)));
    for j=1:length(colors)
        [R(j),G(j),B(j)] = V_map_3D(colors(j), 255,255);
    end
    Rvid = arrayfun(@(x)R(x), vid);
    Gvid = arrayfun(@(x)G(x), vid);
    Bvid = arrayfun(@(x)B(x), vid);
    mkdir(sprintf('treeified/%02d', i));
    vid = cat(4, Rvid,Gvid,Bvid);
    disp(size(vid));
    for f=1:size(vid,3)
        imwrite(uint8(squeeze(vid(:,:,f,:))), sprintf('treeified/%02d/%05d.png', i, f));
    end
end

function vid = load_video(vidpath)
    disp(vidpath);
    imgs = list_images(vidpath);

    [x,y] = size(svread(imgs{1}));
    vid = zeros(x,y,length(imgs));
    for i=1:length(imgs)
        vid(:,:,i) = svread(imgs{i});
    end
end

function out = list_images(vidpath)
    out = dir(fullfile(vidpath, '*.png'));
    out = {out.name};

    % Concatenate path prefix to generate full relative path
    out = cellfun(@(x)fullfile(vidpath, x), out, 'UniformOutput', false);
end

function out = list_dirs(vidpath)
    out = dir(fullfile(vidpath));
    mask = [out.isdir];
    out = {out(mask).name};

    % Remove . and ..
    out(1) = [];
    out(1) = [];

    % Concatenate path prefix to generate full relative path
    out = cellfun(@(x)fullfile(vidpath, x), out, 'UniformOutput', false);
end

function Y = svread(X)
Y = rgb2idx_image(imread(X));
end

function Y = cvlbmap(X)
    % convert labels in X to 1:N

    u = unique(X);
    SparseIndex = sparse(u,1,1:length(u),max(u),1);
    Y = full(SparseIndex(X(:)));
    Y = reshape(Y,size(X));
end

function idx_image = rgb2idx_image(rgb_image)
    % idx_image = rgb2idx_image(rgb_image)
    % If a pixel value is (255, 0, 99), then the
    %   index value = (255 + 1) * 1000000 + (0 + 1) * 1000 + (99 + 1)
    %               = 256001100

    rgb_image = double(rgb_image);
    idx_image = (rgb_image(:,:,1) + 1) * 1000000 +...
        (rgb_image(:,:,2) + 1) * 1000 +...
        (rgb_image(:,:,3) + 1);
end

function [y, x, t] = V_map_3D(V_num, I_w, I_h)

t = floor((V_num - 1)/(I_w * I_h)) + 1;
y = floor((V_num - 1 - ((t-1) * I_w * I_h)) / I_w) + 1;
x = mod(V_num - 1 - ((t-1) * I_w * I_h), I_w) + 1;
end
