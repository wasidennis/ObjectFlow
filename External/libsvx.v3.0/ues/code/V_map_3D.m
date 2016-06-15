function [y, x, t] = V_map_3D(V_num, I_w, I_h)

t = floor((V_num - 1)/(I_w * I_h)) + 1;
y = floor((V_num - 1 - ((t-1) * I_w * I_h)) / I_w) + 1;
x = mod(V_num - 1 - ((t-1) * I_w * I_h), I_w) + 1;
