load tfMM.txt;
tf2 = tfMM;
%load tfFEM.txt;
%tf3 = tfFEM;
load data_junction_simple_cones_reversed.mat
load press.txt

fac = sqrt(pi)/8;
ftSize = 16;

figure
subplot(2,1,1)
% plot(tfMM(:,1), 20*log10(fac*tfMM(:,2)), 'lineWidth', 2)
hold on
plot(tf2(:,1), 20*log10(tf2(:,2)), 'lineWidth', 2)
plot(tfFEM(:,1), 20*log10(tfFEM(:,2)), 'lineWidth', 2)
%plot(tf3(:,1), 20*log10(tf3(:,2)), 'lineWidth', 2)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel '|H| (dB)'

h = legend('MM', 'FEM (FeniCS)', 'location', 'southwest',...
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
ylabel 'angle (rad)'

set(gca, 'fontSize', ftSize)

%% Plot P/U tf

figure

subplot(2,1,1)
hold on
plot(tf2(:,1), 20*log10(tf2(:,4)))
plot(press(:,1), 20*log10((4/9/sqrt(pi)/1.5)*press(:,6)))
% plot(press(:,1), 20*log10(press(:,6)))

xlim([0 10000])
xlabel 'f (Hz)'
ylabel '|H| (dB)'

subplot(2,1,2)
hold on
plot(tf2(:,1), tf2(:,5))
plot(press(:,1), press(:,7))

xlim([0 10000])
xlabel 'f (Hz)'
ylabel 'angle (rad)'
