function draw_hie_hist(x, HIE, output_path)

hist_hie = zeros(1,length(HIE));
for i=1:length(HIE)
    hist_hie(i) = sum(x(HIE(i).sv_start:HIE(i).sv_end));
end
figure;bar(hist_hie);
savepath = [output_path,'/hie_hist.png'];
saveas(gcf, savepath);