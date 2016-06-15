function images_t = transform_image_batch(images, transforms, padvalue)
% image transformation

[im_h,im_w,n] = size(images);
images_t = zeros(im_h, im_w, n);
for cc=1:length(transforms)
    im = images(:,:,cc);
    h = reshape(transforms(:,cc),[3,3]);
    imt = imtransform2(im, h, 'bbox', 'crop', 'pad', padvalue);
    images_t(:,:,cc) = imt;
end
% montage2(patches)
