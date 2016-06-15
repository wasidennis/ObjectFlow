function COMBINE_CURVE_EV(test, dataset, x_min, x_max, output_path)
% COMBINE_CURVE_EV(test, dataset, x_min, x_max)
% dataset = 0 ----> SegTrack Data Set
% dataset = 1 ----> Chen's Data Set
% dataset = 2 ----> Gatech Data Set

if dataset == 0
    load('SegTrack_EV.mat');
    video_num = 6;
end

if dataset == 1
    load('Chen_EV.mat');
    video_num = 8;
end

if dataset == 2
    load('Gatech_EV.mat');
    video_num = 15;
end

x_scale = 200:100:900; % gb, gbh, meanshift, swa
x_num = size(x_scale,2);

x_scale_nys = 200:100:500; % nys
x_num_nys = size(x_scale_nys,2);

x_scale_test = x_min:100:x_max; % test
x_num_test = size(x_scale_test,2);

% swa
swa_ev = zeros(video_num, x_num);
for i=1:video_num
    swa_ev(i,:) = interp1(swa(1,i).stat_sorted(1,:), swa(1,i).stat_sorted(2,:), x_scale);
end
swa_ev_mean = mean(swa_ev);
% gb
gb_ev = zeros(video_num, x_num);
for i=1:video_num
    gb_ev(i,:) = interp1(gb(1,i).stat_sorted(1,:), gb(1,i).stat_sorted(2,:), x_scale);
end
gb_ev_mean = mean(gb_ev);
% gbh
gbh_ev = zeros(video_num, x_num);
for i=1:video_num
    gbh_ev(i,:) = interp1(gbh(1,i).stat_sorted(1,:), gbh(1,i).stat_sorted(2,:), x_scale);
end
gbh_ev_mean = mean(gbh_ev);
% meanshift
meanshift_ev = zeros(video_num, x_num);
for i=1:video_num
    meanshift_ev(i,:) = interp1(meanshift(1,i).stat_sorted(1,:), meanshift(1,i).stat_sorted(2,:), x_scale);
end
meanshift_ev_mean = mean(meanshift_ev);
% nys
nys_ev = zeros(video_num, x_num_nys);
for i=1:video_num
    nys_ev(i,:) = interp1(nys(1,i).stat_sorted(1,:), nys(1,i).stat_sorted(2,:), x_scale_nys);
end
nys_ev_mean = mean(nys_ev);

% test
test_ev = zeros(video_num, x_num_test);
for i=1:video_num
    test_ev(i,:) = interp1(test{1,i}.stat_sorted(1,:), test{1,i}.stat_sorted(2,:), x_scale_test);
end
test_ev_mean = mean(test_ev);


colororder = [
	0.00  0.00  1.00
	0.00  0.50  0.00
	1.00  0.00  0.00
	0.00  0.75  0.75
	0.75  0.00  0.75
	0.75  0.75  0.00
	0.25  0.25  0.25
	0.75  0.25  0.25
	0.95  0.95  0.00
	0.25  0.25  0.75
	0.75  0.75  0.75
	0.00  1.00  0.00
	0.76  0.57  0.17
	0.54  0.63  0.22
	0.34  0.57  0.92
	1.00  0.10  0.60
	0.88  0.75  0.73
	0.10  0.49  0.47
	0.66  0.34  0.65
	0.99  0.41  0.23
];


% Explained Variation
f_ev = figure;
xlabel('Number of Supervoxels','fontsize',14);
ylabel('Explained Variation','fontsize',14);
hold all;
swa = plot(x_scale, swa_ev_mean, '-^', 'Color', colororder(1,:), 'LineWidth', 2);
gb = plot(x_scale, gb_ev_mean, '-o', 'MarkerFaceColor', colororder(2,:), 'Color', colororder(2,:), 'LineWidth', 2);
gbh = plot(x_scale, gbh_ev_mean, '-o', 'Color', colororder(3,:), 'LineWidth', 2);
meanshift = plot(x_scale, meanshift_ev_mean, '-s', 'Color', colororder(7,:), 'LineWidth', 2);
nys = plot(x_scale_nys, nys_ev_mean, '-hexagram', 'Color', colororder(4,:), 'LineWidth', 2);
test = plot(x_scale_test, test_ev_mean, '-+', 'Color', colororder(8,:), 'LineWidth', 2);
box on;
hleg = legend([swa, gb, gbh, meanshift, nys, test] , 'SWA', 'GB', 'GBH', 'Meanshift', 'Nystr\"om', 'Test', 'Location', 'SouthEast');
set(hleg, 'FontSize', 14);
set(hleg, 'interpreter', 'latex');
savepath = [output_path, '/', 'explained_variation.png'];
saveas(f_ev, savepath, 'png');
savepath = [output_path, '/', 'explained_variation.eps'];
print('-depsc',f_ev,savepath);