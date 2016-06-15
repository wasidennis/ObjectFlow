function BR_map = boundary_generator(frame_map)

[I_h, I_w] = size(frame_map);
BR_map = zeros(I_h, I_w);

for i=1:I_h
    for j=1:I_w
        if frame_map(i,j) ~= 0
            flag = 0;
            if i - 1 >= 1
                if frame_map(i-1, j) ~= frame_map(i, j)
                    flag = 1;
                end
            end
            if i + 1 <= I_h
                if frame_map(i+1, j) ~= frame_map(i, j)
                    flag = 1;
                end
            end
            if j - 1 >= 1
                if frame_map(i, j-1) ~= frame_map(i, j)
                    flag = 1;
                end
            end
            if j + 1 <= I_w
                if frame_map(i, j+1) ~= frame_map(i, j)
                    flag = 1;
                end
            end
            if flag == 1
                BR_map(i,j) = 1;
            end
        end
    end
end
