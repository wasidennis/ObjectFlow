function extract_supervoxels(dataInfo, dirInfo, para)

inputPath = dataInfo.inputPath;

supervoxelSave = [dirInfo.supervoxelPath dataInfo.videoName(1:end-1) '/'];
if ~exist(supervoxelSave,'dir'), mkdir(supervoxelSave); end;

% check if supervoxels are already existing
for ff = 1:dataInfo.totalFrame-1
    path = [supervoxelSave sprintf('%05d.mat',ff)];
    if ~exist(path,'file')
        break;
    end
end

if ff == dataInfo.totalFrame-1
    return;
end
    
% convert images to ppm files
mkdir([inputPath 'ppm']);
for ff = 1:dataInfo.totalFrame
    img = dataInfo.videoAll{ff};
    imwrite(img, [inputPath  sprintf('ppm/%05d.ppm', ff)], 'ppm');
end

% compute supervoxels
supervoxelTmp = [dirInfo.supervoxelPath dataInfo.videoName(1:end-1) '/tmp/'];
mkdir(supervoxelTmp);
system(['./External/libsvx.v3.0/gbh_stream/gbh_stream 50 100 100 0.5 60 20 0 ' inputPath 'ppm/ ' supervoxelTmp]);

% save supervoxels
for ff = 1:dataInfo.totalFrame-1
    path = [supervoxelSave sprintf('%05d.mat',ff)];
    if exist(path,'file')
        continue;
    end  
    
    % save supervoxel results
    supervoxelsAll = cell(2,1);
    for ii = 0:1
        tmp = imread([supervoxelTmp sprintf('%02d',para.level) sprintf('/%05d.ppm',ff+ii)]);     
        supervoxelsAll{ii+1} = rgb2gray(tmp);
    end

    save(path, 'supervoxelsAll');
end

% delete temporary files
rmdir(supervoxelTmp, 's');
rmdir([inputPath 'ppm'], 's');