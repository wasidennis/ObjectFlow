function Y = cvlbmap(X)
% convert labels in X to 1:N

u = unique(X);
SparseIndex = sparse(u,1,1:length(u),max(u),1);
Y = full(SparseIndex(X(:)));
Y = reshape(Y,size(X));