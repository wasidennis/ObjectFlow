function x = result_verify(x, HIE, T)

x(x<1) = 0;
x(x>1) = 1;

for i=HIE(end).sv_start:HIE(end).sv_end
    ancestors = T.Id2Anc{i}(1:end-1);
    if sum(x(ancestors)) + x(i) > 1
        disp('hard work!');
        x(ancestors) = 0;
        x(i) = 1;
    end
    if sum(x(ancestors)) + x(i) == 0
        % manual assign
        disp('manual assign!');
        x(i) = 1;
    end
end


% double check
disp('double check!');
for i=HIE(end).sv_start:HIE(end).sv_end
    ancestors = T.Id2Anc{i}(1:end-1);
    if sum(x(ancestors)) + x(i) > 1
        disp('hard work!');
        x(ancestors) = 0;
        x(i) = 1;
    end
    if sum(x(ancestors)) + x(i) == 0
        % manual assign
        disp('manual assign!');
        x(i) = 1;
    end
end