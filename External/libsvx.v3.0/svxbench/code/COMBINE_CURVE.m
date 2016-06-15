function COMBINE_CURVE(test, dataset, x_min, x_max, output_path)
% COMBINE_CURVE_ECCV(test, dataset, x_min, x_max)
% dataset == 0 ----> SegTrack Data Set
% dataset == 1 ----> Chen's Data Set

if dataset == 0    
    load('SegTrack.mat');
    video_num = 6;
else
    load('Chen.mat');
    video_num = 8;
end

x_scale = 200:100:900; % gb, gbh, meanshift, swa
x_num = size(x_scale,2);
x_scale_nys = 200:100:500; % nys
x_num_nys = size(x_scale_nys,2);

% set new method
x_scale_test = x_min:100:x_max;
x_num_test = size(x_scale_test, 2);

swa_ue_2d = zeros(video_num,x_num);
swa_ue_3d = zeros(video_num,x_num);
swa_accu_2d = zeros(video_num,x_num);
swa_accu_3d = zeros(video_num,x_num);
swa_br_2d = zeros(video_num,x_num);
swa_br_3d = zeros(video_num,x_num);
for i=1:video_num
    swa_ue_2d(i,:) = interp1(swa(1,i).stat_sorted(1,:), swa(1,i).stat_sorted(2,:), x_scale);
    swa_ue_3d(i,:) = interp1(swa(1,i).stat_sorted(1,:), swa(1,i).stat_sorted(5,:), x_scale);
    swa_accu_2d(i,:) = interp1(swa(1,i).stat_sorted(1,:), swa(1,i).stat_sorted(3,:), x_scale);
    swa_accu_3d(i,:) = interp1(swa(1,i).stat_sorted(1,:), swa(1,i).stat_sorted(6,:), x_scale);
    swa_br_2d(i,:) = interp1(swa(1,i).stat_sorted(1,:), swa(1,i).stat_sorted(4,:), x_scale);
    swa_br_3d(i,:) = interp1(swa(1,i).stat_sorted(1,:), swa(1,i).stat_sorted(7,:), x_scale);
end
swa_ue_2d_mean = mean(swa_ue_2d);
swa_ue_3d_mean = mean(swa_ue_3d);
swa_accu_2d_mean = mean(swa_accu_2d);
swa_accu_3d_mean = mean(swa_accu_3d);
swa_br_2d_mean = mean(swa_br_2d);
swa_br_3d_mean = mean(swa_br_3d);

gb_ue_2d = zeros(video_num, x_num);
gb_ue_3d = zeros(video_num,x_num);
gb_accu_2d = zeros(video_num,x_num);
gb_accu_3d = zeros(video_num,x_num);
gb_br_2d = zeros(video_num,x_num);
gb_br_3d = zeros(video_num,x_num);
for i=1:video_num
    gb_ue_2d(i,:) = interp1(gb(1,i).stat_sorted(1,:), gb(1,i).stat_sorted(2,:), x_scale);
    gb_ue_3d(i,:) = interp1(gb(1,i).stat_sorted(1,:), gb(1,i).stat_sorted(5,:), x_scale);
    gb_accu_2d(i,:) = interp1(gb(1,i).stat_sorted(1,:), gb(1,i).stat_sorted(3,:), x_scale);
    gb_accu_3d(i,:) = interp1(gb(1,i).stat_sorted(1,:), gb(1,i).stat_sorted(6,:), x_scale);
    gb_br_2d(i,:) = interp1(gb(1,i).stat_sorted(1,:), gb(1,i).stat_sorted(4,:), x_scale);
    gb_br_3d(i,:) = interp1(gb(1,i).stat_sorted(1,:), gb(1,i).stat_sorted(7,:), x_scale);
end
gb_ue_2d_mean = mean(gb_ue_2d);
gb_ue_3d_mean = mean(gb_ue_3d);
gb_accu_2d_mean = mean(gb_accu_2d);
gb_accu_3d_mean = mean(gb_accu_3d);
gb_br_2d_mean = mean(gb_br_2d);
gb_br_3d_mean = mean(gb_br_3d);

gbh_ue_2d = zeros(video_num, x_num);
gbh_ue_3d = zeros(video_num,x_num);
gbh_accu_2d = zeros(video_num,x_num);
gbh_accu_3d = zeros(video_num,x_num);
gbh_br_2d = zeros(video_num,x_num);
gbh_br_3d = zeros(video_num,x_num);
for i=1:video_num
    gbh_ue_2d(i,:) = interp1(gbh(1,i).stat_sorted(1,:), gbh(1,i).stat_sorted(2,:), x_scale);
    gbh_ue_3d(i,:) = interp1(gbh(1,i).stat_sorted(1,:), gbh(1,i).stat_sorted(5,:), x_scale);
    gbh_accu_2d(i,:) = interp1(gbh(1,i).stat_sorted(1,:), gbh(1,i).stat_sorted(3,:), x_scale);
    gbh_accu_3d(i,:) = interp1(gbh(1,i).stat_sorted(1,:), gbh(1,i).stat_sorted(6,:), x_scale);
    gbh_br_2d(i,:) = interp1(gbh(1,i).stat_sorted(1,:), gbh(1,i).stat_sorted(4,:), x_scale);
    gbh_br_3d(i,:) = interp1(gbh(1,i).stat_sorted(1,:), gbh(1,i).stat_sorted(7,:), x_scale);
end
gbh_ue_2d_mean = mean(gbh_ue_2d);
gbh_ue_3d_mean = mean(gbh_ue_3d);
gbh_accu_2d_mean = mean(gbh_accu_2d);
gbh_accu_3d_mean = mean(gbh_accu_3d);
gbh_br_2d_mean = mean(gbh_br_2d);
gbh_br_3d_mean = mean(gbh_br_3d);

meanshift_ue_2d = zeros(video_num, x_num);
meanshift_ue_3d = zeros(video_num,x_num);
meanshift_accu_2d = zeros(video_num,x_num);
meanshift_accu_3d = zeros(video_num,x_num);
meanshift_br_2d = zeros(video_num,x_num);
meanshift_br_3d = zeros(video_num,x_num);
for i=1:video_num
    meanshift_ue_2d(i,:) = interp1(meanshift(1,i).stat_sorted(1,:), meanshift(1,i).stat_sorted(2,:), x_scale);
    meanshift_ue_3d(i,:) = interp1(meanshift(1,i).stat_sorted(1,:), meanshift(1,i).stat_sorted(5,:), x_scale);
    meanshift_accu_2d(i,:) = interp1(meanshift(1,i).stat_sorted(1,:), meanshift(1,i).stat_sorted(3,:), x_scale);
    meanshift_accu_3d(i,:) = interp1(meanshift(1,i).stat_sorted(1,:), meanshift(1,i).stat_sorted(6,:), x_scale);
    meanshift_br_2d(i,:) = interp1(meanshift(1,i).stat_sorted(1,:), meanshift(1,i).stat_sorted(4,:), x_scale);
    meanshift_br_3d(i,:) = interp1(meanshift(1,i).stat_sorted(1,:), meanshift(1,i).stat_sorted(7,:), x_scale);
end
meanshift_ue_2d_mean = mean(meanshift_ue_2d);
meanshift_ue_3d_mean = mean(meanshift_ue_3d);
meanshift_accu_2d_mean = mean(meanshift_accu_2d);
meanshift_accu_3d_mean = mean(meanshift_accu_3d);
meanshift_br_2d_mean = mean(meanshift_br_2d);
meanshift_br_3d_mean = mean(meanshift_br_3d);

nys_ue_2d = zeros(video_num, x_num_nys);
nys_ue_3d = zeros(video_num,x_num_nys);
nys_accu_2d = zeros(video_num,x_num_nys);
nys_accu_3d = zeros(video_num,x_num_nys);
nys_br_2d = zeros(video_num,x_num_nys);
nys_br_3d = zeros(video_num,x_num_nys);
for i=1:video_num
    nys_ue_2d(i,:) = interp1(nys(1,i).stat_sorted(1,:), nys(1,i).stat_sorted(2,:), x_scale_nys);
    nys_ue_3d(i,:) = interp1(nys(1,i).stat_sorted(1,:), nys(1,i).stat_sorted(5,:), x_scale_nys);
    nys_accu_2d(i,:) = interp1(nys(1,i).stat_sorted(1,:), nys(1,i).stat_sorted(3,:), x_scale_nys);
    nys_accu_3d(i,:) = interp1(nys(1,i).stat_sorted(1,:), nys(1,i).stat_sorted(6,:), x_scale_nys);
    nys_br_2d(i,:) = interp1(nys(1,i).stat_sorted(1,:), nys(1,i).stat_sorted(4,:), x_scale_nys);
    nys_br_3d(i,:) = interp1(nys(1,i).stat_sorted(1,:), nys(1,i).stat_sorted(7,:), x_scale_nys);
end
nys_ue_2d_mean = mean(nys_ue_2d);
nys_ue_3d_mean = mean(nys_ue_3d);
nys_accu_2d_mean = mean(nys_accu_2d);
nys_accu_3d_mean = mean(nys_accu_3d);
nys_br_2d_mean = mean(nys_br_2d);
nys_br_3d_mean = mean(nys_br_3d);

test_ue_2d = zeros(video_num, x_num_test);
test_ue_3d = zeros(video_num,x_num_test);
test_accu_2d = zeros(video_num,x_num_test);
test_accu_3d = zeros(video_num,x_num_test);
test_br_2d = zeros(video_num,x_num_test);
test_br_3d = zeros(video_num,x_num_test);
for i=1:video_num
    test_ue_2d(i,:) = interp1(test{1,i}.stat_sorted(1,:), test{1,i}.stat_sorted(2,:), x_scale_test);
    test_ue_3d(i,:) = interp1(test{1,i}.stat_sorted(1,:), test{1,i}.stat_sorted(5,:), x_scale_test);
    test_accu_2d(i,:) = interp1(test{1,i}.stat_sorted(1,:), test{1,i}.stat_sorted(3,:), x_scale_test);
    test_accu_3d(i,:) = interp1(test{1,i}.stat_sorted(1,:), test{1,i}.stat_sorted(6,:), x_scale_test);
    test_br_2d(i,:) = interp1(test{1,i}.stat_sorted(1,:), test{1,i}.stat_sorted(4,:), x_scale_test);
    test_br_3d(i,:) = interp1(test{1,i}.stat_sorted(1,:), test{1,i}.stat_sorted(7,:), x_scale_test);
end
test_ue_2d_mean = mean(test_ue_2d);
test_ue_3d_mean = mean(test_ue_3d);
test_accu_2d_mean = mean(test_accu_2d);
test_accu_3d_mean = mean(test_accu_3d);
test_br_2d_mean = mean(test_br_2d);
test_br_3d_mean = mean(test_br_3d);

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

% Undersegmetation Error 2D
f_ue2d = figure;
xlabel('Number of Supervoxels', 'fontsize', 14);
ylabel('2D Undersegmentation Error', 'fontsize',14);
hold all;
swa = plot(x_scale, swa_ue_2d_mean, '-^', 'Color', colororder(1,:), 'LineWidth', 2);
gb = plot(x_scale, gb_ue_2d_mean, '-o', 'MarkerFaceColor', colororder(2,:), 'Color', colororder(2,:), 'LineWidth', 2);
gbh = plot(x_scale, gbh_ue_2d_mean, '-o', 'Color', colororder(3,:), 'LineWidth', 2);
meanshift = plot(x_scale, meanshift_ue_2d_mean, '-s', 'Color', colororder(7,:), 'LineWidth', 2);
nys = plot(x_scale_nys, nys_ue_2d_mean, '-hexagram', 'Color', colororder(4,:), 'LineWidth', 2);
test = plot(x_scale_test, test_ue_2d_mean, '-+', 'Color', colororder(8,:), 'LineWidth', 2);
box on;
hleg = legend([swa, gb, gbh, meanshift, nys, test] , 'SWA', 'GB', 'GBH', 'Meanshift', 'Nystr\"om', 'Test', 'Location', 'NorthEast');
set(hleg, 'FontSize', 14);
set(hleg, 'interpreter', 'latex');
savepath = [output_path, '/', 'undersegmentation_error_2d.png'];
saveas(f_ue2d, savepath, 'png');
savepath = [output_path, '/', 'undersegmentation_error_2d.eps'];
print('-depsc',f_ue2d,savepath);

% Undersegmentation Error 3D
f_ue3d = figure;
xlabel('Number of Supervoxels','fontsize',14);
ylabel('3D Undersegmentation Error','fontsize',14);
hold all;
swa = plot(x_scale, swa_ue_3d_mean, '-^', 'Color', colororder(1,:), 'LineWidth', 2);
gb = plot(x_scale, gb_ue_3d_mean, '-o', 'MarkerFaceColor', colororder(2,:), 'Color', colororder(2,:), 'LineWidth', 2);
gbh = plot(x_scale, gbh_ue_3d_mean, '-o', 'Color', colororder(3,:), 'LineWidth', 2);
meanshift = plot(x_scale, meanshift_ue_3d_mean, '-s', 'Color', colororder(7,:), 'LineWidth', 2);
nys = plot(x_scale_nys, nys_ue_3d_mean, '-hexagram', 'Color', colororder(4,:), 'LineWidth', 2);
test = plot(x_scale_test, test_ue_3d_mean, '-+', 'Color', colororder(8,:), 'LineWidth', 2);
box on;
hleg = legend([swa, gb, gbh, meanshift, nys, test] , 'SWA', 'GB', 'GBH', 'Meanshift', 'Nystr\"om', 'Test', 'Location', 'NorthEast');
set(hleg, 'FontSize', 14);
set(hleg, 'interpreter', 'latex');
savepath = [output_path, '/', 'undersegmentation_error_3d.png'];
saveas(f_ue3d, savepath, 'png');
savepath = [output_path, '/', 'undersegmentation_error_3d.eps'];
print('-depsc',f_ue3d,savepath);

% Segmentation Accuracy 2D
f_accu2d = figure;
xlabel('Number of Supervoxels','fontsize',14);
ylabel('2D Segmentation Accuracy','fontsize',14);
hold all;
swa = plot(x_scale, swa_accu_2d_mean, '-^', 'Color', colororder(1,:), 'LineWidth', 2);
gb = plot(x_scale, gb_accu_2d_mean, '-o', 'MarkerFaceColor', colororder(2,:), 'Color', colororder(2,:), 'LineWidth', 2);
gbh = plot(x_scale, gbh_accu_2d_mean, '-o', 'Color', colororder(3,:), 'LineWidth', 2);
meanshift = plot(x_scale, meanshift_accu_2d_mean, '-s', 'Color', colororder(7,:), 'LineWidth', 2);
nys = plot(x_scale_nys, nys_accu_2d_mean, '-hexagram', 'Color', colororder(4,:), 'LineWidth', 2);
test = plot(x_scale_test, test_accu_2d_mean, '-+', 'Color', colororder(8,:), 'LineWidth', 2);
box on;
hleg = legend([swa, gb, gbh, meanshift, nys, test] , 'SWA', 'GB', 'GBH', 'Meanshift', 'Nystr\"om', 'Test', 'Location', 'SouthEast');
set(hleg, 'FontSize', 14);
set(hleg, 'interpreter', 'latex');
savepath = [output_path, '/', 'segmentation_accuracy_2d.png'];
saveas(f_accu2d, savepath, 'png');
savepath = [output_path, '/', 'segmentation_accuracy_2d.eps'];
print('-depsc',f_accu2d,savepath);

% Segmentation Accuracy 3D
f_accu3d = figure;
xlabel('Number of Supervoxels','fontsize',14);
ylabel('3D Segmentation Accuracy','fontsize',14);
hold all;
swa = plot(x_scale, swa_accu_3d_mean, '-^', 'Color', colororder(1,:), 'LineWidth', 2);
gb = plot(x_scale, gb_accu_3d_mean, '-o', 'MarkerFaceColor', colororder(2,:), 'Color', colororder(2,:), 'LineWidth', 2);
gbh = plot(x_scale, gbh_accu_3d_mean, '-o', 'Color', colororder(3,:), 'LineWidth', 2);
meanshift = plot(x_scale, meanshift_accu_3d_mean, '-s', 'Color', colororder(7,:), 'LineWidth', 2);
nys = plot(x_scale_nys, nys_accu_3d_mean, '-hexagram', 'Color', colororder(4,:), 'LineWidth', 2);
test = plot(x_scale_test, test_accu_3d_mean, '-+', 'Color', colororder(8,:), 'LineWidth', 2);
box on;
hleg = legend([swa, gb, gbh, meanshift, nys, test] , 'SWA', 'GB', 'GBH', 'Meanshift', 'Nystr\"om', 'Test', 'Location', 'SouthEast');
set(hleg, 'FontSize', 14);
set(hleg, 'interpreter', 'latex');
savepath = [output_path, '/', 'segmentation_accuracy_3d.png'];
saveas(f_accu3d, savepath, 'png');
savepath = [output_path, '/', 'segmentation_accuracy_3d.eps'];
print('-depsc',f_accu3d,savepath);

% Boundary Recall 2D
f_br2d = figure;
xlabel('Number of Supervoxels','fontsize',14);
ylabel('2D Boundary Recall','fontsize',14);
hold all;
swa = plot(x_scale, swa_br_2d_mean, '-^', 'Color', colororder(1,:), 'LineWidth', 2);
gb = plot(x_scale, gb_br_2d_mean, '-o', 'MarkerFaceColor', colororder(2,:), 'Color', colororder(2,:), 'LineWidth', 2);
gbh = plot(x_scale, gbh_br_2d_mean, '-o', 'Color', colororder(3,:), 'LineWidth', 2);
meanshift = plot(x_scale, meanshift_br_2d_mean, '-s', 'Color', colororder(7,:), 'LineWidth', 2);
nys = plot(x_scale_nys, nys_br_2d_mean, '-hexagram', 'Color', colororder(4,:), 'LineWidth', 2);
test = plot(x_scale_test, test_br_2d_mean, '-+', 'Color', colororder(8,:), 'LineWidth', 2);
box on;
hleg = legend([swa, gb, gbh, meanshift, nys, test] , 'SWA', 'GB', 'GBH', 'Meanshift', 'Nystr\"om', 'Test', 'Location', 'SouthEast');
set(hleg, 'FontSize', 14);
set(hleg, 'interpreter', 'latex');
savepath = [output_path, '/', 'boundary_recall_2d.png'];
saveas(f_br2d, savepath, 'png');
savepath = [output_path, '/', 'boundary_recall_2d.eps'];
print('-depsc',f_br2d,savepath);

% Boundary Recall 3D
f_br3d = figure;
xlabel('Number of Supervoxels','fontsize',14);
ylabel('3D Boundary Recall','fontsize',14);
hold all;
swa = plot(x_scale, swa_br_3d_mean, '-^', 'Color', colororder(1,:), 'LineWidth', 2);
gb = plot(x_scale, gb_br_3d_mean, '-o', 'MarkerFaceColor', colororder(2,:), 'Color', colororder(2,:), 'LineWidth', 2);
gbh = plot(x_scale, gbh_br_3d_mean, '-o', 'Color', colororder(3,:), 'LineWidth', 2);
meanshift = plot(x_scale, meanshift_br_3d_mean, '-s', 'Color', colororder(7,:), 'LineWidth', 2);
nys = plot(x_scale_nys, nys_br_3d_mean, '-hexagram', 'Color', colororder(4,:), 'LineWidth', 2);
test = plot(x_scale_test, test_br_3d_mean, '-+', 'Color', colororder(8,:), 'LineWidth', 2);
box on;
hleg = legend([swa, gb, gbh, meanshift, nys, test] , 'SWA', 'GB', 'GBH', 'Meanshift', 'Nystr\"om', 'Test', 'Location', 'SouthEast');
set(hleg, 'FontSize', 14);
set(hleg, 'interpreter', 'latex');
savepath = [output_path, '/', 'boundary_recall_3d.png'];
saveas(f_br3d, savepath, 'png');
savepath = [output_path, '/', 'boundary_recall_3d.eps'];
print('-depsc',f_br3d,savepath);