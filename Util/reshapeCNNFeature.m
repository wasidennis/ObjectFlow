function feats_reshape = reshapeCNNFeature(feats,pad,im_w,im_h,layers,scales)

feats_reshape = cell(length(layers),1);
for j = 1:length(layers)
    act = feats{j};
    act = reshape(act,[size(act,1),512/scales(j),512/scales(j)]);

    if im_w > im_h && (im_w >= 512 || im_h >= 512)
        tmp = imresize(permute(act,[2,3,1]),[im_w im_w]);
    elseif im_h > im_w && (im_w >= 512 || im_h >= 512)
        tmp = imresize(permute(act,[2,3,1]),[im_h im_h]);
    elseif im_w <= 512 && im_h <= 512
        tmp = imresize(permute(act,[2,3,1]),[512 512]);
    end
    act = imPad(tmp, -pad, 0);
    act = reshape(permute(act,[3,1,2]),[size(act,3),im_w*im_h]);
    feats_reshape{j} = act;
end
