load tfMM_1cone.txt
load tfMM_2cone.txt
load tfMM_1cone_rev.txt
load tfMM.txt

tf1 = tfMM_1cone_rev;
tf2 = tfMM;

%% Plot P/V

figure
subplot(2,1,1)
hold on
% plot(tfMM_1cone(:,1), 20*log10(tfMM_1cone(:,2)))
% plot(tfMM_2cone(:,1), 20*log10(tfMM_2cone(:,2)))
% plot(tfMM_rev(:,1), 20*log10(tfMM_rev(:,2)))
% plot(tfMM(:,1), 20*log10(tfMM(:,2)))
plot(tf1(:,1), 20*log10(tf1(:,2)))
plot(tf2(:,1), 20*log10(tf2(:,2)))

subplot(2,1,2)
hold on
% plot(tfMM_1cone(:,1), tfMM_1cone(:,3))
% plot(tfMM_2cone(:,1), tfMM_2cone(:,3))
% plot(tfMM_rev(:,1), tfMM_rev(:,3))
% plot(tfMM(:,1), tfMM(:,3))
plot(tf1(:,1), tf1(:,3))
plot(tf2(:,1), tf2(:,3))

%% Plot P/P

figure
subplot(2,1,1)
hold on
% plot(tfMM_1cone(:,1), 20*log10(tfMM_1cone(:,4)))
% plot(tfMM_2cone(:,1), 20*log10(tfMM_2cone(:,4)))
% plot(tfMM_rev(:,1), 20*log10(tfMM_rev(:,4)))
% plot(tfMM(:,1), 20*log10(tfMM(:,4)))
plot(tf1(:,1), 20*log10(tf1(:,4)))
plot(tf2(:,1), 20*log10(tf2(:,4)))

subplot(2,1,2)
hold on
% plot(tfMM_1cone(:,1), tfMM_1cone(:,5))
% plot(tfMM_2cone(:,1), tfMM_2cone(:,5))
% plot(tfMM_rev(:,1), tfMM_rev(:,5))
% plot(tfMM(:,1), tfMM(:,5))
plot(tf1(:,1), tf1(:,5))
plot(tf2(:,1), tf2(:,5))