% compare FEM and MM for the elephant trunk test case

load elephant_radbig_coarse_out_3D.mat
Hrb = H;
load elephant_rad_out_3D.mat
load elephant_ac_press_MM.txt

% conversion factor from CGS to MKSA
input_area = pi*0.75*0.75;
l = 0.25;
% fac = (10^5)/2/pi;
fac = (10^5)/(sqrt(input_area)/l);

figure
hold on
plot(f, 20*log10(abs(Hrb)))
plot(f, 20*log10(abs(H)))
plot(elephant_ac_press_MM(:,1), 20*log10(fac*elephant_ac_press_MM(:,6)))
xlim([0 10000])
ylim([80 120])
xlabel('f (Hz)')
ylabel('|H| (dB)')
legend('FEM rad Big', 'FEM rad small', 'MM')

%% zero pressure at the opening

load elephant_p0_out_3D.mat
load elephant_ac_press_MM_p0.txt
load elephant_ac_press_MM_p0_100.txt
load elephant_ac_press_MM_p0_1000.txt
load elephant_ac_press_MM_np25.txt
load elephant_ac_press_MM_np100.txt
load elephant_ac_press_MM_mn1.txt
load elephant_ac_press_MM_mn17.txt
load elephant_ac_press_MM_ang_loss.txt
load elephant_ac_press_MM_ang_loss2.txt


% fac = (10^5)/(10^(27/20));
fac = (10^5)/(sqrt(input_area)/l/l);

figure
hold on
plot(f, 20*log10(abs(H)))
plot(f, 20*log10(abs(Hrb)))
plot(elephant_ac_press_MM(:,1), 20*log10(fac*elephant_ac_press_MM(:,6)))
% plot(elephant_ac_press_MM_p0(:,1), 20*log10(fac*elephant_ac_press_MM_p0(:,6)))
plot(elephant_ac_press_MM_mn17(:,1), 20*log10(fac*elephant_ac_press_MM_mn17(:,6)))
plot(elephant_ac_press_MM_ang_loss2(:,1), 20*log10(fac*elephant_ac_press_MM_ang_loss2(:,6)))
xlim([0 10000])
ylim([60 130])
xlabel('f (Hz)')
ylabel('|H| (dB)')
% legend('FEM', 'MM Y = 1000000', 'MM 17 modes')