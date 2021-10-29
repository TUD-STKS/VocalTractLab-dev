% load MM simulation
load p.txt
load q.txt
load Y.txt

% load FEM simulation
load extracted_field\p_center_line_real_2500.csv
[xr, idxr] = sort(p_center_line_real_2500(:,2));
pre = p_center_line_real_2500(idxr, 1);
[xr, idxr] = unique(xr);
pre = pre(idxr);
load extracted_field\p_center_line_imag_2500.csv
[xi, idxi] = sort(p_center_line_imag_2500(:,2));
pim = p_center_line_imag_2500(idxi, 1);
[xi, idxi] = unique(xi);
pim = pim(idxi);
p_fem = pre + 1i*pim;
x_fem = xi;


x = [repmat(8.5*(0:99)/99, 51, 1) repmat(8.5 + 8.5*(0:99)/99, 51, 1)];
y = [(((2 - (0:99)/99)')*(2*(0:50)/50 - 1))' (((2 - (0:99)/99)')*(2*(0:50)/50 - 1))'];

figure,
surf(x, y, p')

figure
hold on
plot(x_fem, abs(p_fem))
plot(x(25,:)/100, 10*p(:,25)')