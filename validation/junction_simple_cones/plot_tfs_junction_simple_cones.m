%load tfMM_2_1_1_05.txt;
%tf2 = tfMM_2_1_1_05;
%load tfFEM.txt;
%tf3 = tfFEM;
load data_junction_simple_cones.mat

fac = sqrt(pi)*4;
ftSize = 16;

figure
subplot(2,1,1)
plot(tfMM(:,1), 20*log10(fac*tfMM(:,2)), 'lineWidth', 2)
hold on
%plot(tf2(:,1), 20*log10(4*fac*tf2(:,2)), 'lineWidth', 2)
plot(tfFEM(:,1), 20*log10(tfFEM(:,2)), 'lineWidth', 2)
%plot(tf3(:,1), 20*log10(tf3(:,2)), 'lineWidth', 2)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel '|H| (dB)'

h = legend('MM', 'FEM (FeniCS)', 'location', 'northeast',...
  'orientation', 'horizontal')
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

