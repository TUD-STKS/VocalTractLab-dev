% generate circle contour
nTheta = 100;
theta = linspace(0, 2*pi, nTheta);
cont = zeros(nTheta, 2);
for ii = 1:nTheta
    cont(ii,1) = cos(theta(ii));
    cont(ii, 2) = sin(theta(ii));
end

no = [0. 1.];    % normal of all segments

r = [3, 5];         % radius of the segments
scaleIn = [1/3, 0.5999999];
scaleOut = [1, 1];
lengths = [0, 8];
sep = ';';

fileName = 'cone.csv';
fid = fopen(fileName, 'w');

for c = 1:2
    % write x coordinates
    fprintf(fid, '%f%s', lengths(c), sep); % x coordinate of center
    fprintf(fid, '%f%s', no(1), sep); % x coordinate of normal
    fprintf(fid, '%f%s', scaleIn(c), sep);    % entrance scaling factor
    % x coordinate of the contour
    for t = 1:nTheta
        fprintf(fid, '%f%s', r(c)*cont(t,1), sep);  
    end
    fprintf(fid, '\n');

    % write y coordinates
    fprintf(fid, '%f%s', 0., sep); % y coordinate of center
    fprintf(fid, '%f%s', no(2), sep); % y coordinate of normal
    fprintf(fid, '%f%s', scaleOut(c), sep);    % exit scaling factor
    % y coordinate of the contour
    for t = 1:nTheta
        fprintf(fid, '%f%s', r(c)*cont(t,2), sep);  
    end
    fprintf(fid, '\n');
end

fclose(fid);