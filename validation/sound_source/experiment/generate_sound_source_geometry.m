
% geometrical parameters
sq = 2.95; % side of the square segment
sql = 1.6; % length of the square segment
co = [3./2, 1.8/2]; % entry and exit radius of the cone part
col = 3.7;      % length of the cone
cy = 0.5;       % radius of the cylinder part
cyl = 1.;       % length of the cylinder part
no = [0. 1.];    % normal of all segments

% generate csv file that can be read by Vocaltractlab
csvName = 'sound_source.csv';
sep = ';';      % define separating character
fid = fopen(csvName, 'w');
%% generate square segment
% % write x coordinates
% fprintf(fid, '%f%s', 0., sep); % x coordinate of center
% fprintf(fid, '%f%s', no(1), sep); % x coordinate of normal
% fprintf(fid, '%f%s', 1., sep);    % entrance scaling factor
% fprintf(fid, '%f%s', -sq/2, sep);  % x coordinate of contour
% fprintf(fid, '%f%s', sq/2, sep);  % x coordinate of contour
% fprintf(fid, '%f%s', sq/2, sep);  % x coordinate of contour
% fprintf(fid, '%f%s', -sq/2, sep);  % x coordinate of contour
% fprintf(fid, '\n')
% 
% % write y coordinates
% fprintf(fid, '%f%s', 0., sep); % y coordinate of center
% fprintf(fid, '%f%s', no(2), sep); % y coordinate of normal
% fprintf(fid, '%f%s', 1., sep);    % exit scaling factor
% fprintf(fid, '%f%s', -sq/2, sep);  % y coordinate of contour
% fprintf(fid, '%f%s', -sq/2, sep);  % y coordinate of contour
% fprintf(fid, '%f%s', sq/2, sep);  % y coordinate of contour
% fprintf(fid, '%f%s',sq/2, sep);  % y coordinate of contour
% fprintf(fid, '\n')
% 
% % write x coordinates
% fprintf(fid, '%f%s', 10., sep); % x coordinate of center
% fprintf(fid, '%f%s', no(1), sep); % x coordinate of normal
% fprintf(fid, '%f%s', 1., sep);    % entrance scaling factor
% fprintf(fid, '%f%s', -sq/2, sep);  % x coordinate of contour
% fprintf(fid, '%f%s', sq/2, sep);  % x coordinate of contour
% fprintf(fid, '%f%s', sq/2, sep);  % x coordinate of contour
% fprintf(fid, '%f%s', -sq/2, sep);  % x coordinate of contour
% fprintf(fid, '\n')
% 
% % write y coordinates
% fprintf(fid, '%f%s', 0., sep); % y coordinate of center
% fprintf(fid, '%f%s', no(2), sep); % y coordinate of normal
% fprintf(fid, '%f%s', 1., sep);    % exit scaling factor
% fprintf(fid, '%f%s', -sq/2, sep);  % y coordinate of contour
% fprintf(fid, '%f%s', -sq/2, sep);  % y coordinate of contour
% fprintf(fid, '%f%s', sq/2, sep);  % y coordinate of contour
% fprintf(fid, '%f%s',sq/2, sep);  % y coordinate of contour
% fprintf(fid, '\n')
%% generate cone

% generate contour coordinates
nTheta = 100;
theta = linspace(0, 2*pi, nTheta);
cont = zeros(nTheta, 2);
for t = 1:nTheta
    cont(t, 1) = co(2)*cos(theta(t));
    cont(t, 2) = co(2)*sin(theta(t));
end

% write x coordinates
fprintf(fid, '%f%s', 0., sep); % x coordinate of center
fprintf(fid, '%f%s', no(1), sep); % x coordinate of normal
fprintf(fid, '%f%s', co(1)/co(2), sep);    % entrance scaling factor
% x coordinate of the contour
for t = 1:nTheta
    fprintf(fid, '%f%s', cont(t,1), sep);  
end
fprintf(fid, '\n');

% write y coordinates
fprintf(fid, '%f%s', 0., sep); % y coordinate of center
fprintf(fid, '%f%s', no(2), sep); % y coordinate of normal
fprintf(fid, '%f%s', 1, sep);    % exit scaling factor
% y coordinate of the contour
for t = 1:nTheta
    fprintf(fid, '%f%s', cont(t,2), sep);  
end
fprintf(fid, '\n');

%% Generate cylinder

% generate contour coordinates
nTheta = 100;
theta = linspace(0, 2*pi, nTheta);
cont = zeros(nTheta, 2);
for t = 1:nTheta
    cont(t, 1) = cy*cos(theta(t));
    cont(t, 2) = cy*sin(theta(t));
end

% write x coordinates
fprintf(fid, '%f%s', col, sep); % x coordinate of center
fprintf(fid, '%f%s', no(1), sep); % x coordinate of normal
fprintf(fid, '%f%s', 1., sep);    % entrance scaling factor
% x coordinate of the contour
for t = 1:nTheta
    fprintf(fid, '%f%s', cont(t,1), sep);  
end
fprintf(fid, '\n');

% write y coordinates
fprintf(fid, '%f%s', 0., sep); % y coordinate of center
fprintf(fid, '%f%s', no(2), sep); % y coordinate of normal
fprintf(fid, '%f%s', 1, sep);    % exit scaling factor
% y coordinate of the contour
for t = 1:nTheta
    fprintf(fid, '%f%s', cont(t,2), sep);  
end
fprintf(fid, '\n');

% write x coordinates
fprintf(fid, '%f%s', col + cyl, sep); % x coordinate of center
fprintf(fid, '%f%s', no(1), sep); % x coordinate of normal
fprintf(fid, '%f%s', 1., sep);    % entrance scaling factor
% x coordinate of the contour
for t = 1:nTheta
    fprintf(fid, '%f%s', cont(t,1), sep);  
end
fprintf(fid, '\n');

% write y coordinates
fprintf(fid, '%f%s', 0., sep); % y coordinate of center
fprintf(fid, '%f%s', no(2), sep); % y coordinate of normal
fprintf(fid, '%f%s', 1, sep);    % exit scaling factor
% y coordinate of the contour
for t = 1:nTheta
    fprintf(fid, '%f%s', cont(t,2), sep);  
end
fprintf(fid, '\n');

fclose(fid);
