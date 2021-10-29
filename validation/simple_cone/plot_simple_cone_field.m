load p.txt
load extracted_field/p_real_centerline_2500.csv
[xr, idxr] = sort(p_real_centerline_2500(:,2));
load extracted_field/p_imag_centerline_2500.csv 
[xi, idxi] = sort(p_imag_centerline_2500(:,2));
p_fem = p_real_centerline_2500(idxr, 1) + 1i*p_imag_centerline_2500(idxi, 1);
x_fem = xi;

x = repmat(8.5*(0:99)/99, 51, 1);
y = (((1 + (0:99)/99)')*(2*(0:50)/50 - 1))';
l = repmat(0.5 + 0.5*(0:99)/99, 51, 1);

%fac = 10*sqrt(pi)/2;
%fac = 0.96*10*sqrt(pi)/2;
% fac = 10^0.93;
fac = 10;

figure
surf(x, y, p')

figure
plot(x_fem, log10(abs(p_fem)))
hold on
plot(x(25, :)/100, log10(fac*p(:, 25))')
plot(x(25, :)/100, log10(p(:, 25))')

figure
plot(x_fem, abs(p_fem))
hold on
plot(x(25, :)/100, fac*p(:, 25)')
plot(x(25, :)/100, p(:, 25)')
