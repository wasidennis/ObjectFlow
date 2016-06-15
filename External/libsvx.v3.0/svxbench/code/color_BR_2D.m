function BR_map_rgb = color_BR_2D(BR_map)

[I_h, I_w] = size(BR_map);
BR_map_rgb = zeros(I_h, I_w, 3);

for i=1:I_h
    for j=1:I_w
        if BR_map(i,j) == 1
            % fp -- green
            BR_map_rgb(i,j,2) = 255;
        end
        if BR_map(i,j) == 2
            % tp -- red
            BR_map_rgb(i,j,1) = 255;
        end
        if BR_map(i,j) == 3
            % fn -- blue
            BR_map_rgb(i,j,3) = 255;
        end
    end
end
