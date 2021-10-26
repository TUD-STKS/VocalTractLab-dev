load data_simple_cone.mat
load tfMM.txt
load tfMM_o_05_1.txt
load tfFEM.txt

close all

fac = sqrt(pi)/2;
ftSize = 16;

%% plot tf U/U
figure
subplot(2,1,1)
hold on
plot(tfMM(:,1), 20*log10(fac*tfMM(:,2)), 'lineWidth', 2)
plot(tfMM_o_05_1(:,1), 20*log10(fac*tfMM_o_05_1(:,2)), 'lineWidth', 2)
plot(tfFEM(:,1), 20*log10(tfFEM(:,2)), 'lineWidth', 2)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel '|H| (dB)'

h = legend('MM', 'FEM (FeniCS)', 'location', 'southwest');
legend boxoff
set(h, 'fontSize', ftSize) 
set(gca, 'fontSize', ftSize)

subplot(2,1,2)
hold on
plot(tfMM(:,1), tfMM(:,3), 'lineWidth', 2)
plot(tfMM_o_05_1(:,1), tfMM_o_05_1(:,3), 'lineWidth', 2)
plot(tfFEM(:,1), tfFEM(:,3), 'lineWidth', 2)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel 'phase (rad)'

set(gca, 'fontSize', ftSize)

%% plot tf P/U

fac = 10*sqrt(pi)/2;

figure
subplot(2,1,1)
hold on
plot(tfMM(:,1), 20*log10(fac*tfMM(:,4)), 'lineWidth', 2)
plot(tfMM_o_05_1(:,1), 20*log10(fac*tfMM_o_05_1(:,4)), 'lineWidth', 2)
plot(tfFEM(:,1), 20*log10(tfFEM(:,4)), 'lineWidth', 2)

xlim([0 10000])
ylim([0 100])
xlabel 'f (Hz)'
ylabel '|H| (dB)'

subplot(2,1,2)
hold on
plot(tfMM(:,1), tfMM(:,5), 'lineWidth', 2)
plot(tfMM_o_05_1(:,1), tfMM_o_05_1(:,5), 'lineWidth', 2)
plot(tfFEM(:,1), tfFEM(:,5), 'lineWidth', 2)

xlim([0 10000])
xlabel 'f (Hz)'
ylabel 'phase (rad)'
