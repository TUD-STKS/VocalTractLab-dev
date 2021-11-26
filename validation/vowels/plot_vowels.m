
area = 0.8;
corrFac = (10^5)/area;

vowels = {'a', 'i', 'u'};
types = {'_st', '', '_a'};
typesSave = {'_straight', '_curved', '_curved_var_area'};
load a_m.txt
freqs = a_m(:,1);
nFreqs = length(freqs);

lineSize = 1;
ftSize = 14;
colors = {'r', 'k', 'b', [0.2 0.6 0.4]};
stringSave = 'save(''-mat'', ''vowels_MM.mat''';

% loop over vowels
for v = 1:3
  load([vowels{v} '_m_fem.mat']);  % FEM 
  mod_MM = ones(nFreqs, 3);
  phase_MM = ones(nFreqs, 3);
  for t = 1:3
    name = [vowels{v} '_m' types{t}];
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

  figure

  % plot module
  subplot(2,1,1)
  title(['vowel ' vowels{v}])

  hold on
  plot(f, 20*log10(abs(H)), 'linewidth', lineSize, 'color', colors{1})
  for t = 1:3
    plot(freqs, 20*log10(corrFac * mod_MM(:,t)), ...
      'linewidth', lineSize, 'color', colors{t+1})
  end

  xlim([0 10000])
  ylim([70 150])
  xlabel('f (Hz)')
  ylabel('|H| (dB)')

  set(gca, 'fontsize', ftSize)

  % plot phase
  subplot(2,1,2)
  hold on
  plot(f, -angle(H), 'linewidth', lineSize, 'color', colors{1})
  for t = 1:3
    plot(freqs, phase_MM(:,t), 'linewidth', lineSize, 'color', colors{t+1})
  end

  xlim([0 10000])

  xlabel('f (Hz)')
  ylabel('phase (rad)')

  h = legend('FEM', 'MM straight', 'MM curved', 'MM curved + area variation',...
     'orientation', 'horizontal');
  set(h, 'fontsize', ftSize, 'position', [0.15 0.45 0.7 0.07]);
  legend boxoff 
  set(gca, 'fontsize', ftSize, 'position', [0.13 0.11 0.775 0.3])
end

stringSave = [stringSave ');'];
eval(stringSave);
