function B = binary_term(HIE, T, E)

%% Build Binary Relationships
B = zeros(T.sv_num, T.sv_num);
for i=1:length(HIE)
    for k=HIE(i).sv_start:HIE(i).sv_end
        svmap_k = (HIE(T.Id2Hie(k)).sv_map==k);
        svmap_k_b = imdilate(svmap_k, ones(3,3,3))&(~svmap_k);
        for j=i:length(HIE)
            neighbors = unique(HIE(j).sv_map(svmap_k_b));
            B(k, neighbors) = 1;
        end
    end
end
for i=1:T.sv_num
    for j=1:T.sv_num
        if B(i,j) == 1
            B(j,i) = 1;
        end
    end
end

%% Fill with weight
for i=1:T.sv_num
    for j=1:T.sv_num
        if B(i,j) == 1
            B(i,j) = abs(E(i)-E(j))*T.Id2Size(i)*T.Id2Size(j);
            B(j,i) = B(i,j);
        end
    end
end

B = B./(sum(sum(B))/2);