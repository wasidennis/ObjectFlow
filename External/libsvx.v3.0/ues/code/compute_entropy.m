function E = compute_entropy(HIE, T, energy_bin)
% Compute entropy on each node.

E = zeros(T.sv_num, 1);
for i=1:T.sv_num
    svmap_i = (HIE(T.Id2Hie(i)).sv_map==i); % sv volume
    bins = energy_bin(svmap_i); % energy bin in the sv volume
    bin_ids = unique(bins);
    bin_counts = zeros(length(bin_ids),1);
    for k=1:length(bin_ids)
        bin_counts(k) = length(find(bins==bin_ids(k)));
    end
    bin_p = bin_counts./sum(bin_counts);
    E(i) = abs(sum(log(bin_p).*bin_p));
end
% figure;
% bar(E);