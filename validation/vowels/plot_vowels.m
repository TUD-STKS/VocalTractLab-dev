
area = [0.8 1.34];

vowels = {'a', 'i', 'u', 'schwa'};
types = {'_st', '', '_a'};
gender = {'m', 'f'};
genderNames = {'male', 'female'};
typesSave = {'_straight', '_curved', '_curved_var_area'};
load a_m.txt
freqs = a_m(:,1);
nFreqs = length(freqs);

lineSize = 1;
ftSize = 10;
colors = {'r', 'k', 'b', [0.2 0.6 0.4]};
% colors = {'k', 0.2*[1 1 1], 0.5*[1 1 1], 0.8*[1 1 1]};
lineStyle = {'-', '-', '-', '-'};
% lineStyle = {'-', '--', ':', '-.'};
stringSave = 'save(''-mat'', ''vowels_MM.mat''';
csvFileName = 'resonancesFEM.csv';
fid = fopen(csvFileName, 'w');

% h = figure('position', [656   266   414   673]);%('position', [568    42   982   954]);
cnt = 0;


% loop over vowels
for v = 1:3
    
    fprintf(fid, 'Vowel %s%s\n', vowels{v}, sep);

    % loop over genders
    for g = 1:1
        corrFac = (10^5)/area(g);
      load([vowels{v} '_' gender{g} '_fem.mat']);  % FEM 
      mod_MM = ones(nFreqs, 3);
      phase_MM = ones(nFreqs, 3);
      for t = 1:3
        name = [vowels{v} '_' gender{g} types{t}];
        if exist([name '.txt'])
          load([name '.txt'])
          eval(sprintf('mod_MM(:,%d) = %s(:,2);', t, name));
          eval(sprintf('phase_MM(:,%d) = %s(:,3);', t, name));
          nameSave = [vowels{v} '_m' typesSave{t}];
          eval(sprintf('%s(:,1) = freqs;', nameSave));
          eval(sprintf('%s(:,2) = mod_MM(:,%d) * corrFac;', nameSave, t));
          eval(sprintf('%s(:,3) = phase_MM(:,%d);', nameSave, t));
          stringSave = [stringSave ',''' nameSave ''' ']; 
        end
      end 

      cnt = cnt + 1;
%       subplot(3,1,cnt)
      figure

      % plot module
%       subplot(2,1,1)
      title(['/' vowels{v} '/ ' genderNames{g}])

      hold on
      H = 20*log10(abs(H));
      plot(f/1000, H, lineStyle{1}, 'linewidth', lineSize, 'color', colors{1})
      
      % extract resonances of FEM
      res = extractResonances(H);
      nRes = length(res);
      for ii = 1:nRes
          fprintf(fid, '%d%s', res(ii, 2), sep);
      end
      fprintf(fid, '\n');
      for ii = 1:nRes
            fprintf(fid, '%d%s', res(ii, 3)-res(ii, 1), sep);
      end
      fprintf(fid, '\n');
      for ii = 1:nRes
        fprintf(fid, strrep(sprintf('%d%s', H(res(ii, 2)), sep),'.', ','));
      end
      fprintf(fid, '\n');
      
      
      for t = 1:3
        plot(freqs/1000, 20*log10(corrFac * mod_MM(:,t)), lineStyle{t+1}, ...
          'linewidth', lineSize, 'color', colors{t+1})
      end

      grid on
      xlim([0 10])
      ylim([70 150])
      xlabel('f (kHz)')
      ylabel('|H| (dB)')

      set(gca, 'fontsize', ftSize)

%       % plot phase
%       subplot(2,1,2)
%       hold on
%       plot(f, -angle(H), 'linewidth', lineSize, 'color', colors{1})
%       for t = 1:3
%         plot(freqs, phase_MM(:,t), 'linewidth', lineSize, 'color', colors{t+1})
%       end
% 
%       xlim([0 10000])
% 
%       xlabel('f (Hz)')
%       ylabel('phase (rad)')

%       h = legend('FEM', 'MM straight', 'MM curved', 'MM curved + area variation',...
%          'orientation', 'horizontal');
%       set(h, 'fontsize', ftSize, 'position', [0.15 0.45 0.7 0.07]);
%       legend boxoff 
%       set(gca, 'fontsize', ftSize, 'position', [0.13 0.11 0.775 0.3])
    end
end
fclose(fid);

% print('-painters', '-dsvg', h, 'vowels.svg')

stringSave = [stringSave ');'];
eval(stringSave);

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