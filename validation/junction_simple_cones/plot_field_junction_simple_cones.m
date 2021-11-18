% load MM simulation
load p.txt
load q.txt
load Y.txt
load cx.txt
load cy.txt
load field.txt
fx = linspace(-5, 10, size(field,2));
fy = linspace(-10, 5, size(field, 1));

% load FEM simulation
load extracted_field/p_center_line_real_2500.csv
[xr, idxr] = sort(p_center_line_real_2500(:,2));
pre = p_center_line_real_2500(idxr, 1);
[xr, idxr] = unique(xr);
pre = pre(idxr);
load extracted_field/p_center_line_imag_2500.csv
[xi, idxi] = sort(p_center_line_imag_2500(:,2));
pim = p_center_line_imag_2500(idxi, 1);
[xi, idxi] = unique(xi);
pim = pim(idxi);
p_fem = pre + 1i*pim;
x_fem = xi;

figure,
surf(cx, cy, log10(p))
axis equal
shading interp

figure, 
surf(fx, fy, log10(field))
axis equal
shading interp

figure
hold on
plot(x_fem, abs(p_fem))
plot(cx(:,25)/100, 10*p(:,25))
plot((fx+4)/100, (16/9/sqrt(pi)/1.5)*10*field(300,:))
