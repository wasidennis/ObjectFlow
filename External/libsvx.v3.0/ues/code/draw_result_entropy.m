function draw_result_entropy(x, HIE, T, E, output_path)

[V_h, V_w, V_f] = size(HIE(1).sv_map);
entropy_video = zeros(V_h, V_w, V_f);

for i=1:length(x)
    if x(i) == 1
        svmap_i = (HIE(T.Id2Hie(i)).sv_map==i);
        entropy_video(svmap_i) = E(i);
    end
end

for k=1:length(HIE)
    entropy_video_F{k} = zeros(V_h, V_w, V_f);
    for i=HIE(k).sv_start:HIE(k).sv_end
        svmap_i = (HIE(k).sv_map==i);
        entropy_video_F{k}(svmap_i) = E(i);
    end
end

if ~exist([output_path,'/entropy'], 'dir')
    mkdir([output_path,'/entropy']);
end

for i=1:V_f
    % pause(0.5);
    entropy_draw = [entropy_video(:,:,i)];
    for k=1:length(HIE)
        entropy_draw = [entropy_draw, entropy_video_F{k}(:,:,i)];
    end
    imagesc(entropy_draw)
    axis image
    colorbar

    saveas(gcf, sprintf('%s/entropy/%05d.png',output_path,i))
end
