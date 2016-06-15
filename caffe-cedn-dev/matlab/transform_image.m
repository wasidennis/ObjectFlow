function patches = transform_image(im, H, padvalue)
% image transformation

[im_h,im_w] = size(im);
patches = zeros(im_h, im_w, length(H));
for cc=1:length(H)
    imt = imtransform2(im, H{cc}, 'bbox', 'crop', 'pad', padvalue);
    patches(:,:,cc) = imt;
end
% montage2(patches)
