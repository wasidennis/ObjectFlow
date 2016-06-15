function loss = loss_euclidean(pred, output)
loss = sum((pred(:) - output(:)).^2)/2;
% loss = loss / size(pred,4);
