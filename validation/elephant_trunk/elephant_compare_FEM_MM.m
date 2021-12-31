% compare FEM and MM for the elephant trunk test case

ftSize = 10;
lWidth = 1;

load elephant_radbig_coarse_out_3D.mat
Hrb = H;
load elephant_rad_out_3D.mat
load elephant_ac_press_MM.txt

% conversion factor from CGS to MKSA
input_area = pi*0.75*0.75;
norm_area = pi*3*3;
l = 0.25;

% figure
% subplot(2,1,1)
% hold on
% plot(f, 20*log10(abs(Hrb)), 'linewidth', lWidth)
% plot(f, 20*log10(abs(H)), 'linewidth', lWidth)
% plot(elephant_ac_press_MM(:,1), 20*log10(elephant_ac_press_MM(:,2)), 'linewidth', lWidth)
% xlim([0 10000])
% ylim([80 120])
% xlabel('f (Hz)')
% ylabel('|H| (dB)')
% h = legend('FEM rad Big', 'FEM rad small', 'MM', 'location', 'southeast');
% set(h, 'fontsize', ftSize);
% set(gca, 'fontsize', ftSize);
% 
% subplot(2,1,2)
% hold on
% plot(f, unwrap(-angle(Hrb)), 'linewidth', lWidth)
% plot(f, unwrap(-angle(H)), 'linewidth', lWidth)
% plot(elephant_ac_press_MM(:,1), unwrap(elephant_ac_press_MM(:,3)), 'linewidth', lWidth)
% xlim([0 10000])
% xlabel('f (Hz)')
% ylabel('phase (deg)')
% 
% set(gca, 'fontsize', ftSize);


%% zero pressure at the opening

load elephant_p0_out_3D.mat
Hp0_orig = H;
load elephant_p0_stabilized_out_3D.mat
load elephant_ac_press_MM_p0.txt

% effect of number of modes
% param = [6 17 32 53];
% paramName = 'num modes';
% load elephant_ac_press_MM_p0_mn_6_nPt_200_d_30.txt
% p1 = elephant_ac_press_MM_p0_mn_6_nPt_200_d_30;
% load elephant_ac_press_MM_p0_mn_17_nPt_200_d_30.txt
% p2 = elephant_ac_press_MM_p0_mn_17_nPt_200_d_30;
% load elephant_ac_press_MM_p0_mn_32_nPt_200_d_30.txt
% p3 = elephant_ac_press_MM_p0_mn_32_nPt_200_d_30;
% load elephant_ac_press_MM_p0_mn_53_nPt_200_d_30.txt
% p4 = elephant_ac_press_MM_p0_mn_53_nPt_200_d_30;

% % effect of mesh density
% param = [10 20 30];
% paramName = 'mesh density';
% load elephant_ac_press_MM_p0_mn_53_nPt_200_d_10.txt
% p1 = elephant_ac_press_MM_p0_mn_53_nPt_200_d_10;
% load elephant_ac_press_MM_p0_mn_53_nPt_200_d_20.txt
% p2 = elephant_ac_press_MM_p0_mn_53_nPt_200_d_20;
% load elephant_ac_press_MM_p0_mn_53_nPt_200_d_30.txt
% p3 = elephant_ac_press_MM_p0_mn_53_nPt_200_d_30;

% effect of number of points for the Magnus scheme
param = [25 50 100 200];
paramName = 'num int pts';
load elephant_ac_press_MM_p0_mn_53_nPt_25_d_30.txt
p1 = elephant_ac_press_MM_p0_mn_53_nPt_25_d_30;
load elephant_ac_press_MM_p0_mn_53_nPt_50_d_30.txt
p2 = elephant_ac_press_MM_p0_mn_53_nPt_50_d_30;
load elephant_ac_press_MM_p0_mn_53_nPt_100_d_30.txt
p3 = elephant_ac_press_MM_p0_mn_53_nPt_100_d_30;
load elephant_ac_press_MM_p0_mn_53_nPt_200_d_30.txt
p4 = elephant_ac_press_MM_p0_mn_53_nPt_200_d_30;

fac = input_area*(10^5);

h = figure('position', [680   751   518   227]);
% subplot(2,1,1)
hold on
% plot(f, 20*log10(abs(Hp0_orig)), 'k', 'linewidth', lWidth)
HFEM = 20*log10(abs(H(f <= 10000)));%-0.38;
f = f(f <= 10000);
plot(f/1000, HFEM, 'linewidth', lWidth)
% plot(elephant_ac_press_MM_p0(:,1), 20*log10(elephant_ac_press_MM_p0(:,2)), 'b', 'linewidth', lWidth)
resFEM = extractResonances(HFEM);
H1 = 20*log10(abs(interp1(p1(:,1), p1(:,2), f, 'spline')));
plot(f/1000, H1)
res1 = extractResonances(H1);
H2 = 20*log10(abs(interp1(p2(:,1), p2(:,2), f, 'spline')));
plot(f/1000, H2)
res2 = extractResonances(H2);
H3 = 20*log10(abs(interp1(p3(:,1), p3(:,2), f, 'spline')));
plot(f/1000, H3)
res3 = extractResonances(H3);
H4 = 20*log10(abs(interp1(p4(:,1), p4(:,2), f, 'spline')));
plot(f/1000, H4)
res4 = extractResonances(H4);

% plot(f(res1(:,1)), H1(res1(:,1)),'g.')
% plot(f(res1(:,2)), H1(res1(:,2)),'r.')
% plot(f(res1(:,3)), H1(res1(:,3)),'g.')

xlim([0 10])
ylim([70 140])
xlabel('f (kHz)')
ylabel('|H| (dB)')
grid on

h = legend('FEM', 'MM', 'location', 'southeast', 'orientation', 'horizontal');
% h = legend('FEM', 'MM d = 10', 'MM d = 20', 'MM d = 30', 'location', 'southeast', 'orientation', 'horizontal');
% h = legend('FEM', 'MM nPt 25', 'MM nPt 50', 'MM nPt 100', 'MM nPt 200', 'location', 'southeast', 'orientation', 'horizontal');
set(h, 'fontsize', ftSize);
legend boxoff
set(gca, 'fontsize', ftSize);

% subplot(2,1,2)
% hold on
% % plot(f, -angle(Hp0_orig), 'k', 'linewidth', lWidth)
% plot(f, -angle(H), 'r', 'linewidth', lWidth)
% plot(elephant_ac_press_MM_p0(:,1), elephant_ac_press_MM_p0(:,3), 'b', 'linewidth', lWidth)
% 
% xlim([0 10000])
% xlabel('f (Hz)')
% ylabel('phase (deg)')
% set(gca, 'fontsize', ftSize);

%% Export resonances in csv file
csvFile = 'resonances.csv';
fid = fopen(csvFile, 'w');
sep = ';';

% generate header
header = sprintf('%s%s', paramName, sep);
nRes = 0;
for n = 1:length(param)
    eval(sprintf('nRes = max(nRes, length(res%d));', n));
end
for n = 1:nRes
    header =  [header sprintf('R%d%s', n, sep)];
end
header = [header '\n'];

% write resonance frequencies
fprintf(fid, 'Frequencies%s\n', sep);
fprintf(fid, header);
for n = 1:length(param)
    fprintf(fid, '%d%s', param(n), sep);
    eval(sprintf('res = res%d;', n));
    
    eval(sprintf('nRes = length(res%d);', n));
    for ii = 1:nRes
        fprintf(fid, '%d%s', res(ii, 2), sep);
    end
    fprintf(fid, '\n');
end
% add FEM data
fprintf(fid, 'FEM%s', sep);
for ii = 1 : length(resFEM)
    fprintf(fid, '%d%s', resFEM(ii, 2), sep);
end
fprintf(fid, '\n');
fprintf(fid, '\n');

% write bandwidth
fprintf(fid, '-3dB Bandwidths%s\n', sep);
fprintf(fid, header);
for n = 1:length(param)
    fprintf(fid, '%d%s', param(n), sep);
    eval(sprintf('res = res%d;', n));
    
    eval(sprintf('nRes = length(res%d);', n));
    for ii = 1:nRes
        fprintf(fid, '%d%s', res(ii, 3) - res(ii, 1), sep);
    end
    fprintf(fid, '\n');
end
% add FEM data
fprintf(fid, 'FEM%s', sep);
for ii = 1 : length(resFEM)
    fprintf(fid, '%d%s', resFEM(ii, 3)-resFEM(ii, 1), sep);
end
fprintf(fid, '\n');
fprintf(fid, '\n');

% write amplitude
fprintf(fid, 'Amplitude (dB)%s\n', sep);
fprintf(fid, header);
for n = 1:length(param)
    fprintf(fid, '%d%s', param(n), sep);
    eval(sprintf('res = res%d;', n));
    eval(sprintf('H = H%d;', n));
    
    eval(sprintf('nRes = length(res%d);', n));
    for ii = 1:nRes
        fprintf(fid, strrep(sprintf('%f%s', H(res(ii, 2)), sep), '.', ','));
    end
    fprintf(fid, '\n');
end
% add FEM data
fprintf(fid, 'FEM%s', sep);
for ii = 1 : length(resFEM)
    fprintf(fid, strrep(sprintf('%f%s', HFEM(resFEM(ii, 2)), sep), '.', ','));
end
fprintf(fid, '\n');
fclose(fid);

%%
function [bwth] = extractResonances(H)

nH = length(H);

[pks, idx] = findpeaks(H);
nPks = length(pks);

bwth = nan(nPks,3);
% loop over peaks to extract the bandwidth
for p = 1:nPks
    
    % find left side -3 dB limit
    idxLow = max(idx(p)-1, 1);
    while and(idxLow >= 1, ...
            and(H(idxLow)>(pks(p)-3), H(idxLow)<H(idxLow+1)))
        idxLow = idxLow-1;
        if idxLow == 1
            break
        end
    end
    
    % find right side -3 dB limit
    idxHigh = min(idx(p) +1, nH);
    while and(idxHigh <= nH, ...
            and(H(idxHigh)>(pks(p)-3), H(idxHigh)<H(idxHigh-1)))
        idxHigh = idxHigh + 1;
        if idxHigh == nH
            break
        end
    end
    
    if and(...
        and(H(idxLow+1)>(pks(p)-3), H(idxLow)<H(idxLow+1)),...
        and(H(idxHigh-1)>(pks(p)-3), H(idxHigh)<H(idxHigh-1)))
        
        bwth(p,1) = idxLow;
        bwth(p,2) = idx(p);
        bwth(p,3) = idxHigh;
    end
    
end
bwth = bwth(not(isnan(bwth(:,1))),:);
end
