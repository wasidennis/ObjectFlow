function BR_map_rgb = color_BR_3D(BR_map)

[I_h, I_w, frame_num] = size(BR_map);
BR_map_rgb = zeros(I_h, I_w, 3, frame_num);

for k=1:frame_num
    for i=1:I_h
        for j=1:I_w
            if BR_map(i,j,k) == 1
                % fp -- green
                BR_map_rgb(i,j,2,k) = 255;
            end
            if BR_map(i,j,k) == 2
                % tp -- red
                BR_map_rgb(i,j,1,k) = 255;
            end
            if BR_map(i,j,k) == 3
                % fn -- blue
                BR_map_rgb(i,j,3,k) = 255;
            end
        end
    end
end
