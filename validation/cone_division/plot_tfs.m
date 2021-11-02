load tfMM_1cone.txt
load tfMM_2cone.txt
load tfMM_1cone_rev.txt
load tfMM_1cone_rev_curved.txt
load tfMM.txt

tf1 = tfMM_1cone_rev_curved;
tf2 = tfMM;

%% Plot P/V

figure
subplot(2,1,1)
hold on
plot(tf1(:,1), 20*log10(tf1(:,2)))
plot(tf2(:,1), 20*log10(tf2(:,2)))

subplot(2,1,2)
hold on
plot(tf1(:,1), tf1(:,3))
plot(tf2(:,1), tf2(:,3))

%% Plot P/P

figure
subplot(2,1,1)
hold on
plot(tf1(:,1), 20*log10(tf1(:,4)))
plot(tf2(:,1), 20*log10(tf2(:,4)))

subplot(2,1,2)
hold on
plot(tf1(:,1), tf1(:,5))
plot(tf2(:,1), tf2(:,5))

%% plot input impedance
% return

figure
subplot(2,1,1)
hold on
% plot(tf1(:,1), 20*log10(tf1(:,6)))
plot(tf2(:,1), 20*log10(tf2(:,6)))

subplot(2,1,2)
hold on
% plot(tf1(:,1), tf1(:,7))
plot(tf2(:,1), tf2(:,7))