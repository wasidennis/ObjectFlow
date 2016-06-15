function onlineModel = plotResult(onlineModel, dataInfo, dirInfo, para)
ff = onlineModel.ff;
mask = onlineModel.mask;

if ~isempty(dataInfo.gtMask{ff+1})
    %% load ground truth
    trimap = dataInfo.gtMask{ff+1};
    
    onlineModel.union = onlineModel.union + sum(logical(trimap(:)) | logical(mask(:)));
    onlineModel.inter = onlineModel.inter + sum(logical(trimap(:)) & logical(mask(:)));
end

%% visualize results
if para.seeResult == 1
    im = dataInfo.videoAll{ff+1};
    maskSeg = cat(3,mask,mask,mask);
    imSeg = im.*uint8(maskSeg);
    
    pos_y = 1; pos_x = 3;
    subplot(pos_y,pos_x,1); imshow(im); title([num2str(ff+1) '/' num2str(dataInfo.totalFrame)]);
    
    if ~isempty(dataInfo.gtMask{ff+1})
        imGT = cat(3,trimap,trimap,trimap); imGT = im.*uint8(imGT);
        iou = sum(logical(trimap(:)) & logical(mask(:))) / sum(logical(trimap(:)) | logical(mask(:)));
        
        subplot(pos_y,pos_x,3); imshow(imGT); title('ground truth');
        subplot(pos_y,pos_x,2); imshow(imSeg); title(['IOU: ' sprintf('%f',iou)]);
    else
        clf
        subplot(pos_y,pos_x,2); imshow(imSeg); title('predicted mask');
    end
end

%% save results
if para.saveResult == 1
    path = [dirInfo.resultPath sprintf('%s/%s/',dataInfo.videoName(1:end-1),num2str(dataInfo.objId))];
    if ~exist(path,'dir'), mkdir(path); end;
    save([path sprintf('%05d_mask.mat',ff+1)],'mask','-v7.3');
end