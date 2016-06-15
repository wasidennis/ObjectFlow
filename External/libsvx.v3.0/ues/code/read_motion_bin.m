function flow_bin = read_motion_bin(flow, mag_num, ang_num)
% This function is used for optical flow quantization.

flow = permute(flow, [1 2 4 3]);
flow_x = flow(:,:,:,1);
flow_y = flow(:,:,:,2);

%% magnitude -- max-min
flow_mag = sqrt(flow_x.^2+flow_y.^2);
% hist_mag = figure;
% hist(flow_mag(:));

mag_interval = zeros(mag_num-1, 1);
% mag_min = min(flow_mag(:));
mag_min = 0;
mag_max = max(flow_mag(:));
for i=1:mag_num-1
    mag_interval(i) = mag_min + (mag_max-mag_min)*i/mag_num;
end
flow_mag_bin = zeros(size(flow_mag));
flow_mag_bin(flow_mag>=mag_interval(end)) = mag_num;
for i=mag_num-1:-1:1
    flow_mag_bin(flow_mag<=mag_interval(i)) = i;
end
% hist(flow_mag_bin(:),1:16);

%% angular -- equal
flow_ang = atan2d(flow_y, flow_x);
% hist_ang = figure;
% hist(flow_ang(:));

ang_interval = -180:360/ang_num:180;
ang_interval = ang_interval(2:end-1);
flow_ang_bin = zeros(size(flow_ang));
flow_ang_bin(flow_ang>=ang_interval(end)) = ang_num;
for i=ang_num-1:-1:1
    flow_ang_bin(flow_ang<=ang_interval(i)) = i;
end
% hist(flow_ang_bin(:),1:16);

%% combine
flow_bin = (flow_mag_bin-1).*4 + flow_ang_bin;
% hist(flow_bin(:),1:mag_num*ang_num);



