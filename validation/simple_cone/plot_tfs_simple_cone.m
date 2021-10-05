load data_simple_cone.mat

fac = sqrt(pi)/2;
ftSize = 16;

figure
subplot(2,1,1)
plot(tfMM(:,1), 20*log10(fac*tfMM(:,2)), 'lineWidth', 2)
hold on
plot(tfFEM(:,1), 20*log10(tfFEM(:,2)), 'lineWidth', 2)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel '|H| (dB)'

h = legend('MM', 'FEM (FeniCS)', 'location', 'southwest')
legend boxoff
set(h, 'fontSize', ftSize) 
set(gca, 'fontSize', ftSize)

subplot(2,1,2)
plot(tfMM(:,1), tfMM(:,3), 'lineWidth', 2)
hold on
plot(tfFEM(:,1), tfFEM(:,3), 'lineWidth', 2)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel '|H| (dB)'

set(gca, 'fontSize', ftSize)
