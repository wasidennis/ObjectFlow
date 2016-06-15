function draw_images(x, HIE, T, output_path)

x(x<1) = 0;
x(x>1) = 1;

numOfSvx = sum(x);
Color_sample = randsample(1:255*255*255, numOfSvx);
for i=1:numOfSvx
    [R(i), G(i), B(i)] = V_map_3D(Color_sample(1,i),255,255);
end

[V_h, V_w, V_f] = size(HIE(1).sv_map);
video_r = zeros(V_h, V_w, V_f);
video_g = zeros(V_h, V_w, V_f);
video_b = zeros(V_h, V_w, V_f);
color_i = 0;
for i=1:length(x)
    if x(i)==1
        color_i = color_i + 1;
        svmap_i = (HIE(T.Id2Hie(i)).sv_map==i);
        video_r(svmap_i) = R(color_i);
        video_g(svmap_i) = G(color_i);
        video_b(svmap_i) = B(color_i);
    end
end

if ~exist([output_path,'/image'], 'dir')
    mkdir([output_path,'/image']);
end

for i=1:V_f
    savepath = sprintf('%s/image/%05d.png', output_path, i);
    im = cat(3, video_r(:,:,i), video_g(:,:,i), video_b(:,:,i));
    im = uint8(im);
    imwrite(im, savepath);
end