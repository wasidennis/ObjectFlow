function U = unary_term(HIE, T)

U = zeros(T.sv_num, 1);

for i=1:T.sv_num
    U(i) = HIE(T.Id2Hie(i)).sv_num;
end

U = U./norm(U,1);