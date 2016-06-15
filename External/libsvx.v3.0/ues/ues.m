function ues(video_path, hie_path, output_path, hie_select_num, sigma, method, visflag)
% This is the main function to perform UES algorithm.
% Author: Chenliang Xu (chenlian@buffalo.edu)
% Date: Nov. 2013

settings; % load settings
tic;
if ~exist(output_path, 'dir')
    mkdir(output_path); % make output directory
end

%% Step 1: Construct Supervoxel Segmentation Tree
disp('---- Constructing Supervoxel Segmentation Tree ...');
[HIE, T] = select_segmentaion_hierarchy(hie_path, hie_select_num);
save(fullfile(output_path,'hierarchy.mat'), 'HIE', 'T');

%% Step 2: Compute Entropy Field
disp('---- Computing Entropy Field ...');
switch method
    case 'motion'
        flow = compute_optflow(video_path, paths);
        save(fullfile(output_path,'flow.mat'), 'flow');
        if visflag == 1
            draw_video_flow(flow, output_path, paths);
        end
        flow_bin = read_motion_bin(flow, mag_num, ang_num);
        E = compute_entropy(HIE, T, flow_bin);
    case 'objectness'
        obj = compute_objness(video_path, win_num, paths);
        save(fullfile(output_path,'obj.mat'), 'obj');
        if visflag == 1
            draw_video_objness(obj, output_path);
        end
        obj_bin = read_objness_bin(obj, obj_num);
        E = compute_entropy(HIE, T, obj_bin);
    case 'detection'
        det = compute_detection(video_path, model_name, model_thresh, paths);
        save(fullfile(output_path,'det.mat'), 'det');
        if visflag == 1
            draw_video_detection(det, output_path);
        end
        det_bin = read_detection_bin(det, det_num);
        E = compute_entropy(HIE, T, det_bin);
    otherwise
        error('Unknown feature criteria method!');
end


%% Step 3: Build Unary and Binary Term
disp('---- Building Unary ...');
U = unary_term(HIE, T);
disp('---- Building Binary ...');
B = binary_term(HIE, T, E);

%% Step 4: Mixed Integer Quadratic Programming (MIQP)
disp('---- Solving MIQP ...');
message = sprintf('    computing time bounded in %d seconds', opt.MaxTime);
disp(message);
[S, L, lb, ub, ctype] = build_constraint(HIE, T);
[x, fval, exitflag, output] = cplexmiqp(B.*sigma, U, ...
    [], [], S, L, [], [], [], lb, ub, ctype, [], opt);
% Results Verify
if exitflag <= 0
    disp('    Results not converge ...');
    x = result_verify(x, HIE, T);
    disp('    Manually assign a solution!')
    disp('    Try a longer computing time!')
else
    disp('    Converge to a solution!');
end

%% Step 5: Save Results
save(fullfile(output_path,'result.mat'), 'x');
message = sprintf('    Total output supervoxel number: %d', sum(x));
disp(message);
if visflag == 1
    draw_hie_hist(x, HIE, output_path);
    draw_hie_images(x, HIE, T, output_path);
    draw_result_entropy(x, HIE, T, E, output_path);
end
draw_images(x, HIE, T, output_path);
toc;
