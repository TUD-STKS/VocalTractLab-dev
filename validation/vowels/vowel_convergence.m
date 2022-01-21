vowels = {'a', 'i', 'u'};
nVowels = length(vowels);
param = [20 30 40 50 60];
nCutOffs = length(param);

% simulation times
simuTimesA = [6+4/60, 6+12/60, 11, 23, 45];
simuTimesI = [5+49/60, 9, 21, 50, 60+56];
simuTimesU = [3.5, 6.5, 15, 36,60+25];

% linear regression
lmA = fitlm(table(param(2:end)', log(simuTimesA(2:end))'), 'linear')

% plot simulation times
h = figure('position', [680   558   467   325]);
semilogy(param, simuTimesA, 'r*')
hold on
semilogy(param, simuTimesI, 'kx')
semilogy(param, simuTimesU, 'b+')
set(gca, 'xtick', param)
xlim([15 65])
ylim([3 150])
xlabel('Maximal cut-off frequency (kHz)')
ylabel('Computation time (minutes)')
legend('/a/', '/i/', '/u/', 'location', 'northwest')
legend boxoff

print('-dsvg', h, 'computation_time_raw.svg')




sep = ';';

f = 1:9991;
csvFileName = 'resonances.csv';
fid = fopen(csvFileName, 'w');

for v = 1:nVowels
    figure
    hold on
    fprintf(fid, 'Vowel %s%s\n', vowels{v}, sep);
    
    for m = 1:nCutOffs
        tfName = [vowels{v} '_m_a_' num2str(maxCutOff(m)) '.txt'];
        load(tfName);
        eval(['H = ' tfName(1:end-4) ';']);
        H = 20*log10(abs(interp1(H(:,1), H(:,2), f, 'spline')));
        
        plot(f/1000, H)
        res = extractResonances(H);
        
        % export resonances frequencies in csv file
        nRes = length(res);
        fprintf(fid, '%d%s', param(m), sep);
        for ii = 1:nRes
            fprintf(fid, '%d%s', res(ii, 2), sep);
        end
        fprintf(fid, '\n');
        
        % export resonance bandwidths in csv file
                fprintf(fid, '%d%s', param(m), sep);
        for ii = 1:nRes
            fprintf(fid, '%d%s', res(ii, 3)-res(ii, 1), sep);
        end
        fprintf(fid, '\n');
        
        % export resonance amplitude in csv file
        fprintf(fid, '%d%s', param(m), sep);
        for ii = 1:nRes
            fprintf(fid, strrep(sprintf('%d%s', H(res(ii, 2)), sep),'.', ','));
        end
        fprintf(fid, '\n');
    end
    title(['/' vowels{v} '/'])
    legend('20', '30', '40', '50', '60')
    xlim([0 10])
    
    fprintf(fid, '\n');
    
end
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