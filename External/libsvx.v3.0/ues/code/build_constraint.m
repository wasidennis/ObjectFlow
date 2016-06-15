function [S, L, lb, ub, ctype] = build_constraint(HIE, T)

S = zeros(HIE(end).sv_num, T.sv_num);
for i=1:HIE(end).sv_num
    S(i,T.Id2Anc{HIE(end).sv_start+i-1}(1:end-1)) = 1;
    S(i,HIE(end).sv_start+i-1) = 1;
end

L = ones(HIE(end).sv_num,1);

lb = zeros(T.sv_num,1);
ub = ones(T.sv_num,1);

for i=1:T.sv_num
    ctype(i) = 'B';
end