load data_simple_cone.mat
load tfMM.txt
load tfMM_05_1.txt
load tfMM_1_2.txt
load tfMM_05_1_nol.txt
load tfMM_1_2_nol.txt
load tfMM_05_1_xl0.txt
load tfMM_1_2_xl0.txt
load tfMM_05_1_l2.txt
load tfMM_1_2_l2.txt
load tfMM_05_1_a.txt
load tfMM_1_2_a.txt
load tfMM3.txt
load tfFEM.txt

% close all

ftSize = 16;
lWidth = 1;

% %% Effect of factor l with different scaling
% 
% figure
% subplot(2,1,1)
% hold on
% plot(tfMM_05_1_nol(:,1), 20*log10(fac*tfMM_05_1_nol(:,2)), 'lineWidth', lWidth)
% plot(tfMM_1_2_nol(:,1), 20*log10(0.5*fac*tfMM_1_2_nol(:,2)), 'lineWidth', lWidth)
% 
% xlim([0 10000])
% xlabel 'f (Hz)'
% ylabel '|H| (dB)'
% 
% h = legend('0.5 1', '1 2', 'location', 'southwest');
% legend boxoff
% set(h, 'fontSize', ftSize) 
% set(gca, 'fontSize', ftSize)
% 
% title('No factor l')
% 
% subplot(2,1,2)
% hold on
% plot(tfMM_05_1(:,1), 20*log10(fac*tfMM_05_1(:,2)), 'lineWidth', lWidth*2)
% plot(tfMM_1_2(:,1), 20*log10(0.5*fac*tfMM_1_2(:,2)), 'lineWidth', lWidth)
% xlim([0 10000])
% xlabel 'f (Hz)'
% ylabel '|H| (dB)'
% 
% h = legend('0.5 1', '1 2', 'location', 'southwest');
% legend boxoff
% set(h, 'fontSize', ftSize) 
% set(gca, 'fontSize', ftSize)
% 
% title('With factor l')

%% plot tf U/U
fac = 1;
figure
subplot(2,1,1)
hold on
plot(tfMM(:,1), 20*log10(fac*tfMM(:,2)), 'lineWidth', lWidth)
% plot(tfMM_05_1(:,1), 20*log10(fac*tfMM_05_1(:,2)), 'lineWidth', lWidth)
% plot(tfMM_05_1(:,1), 20*log10(fac*tfMM_05_1_l2(:,2)), 'lineWidth', lWidth)
% plot(tfMM_05_1_xl0(:,1), 20*log10(2*fac*tfMM_05_1_xl0(:,2)), 'lineWidth', lWidth)
% plot(tfMM_1_2(:,1), 20*log10(fac*tfMM_1_2(:,2)), 'lineWidth', lWidth*2)
% plot(tfMM_1_2_xl0(:,1), 20*log10(fac*tfMM_1_2_xl0(:,2)), 'lineWidth', lWidth)
% plot(tfMM_1_2_l2(:,1), 20*log10(fac*tfMM_1_2_l2(:,2)), 'lineWidth', lWidth)
% plot(tfMM3(:,1), 20*log10(fac*tfMM3(:,2)), 'lineWidth', lWidth)
% plot(tfMM_05_1_a(:,1), 20*log10(fac*tfMM_05_1_a(:,2)), 'lineWidth', lWidth)
% plot(tfMM_1_2_a(:,1), 20*log10(fac*tfMM_1_2_a(:,2)), 'lineWidth', lWidth)
plot(tfFEM(:,1), 20*log10(tfFEM(:,2)), 'lineWidth', lWidth)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel '|H| (dB)'

h = legend('MM', 'FEM (FeniCS)', 'location', 'southwest');
legend boxoff
set(h, 'fontSize', ftSize) 
set(gca, 'fontSize', ftSize)

subplot(2,1,2)
hold on
% plot(tfMM(:,1), tfMM(:,3), 'lineWidth', lWidth)
plot(tfMM_05_1(:,1), tfMM_05_1(:,3), 'lineWidth', lWidth)
plot(tfMM_05_1_xl0(:,1), tfMM_05_1_xl0(:,3), 'lineWidth', lWidth)
plot(tfFEM(:,1), tfFEM(:,3), 'lineWidth', lWidth)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel 'phase (rad)'

set(gca, 'fontSize', ftSize)

%% plot tf P/U

fac = 10;

figure
subplot(2,1,1)
hold on
plot(tfMM(:,1), 20*log10(fac*tfMM(:,4)), 'lineWidth', lWidth)
% plot(tfMM_05_1(:,1), 20*log10(fac*tfMM_05_1(:,4)), 'lineWidth', lWidth)
% plot(tfMM_05_1(:,1), 20*log10(fac*tfMM_05_1_l2(:,4)), 'lineWidth', lWidth)
% plot(tfMM_05_1_xl0(:,1), 20*log10(2*fac*tfMM_05_1_xl0(:,4)), 'lineWidth', lWidth)
% plot(tfMM_1_2(:,1), 20*log10(fac*tfMM_1_2(:,4)), 'lineWidth', lWidth*2)
% plot(tfMM_1_2_xl0(:,1), 20*log10(fac*tfMM_1_2_xl0(:,4)), 'lineWidth', lWidth)
% plot(tfMM_1_2_l2(:,1), 20*log10(fac*tfMM_1_2_l2(:,4)), 'lineWidth', lWidth)
% plot(tfMM3(:,1), 20*log10((16/9)*fac*tfMM3(:,4)), 'lineWidth', lWidth)
% plot(tfMM_05_1_a(:,1), 20*log10(fac*tfMM_05_1_a(:,4)), 'lineWidth', lWidth)
% plot(tfMM_1_2_a(:,1), 20*log10(fac*tfMM_1_2_a(:,4)), 'lineWidth', lWidth)
plot(tfFEM(:,1), 20*log10(tfFEM(:,4)), 'lineWidth', lWidth)

xlim([0 10000])
% ylim([0 100])
xlabel 'f (Hz)'
ylabel '|H| (dB)'

subplot(2,1,2)
hold on
% plot(tfMM(:,1), tfMM(:,5), 'lineWidth', lWidth)
plot(tfMM_05_1(:,1), tfMM_05_1(:,5), 'lineWidth', lWidth)
plot(tfMM_05_1_xl0(:,1), tfMM_05_1_xl0(:,5), 'lineWidth', lWidth)
plot(tfFEM(:,1), tfFEM(:,5), 'lineWidth', lWidth)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel 'phase (rad)'
