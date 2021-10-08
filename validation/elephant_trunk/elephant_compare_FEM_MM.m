% compare FEM and MM for the elephant trunk test case

ftSize = 14;

load elephant_radbig_coarse_out_3D.mat
Hrb = H;
load elephant_rad_out_3D.mat
load elephant_ac_press_MM.txt

% conversion factor from CGS to MKSA
input_area = pi*0.75*0.75;
norm_area = pi*3*3;
l = 0.25;
fac = (10^5)/2/pi;

figure
hold on
plot(f, 20*log10(abs(Hrb)))
plot(f, 20*log10(abs(H)))
plot(elephant_ac_press_MM(:,1), 20*log10(fac*elephant_ac_press_MM(:,2)))
xlim([0 10000])
ylim([80 120])
xlabel('f (Hz)')
ylabel('|H| (dB)')
legend('FEM rad Big', 'FEM rad small', 'MM', 'location', 'southeast')


%% zero pressure at the opening

load elephant_p0_out_3D.mat
Hp0_orig = H;
load elephant_p0_stabilized_out_3D.mat
load elephant_ac_press_MM_p0.txt

fac = input_area*(10^5);

figure
hold on
plot(f, 20*log10(abs(Hp0_orig)), 'k', 'linewidth', 2)
plot(f, 20*log10(abs(H)), 'r', 'linewidth', 2)
plot(elephant_ac_press_MM_p0(:,1), 20*log10(fac*elephant_ac_press_MM_p0(:,2)), 'b', 'linewidth', 2)

xlim([0 10000])
ylim([70 140])
xlabel('f (Hz)')
ylabel('|H| (dB)')
h = legend('FEM original','FEM stabilized', 'MM', 'location', 'southeast');
set(h, 'fontsize', ftSize);
set(gca, 'fontsize', ftSize);
