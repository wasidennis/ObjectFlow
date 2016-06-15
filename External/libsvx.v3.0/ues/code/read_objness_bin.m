function objness_bin = read_objness_bin(objness, obj_num)

%% video as one
% obj_max = max(max(max(objness)));
% obj_min = min(min(min(objness)));
% obj_interval = zeros(obj_num-1,1);
% for i=1:obj_num-1
%     obj_interval(i) = obj_min + (obj_max-obj_min)*i/obj_num;
% end
% objness_bin = zeros(size(objness));
% objness_bin(objness>=obj_interval(end)) = obj_num;
% for i=obj_num-1:-1:1
%     objness_bin(objness<=obj_interval(i)) = i;
% end

%% frame as one
[I_h, I_w, frame_num] = size(objness);
objness_bin = zeros(size(objness));
for i=1:frame_num
    obj_max = max(max(objness(:,:,i)));
    obj_min = min(min(objness(:,:,i)));
    obj_interval = zeros(obj_num-1,1);
    
    % 2^p devide
    for k=1:obj_num-1
       if k==1
           obj_interval(k) = obj_min + (obj_max-obj_min)/(2^k);
       else
           obj_interval(k) = obj_interval(k-1) + (obj_max-obj_min)/(2^k);
       end
    end
        
    % equal devide
%     for k=1:obj_num-1
%         obj_interval(k) = obj_min + (obj_max-obj_min)*k/obj_num;
%     end
    
    mask = zeros(I_h,I_w);
    mask(objness(:,:,i)>=obj_interval(end)) = obj_num;
    for k=obj_num-1:-1:1
        mask(objness(:,:,i)<=obj_interval(k)) = k;
    end
    objness_bin(:,:,i) = mask;
%     pause(0.5);
%     imagesc(objness_bin(:,:,i));
%     pause(0.5);
end
