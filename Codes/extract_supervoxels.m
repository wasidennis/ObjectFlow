function extract_supervoxels(dataInfo, dirInfo, para)

inputPath = dataInfo.inputPath;

supervoxelSave = [dirInfo.supervoxelPath dataInfo.videoName(1:end-1) '/'];
if ~exist(supervoxelSave,'dir'), mkdir(supervoxelSave); end;
mkdir([inputPath 'ppm']);

supervoxelTmp = [dirInfo.supervoxelPath dataInfo.videoName(1:end-1) '/tmp/'];
for ff = 1:dataInfo.totalFrame-1
    path = [supervoxelSave sprintf('%05d.mat',ff)];
    if exist(path,'file')
        continue;
    end  
    
    % convert images to ppm files
    img1 = dataInfo.videoAll{ff}; img2 = dataInfo.videoAll{ff+1};
    imwrite(img1, [inputPath  'ppm/00001.ppm'], 'ppm');
    imwrite(img2, [inputPath  'ppm/00002.ppm'], 'ppm');
    
    % run frame by frame supervoxel extraction
    mkdir(supervoxelTmp);
    system(['./External/libsvx.v3.0/gbh/gbh 50 100 100 0.5 20 ' inputPath 'ppm/ ' supervoxelTmp]);
        
    % save supervoxel results
    supervoxelsAll = cell(2,1);
    for ii = 1:2
        tmp = imread([supervoxelTmp sprintf('%02d',para.level) sprintf('/%05d.ppm',ii)]);     
        supervoxelsAll{ii} = rgb2gray(tmp);
    end

    save(path, 'supervoxelsAll');
    
    % delete temporary files
    delete([inputPath 'ppm/00001.ppm']);
    delete([inputPath 'ppm/00002.ppm']);
    rmdir(supervoxelTmp, 's');
end
rmdir([inputPath 'ppm']);