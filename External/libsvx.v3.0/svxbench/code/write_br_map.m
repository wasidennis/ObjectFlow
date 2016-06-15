function write_br_map(BR_map_color, dir_save_path)

if strcmp(dir_save_path(length(dir_save_path)), '/') > 0
    dir_save_path = dir_save_path(1:length(dir_save_path)-1);
end

mkdir(dir_save_path);
frame_num = size(BR_map_color,4);
for i=1:frame_num
    savepath = sprintf('%s%s%d%s',dir_save_path,'/',i,'.png');
    imwrite(BR_map_color(:,:,:,i), savepath);
end
