function idx_image = rgb2idx_image(rgb_image)
% idx_image = rgb2idx_image(rgb_image)
% If a pixel value is (255, 0, 99), then the 
%   index value = (255 + 1) * 1000000 + (0 + 1) * 1000 + (99 + 1)
%               = 256001100

rgb_image = double(rgb_image);
idx_image = (rgb_image(:,:,1) + 1) * 1000000 +...
    (rgb_image(:,:,2) + 1) * 1000 +...
    (rgb_image(:,:,3) + 1);
