function dataInfo = compute_optical_flow(dataInfo, dirInfo)

totalFrame = dataInfo.totalFrame;
videoAll = dataInfo.videoAll;

flowSave = [dirInfo.flowPath dataInfo.videoName(1:end-1) '_flow.mat'];
if exist(flowSave,'file')
    load(flowSave);    
else
    flowsAll = cell(totalFrame-1,1);
    for ff = 1:totalFrame-1
        fprintf('Computing optical flow: %d/%d\n',ff,totalFrame-1);
        im1 = double(videoAll{ff});
        im2 = double(videoAll{ff+1});
        
        uv = int16(estimate_flow_interface(im1,im2,'classic+nl-fast'));
        flowsAll{ff}(:,:,1) = uv(:,:,2);
        flowsAll{ff}(:,:,2) = uv(:,:,1);
    end
    save(flowSave,'flowsAll');
end
dataInfo.flowsAll = flowsAll;

flowInvSave = [dirInfo.flowPath dataInfo.videoName(1:end-1) '_flowInv.mat'];
if exist(flowInvSave,'file')
    load(flowInvSave);    
else
    flowsInvAll = cell(totalFrame-1,1);
    for ff = totalFrame:-1:2
        fprintf('Computing inverse optical flow: %d/%d\n',ff-1,totalFrame-1);
        im1 = double(videoAll{ff});
        im2 = double(videoAll{ff-1});
        
        uv = int16(estimate_flow_interface(im1,im2,'classic+nl-fast'));
        flowsInvAll{totalFrame-ff+1}(:,:,1) = uv(:,:,2);
        flowsInvAll{totalFrame-ff+1}(:,:,2) = uv(:,:,1);
    end
    save(flowInvSave,'flowsInvAll');
end
dataInfo.flowsInvAll = flowsInvAll;