function draw_hie_images(x, HIE, T, output_path)

[V_h, V_w, V_f] = size(HIE(1).sv_map);
hie_video = zeros(V_h, V_w, V_f);

for i=1:length(x)
    if x(i)==1
        svmap_i = (HIE(T.Id2Hie(i)).sv_map==i);
        hie_video(svmap_i) = T.Id2Hie(i);
    end
end

if ~exist([output_path,'/select'], 'dir')
    mkdir([output_path,'/select']);
end

for i=1:V_f
    imagesc(hie_video(:,:,i));
    axis image
    axis off
    caxis([1 length(HIE)])
    savepath = sprintf('%s/select/%05d.png', output_path, i);
    
    saveas(gcf, savepath);
    
    % set(gca, 'Units', 'normalized', 'Position', [0 0 1 1]);
    % set(gcf,'Units','pixels','Position',[200 200 V_w V_h]);
    % f = getframe(gcf);
    % imwrite(f.cdata, savepath);
    % close(gcf);
end