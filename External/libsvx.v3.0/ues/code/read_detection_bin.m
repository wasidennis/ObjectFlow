function detection_bin = read_detection_bin(det, det_num)

%% video as one
% det_max = max(max(max(det)));
% det_min = min(min(min(det)));
% det_interval = zeros(det_num-1,1);
% for i=1:det_num-1
%     det_interval(i) = det_min + (det_max-det_min)*i/det_num;
% end
% detection_bin = zeros(size(det));
% detection_bin(det>=det_interval(end)) = det_num;
% for i=det_num-1:-1:1
%     detection_bin(det<=det_interval(i)) = i;
% end

%% frame as one
[I_h, I_w, frame_num] = size(det);
detection_bin = zeros(size(det));
for i=1:frame_num
    det_max = max(max(det(:,:,i)));
    det_min = min(min(det(:,:,i)));
    det_interval = zeros(det_num-1,1);
    
    % 2^p devide
    for k=1:det_num-1
       if k==1
           det_interval(k) = det_min + (det_max-det_min)/(2^k);
       else
           det_interval(k) = det_interval(k-1) + (det_max-det_min)/(2^k);
       end
    end
        
    % equal devide
%     for k=1:det_num-1
%         det_interval(k) = det_min + (det_max-det_min)*k/det_num;
%     end
    
    mask = zeros(I_h,I_w);
    mask(det(:,:,i)>=det_interval(end)) = det_num;
    for k=det_num-1:-1:1
        mask(det(:,:,i)<=det_interval(k)) = k;
    end
    detection_bin(:,:,i) = mask;
%     pause(0.5);
%     imagesc(detection_bin(:,:,i));
%     pause(0.5);
end
