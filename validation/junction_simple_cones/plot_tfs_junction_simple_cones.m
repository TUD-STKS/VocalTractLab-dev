% load data_junction_simple_cones.mat
%load tfMM_2_1_1_05.txt;
%tf1 = tfMM_2_1_1_05;
%load tfMM_2_1_2_1.txt;
%tf2 = tfMM_2_1_2_1;
%load tfMM_1_05_1_05.txt;
%tf3 = tfMM_1_05_1_05;
% load tfMM_1_05_2_1.txt;
% tf4 = tfMM_1_05_2_1;
%load tfFEM.txt;
%tf3 = tfFEM;
load tfMM.txt;
load tfFEM.txt
load press.txt;

ftSize = 16;
lSize = 1;

%% plot V/V tf

fac = 1;

figure
subplot(2,1,1)
hold on
plot(tfFEM(:,1), 20*log10(tfFEM(:,2)), 'lineWidth', lSize)
plot(tfMM(:,1), 20*log10(fac*tfMM(:,2)), 'lineWidth', lSize)
%plot(tf1(:,1), 20*log10(tf1(:,2)), 'lineWidth', lSize)
%plot(tf2(:,1), 20*log10(tf2(:,2)), 'lineWidth', lSize)
%plot(tf3(:,1), 20*log10(tf3(:,2)), 'lineWidth', lSize)
% plot(tf4(:,1), 20*log10(2*fac*tf4(:,2)), 'lineWidth', lSize)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel '|H| (dB)'

h = legend('FEM (FeniCS)', 'MM', 'location', 'northeast',...
  'orientation', 'horizontal');
legend boxoff
set(h, 'fontSize', ftSize) 
set(gca, 'fontSize', ftSize)

subplot(2,1,2)
hold on
plot(tfFEM(:,1), tfFEM(:,3), 'lineWidth', lSize)
plot(tfMM(:,1), tfMM(:,3), 'lineWidth', lSize)
%plot(tf1(:,1), tf1(:,3), 'lineWidth', lSize)
%plot(tf2(:,1), tf2(:,3), 'lineWidth', lSize)
%plot(tf3(:,1), tf3(:,3), 'lineWidth', lSize)
% plot(tf4(:,1), tf4(:,3), 'lineWidth', lSize)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel '|H| (dB)'

% h = legend('|2 | 1 | 1 | 0.5|', '|2 | 1 | 2 | 1|', '|1 | 0.5 | 1 | 0.5|', '|1 | 0.5 | 2 | 1|',...
%   'location', 'northoutside', 'orientation', 'horizontal')
% legend boxoff
% set(h, 'fontSize', ftSize) 
set(gca, 'fontSize', ftSize)

%% plot P/V tf

fac = 10;

figure

subplot(2,1,1)
hold on
plot(tfFEM(:,1), 20*log10(tfFEM(:,4)), 'lineWidth', lSize)
plot(tfMM(:,1), 20*log10(fac*tfMM(:,4)), 'lineWidth', lSize)
plot(press(:,1), 20*log10((16/9/sqrt(pi)/1.5)*fac*press(:,6)), 'lineWidth', lSize)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel '|H| (dB)'

h = legend('FEM (FeniCS)', 'MM', 'location', 'southeast',...
  'orientation', 'horizontal');
legend boxoff
set(h, 'fontSize', ftSize) 

subplot(2,1,2)
hold on
plot(tfFEM(:,1), tfFEM(:,5), 'lineWidth', lSize)
plot(tfMM(:,1), tfMM(:,5), 'lineWidth', lSize)
plot(press(:,1), press(:,7), 'lineWidth', lSize)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel '|H| (dB)'
