function result = TEST_Dataset_EV(path_input_method, path_ppm, dataset, verbose, output_path)
% result = TEST_Dataset_EV(path_input_method, path_ppm, dataset)
% dataset = 0 ----> SegTrack Data Set
% dataset = 1 ----> Chen's Data Set
% dataset = 2 ----> Gatech Data Set

if path_ppm(end) == '/'
    path_ppm = path_ppm(1:end-1);
end
if path_input_method(end) == '/'
    path_input_method = path_input_method(1:end-1);
end
dir_input_method = dir(path_input_method);
videoset_num = size(dir_input_method,1)-2;

for i=1:videoset_num
    result{1,i}.name = dir_input_method(i+2,1).name;
    dir_ppm_video_path = [path_ppm,'/',result{1,i}.name];
    path_input_videoset = [path_input_method,'/',result{1,i}.name];
    dir_input_videoset = dir(path_input_videoset);
    video_num = size(dir_input_videoset,1)-2;
    stat = zeros(2,video_num);
    parfor j=1:video_num
        videopath = [path_input_videoset,'/',dir_input_videoset(j+2,1).name];
        stat(:,j) = explained_variation(videopath, dir_ppm_video_path);
    end
    
    sv_unique = unique(stat(1,:));
    for k = 1:size(sv_unique,2)
        this_sv = sv_unique(1,k);
        this_sv_idx = find(stat(1,:) == this_sv);
        num_sv_same = size(this_sv_idx, 2);
        result{1,i}.stat(:,k) = sum(stat(:, this_sv_idx), 2)/num_sv_same;
    end
    [~, I] = sort(result{1,i}.stat(1,:));
    result{1,i}.stat_sorted = result{1,i}.stat(:,I);
end

if verbose == 1

    if dataset == 0
        % SegTrack Dataset Explained Variation
        f_segtrack_ev = figure;
        title('Explained Variation', 'fontsize', 16);
        xlabel('supervoxel numbers','fontsize',16);
        ylabel('values','fontsize',16);
        hold all;
        plot(result{1,1}.stat_sorted(1,:), result{1,1}.stat_sorted(2,:), ...
            result{1,2}.stat_sorted(1,:), result{1,2}.stat_sorted(2,:), ...
            result{1,3}.stat_sorted(1,:), result{1,3}.stat_sorted(2,:), ...
            result{1,4}.stat_sorted(1,:), result{1,4}.stat_sorted(2,:), ...
            result{1,5}.stat_sorted(1,:), result{1,5}.stat_sorted(2,:), ...
            result{1,6}.stat_sorted(1,:), result{1,6}.stat_sorted(2,:));
        hleg = legend(result{1,1}.name, result{1,2}.name, result{1,3}.name, ...
            result{1,4}.name, result{1,5}.name, result{1,6}.name, ...
            'Location', 'SouthEast');
        savepath = [output_path, '/', 'segtrack_ev.png'];
        saveas(f_segtrack_ev, savepath, 'png');
        savepath = [output_path, '/', 'segtrack_ev.eps'];
        print('-depsc',f_segtrack_ev,savepath);
    end

    if dataset == 1
        % Chen Dataset Explained Variation
        f_chen_ev = figure;
        title('Explained Variation', 'fontsize', 16);
        xlabel('supervoxel numbers','fontsize',16);
        ylabel('values','fontsize',16);
        hold all;
        plot(result{1,1}.stat_sorted(1,:), result{1,1}.stat_sorted(2,:), ...
            result{1,2}.stat_sorted(1,:), result{1,2}.stat_sorted(2,:), ...
            result{1,3}.stat_sorted(1,:), result{1,3}.stat_sorted(2,:), ...
            result{1,4}.stat_sorted(1,:), result{1,4}.stat_sorted(2,:), ...
            result{1,5}.stat_sorted(1,:), result{1,5}.stat_sorted(2,:), ...
            result{1,6}.stat_sorted(1,:), result{1,6}.stat_sorted(2,:), ...
            result{1,7}.stat_sorted(1,:), result{1,7}.stat_sorted(2,:), ...
            result{1,8}.stat_sorted(1,:), result{1,8}.stat_sorted(2,:));
        hleg = legend(result{1,1}.name, result{1,2}.name, result{1,3}.name, ...
            result{1,4}.name, result{1,5}.name, result{1,6}.name, ...
            result{1,7}.name, result{1,8}.name, ...
            'Location', 'SouthEast');
        savepath = [output_path, '/', 'chen_ev.png'];
        saveas(f_chen_ev, savepath, 'png');
        savepath = [output_path, '/', 'chen_ev.eps'];
        print('-depsc',f_chen_ev,savepath);
    end

    if dataset == 2
        % Gatech Dataset Explained Variation
        f_gatech_ev = figure;
        title('Explained Variation', 'fontsize', 16);
        xlabel('supervoxel numbers','fontsize',16);
        ylabel('values','fontsize',16);
        hold all;
        plot(result{1,1}.stat_sorted(1,:), result{1,1}.stat_sorted(2,:), ...
            result{1,2}.stat_sorted(1,:), result{1,2}.stat_sorted(2,:), ...
            result{1,3}.stat_sorted(1,:), result{1,3}.stat_sorted(2,:), ...
            result{1,4}.stat_sorted(1,:), result{1,4}.stat_sorted(2,:), ...
            result{1,5}.stat_sorted(1,:), result{1,5}.stat_sorted(2,:), ...
            result{1,6}.stat_sorted(1,:), result{1,6}.stat_sorted(2,:), ...
            result{1,7}.stat_sorted(1,:), result{1,7}.stat_sorted(2,:), ...
            result{1,8}.stat_sorted(1,:), result{1,8}.stat_sorted(2,:), ...
            result(1,9).stat_sorted(1,:), result(1,9).stat_sorted(2,:), ...
            result{1,10}.stat_sorted(1,:), result{1,10}.stat_sorted(2,:), ...
            result{1,11}.stat_sorted(1,:), result{1,11}.stat_sorted(2,:), ...
            result{1,12}.stat_sorted(1,:), result{1,12}.stat_sorted(2,:), ...
            result{1,13}.stat_sorted(1,:), result{1,13}.stat_sorted(2,:), ...
            result{1,14}.stat_sorted(1,:), result{1,14}.stat_sorted(2,:), ...
            result{1,15}.stat_sorted(1,:), result{1,15}.stat_sorted(2,:));
        hleg = legend(result{1,1}.name, result{1,2}.name, result{1,3}.name, ...
            result{1,4}.name, result{1,5}.name, result{1,6}.name, ...
            result{1,7}.name, result{1,8}.name, result(1,9).name, ...
            result{1,10}.name, result{1,11}.name, result{1,12}.name, ...
            result{1,13}.name, result{1,14}.name, result{1,15}.name, ...
            'Location', 'EastOutside');
        savepath = [output_path, '/', 'gatech_ev.png'];
        saveas(f_gatech_ev, savepath, 'png');
        savepath = [output_path, '/', 'gatech_ev.eps'];
        print('-depsc',f_gatech_ev,savepath);
    end

end