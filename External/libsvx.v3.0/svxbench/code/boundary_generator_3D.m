function BR_map = boundary_generator_3D(video_map)

[I_h, I_w, frame_num] = size(video_map);
BR_map = zeros(I_h, I_w, frame_num);

for k=1:frame_num
    for i=1:I_h
        for j=1:I_w
            if video_map(i,j,k) ~= 0
                flag = 0;
                if i - 1 >= 1
                    if video_map(i-1,j,k) ~= video_map(i,j,k)
                        flag = 1;
                    end
                end
                if i + 1 <= I_h
                    if video_map(i+1,j,k) ~= video_map(i,j,k)
                        flag = 1;
                    end
                end
                if j - 1 >= 1
                    if video_map(i,j-1,k) ~= video_map(i,j,k)
                        flag = 1;
                    end
                end
                if j + 1 <= I_w
                    if video_map(i,j+1,k) ~= video_map(i,j,k)
                        flag = 1;
                    end
                end
                if k - 1 >= 1
                    if video_map(i,j,k-1) ~= video_map(i,j,k)
                        flag = 1;
                    end
                end
                if k + 1 <= frame_num
                    if video_map(i,j,k+1) ~= video_map(i,j,k)
                        flag = 1;
                    end
                end
                if flag == 1
                    BR_map(i,j,k) = 1;
                end
            end
        end
    end
end
