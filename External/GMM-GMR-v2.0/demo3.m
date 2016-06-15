function demo3
%
% Demo of the smooth transitions properties of data retrieved by 
% Gaussian Mixture Regression (GMR). 
% This source code is the implementation of the algorithms described in 
% Section 2.4, p.38 of the book "Robot Programming by Demonstration: A 
% Probabilistic Approach". 
%
% Author:	Sylvain Calinon, 2009
%			http://programming-by-demonstration.org
%
% This program loads two 3D datasets, 
% trains two separates Gaussian Mixture Model (GMM), and retrieves a 
% generalized version of the two datasets concatenated in time, with 
% associated constraints, through Gaussian Mixture Regression (GMR). 
% Each datapoint has 3 dimensions, consisting of 1 temporal value and 2 
% spatial values (e.g. drawing on a 2D Cartesian plane). A sequence of 
% temporal values is used as query points to retrieve a sequence of 
% expected spatial distributiuon through Gaussian Mixture Regression (GMR).
% The position of the last datapoint in the first dataset is not consistent
% with the first datapoint of the second dataset. However, by encoding 
% separately the two datasets in GMM, then concatenating the components in 
% a single model and using regression, a smooth signal with smooth 
% transition between the two data is retrieved.
%
% This source code is given for free! However, I would be grateful if you refer 
% to the book (or corresponding article) in any academic publication that uses 
% this code or part of it. Here are the corresponding BibTex references: 
%
% @book{Calinon09book,
%   author="S. Calinon",
%   title="Robot Programming by Demonstration: A Probabilistic Approach",
%   publisher="EPFL/CRC Press",
%   year="2009",
%   note="EPFL Press ISBN 978-2-940222-31-5, CRC Press ISBN 978-1-4398-0867-2"
% }
%
% @article{Calinon07,
%   title="On Learning, Representing and Generalizing a Task in a Humanoid Robot",
%   author="S. Calinon and F. Guenter and A. Billard",
%   journal="IEEE Transactions on Systems, Man and Cybernetics, Part B",
%   year="2007",
%   volume="37",
%   number="2",
%   pages="286--298",
% }

%% Definition of the number of components used in GMM.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
nbStates1 = 2;
nbStates2 = 5;

%% Load a dataset consisting of 3 demonstrations of a 2D signal.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
load('data/data3_a.mat'); %load 'Data1'
load('data/data3_b.mat'); %load 'Data2'
nbVar = size(Data1,1);

%% Training of GMM1 by EM algorithm, initialized by k-means clustering.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[Priors1, Mu1, Sigma1] = EM_init_kmeans(Data1, nbStates1);
[Priors1, Mu1, Sigma1] = EM(Data1, Priors1, Mu1, Sigma1);

%% Training of GMM2 by EM algorithm, initialized by k-means clustering.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
[Priors2, Mu2, Sigma2] = EM_init_kmeans(Data2, nbStates2);
[Priors2, Mu2, Sigma2] = EM(Data2, Priors2, Mu2, Sigma2);

%% Creation of a mixed model
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
Priors = cat(2,Priors1,Priors2)./2;
Mu = cat(2,Mu1,Mu2);
Sigma = cat(3,Sigma1,Sigma2);

%% Use of GMR to retrieve a generalized version of the data and associated
%% constraints. A sequence of temporal values is used as input, and the 
%% expected distribution is retrieved. 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
expData(1,:) = linspace(1, 200, 200);
[expData(2:nbVar,:), expSigma] = GMR(Priors, Mu, Sigma,  expData(1,:), [1], [2:nbVar]);

%% Plot of the data
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
figure('position',[10,10,1000,800],'name','GMM-GMR-demo3');
%plot 1D
for n=1:nbVar-1
  subplot(3*(nbVar-1),2,(n-1)*2+1); hold on;
  plot(Data1(1,:), Data1(n+1,:), 'x', 'markerSize', 4, 'color', [.8 0 0]);
  plot(Data2(1,:), Data2(n+1,:), 'x', 'markerSize', 4, 'color', [0 .8 0]);
  axis([min(expData(1,:)) max(expData(1,:)) min(expData(n+1,:))-0.02 max(expData(n+1,:))+0.02]);
  xlabel('t','fontsize',16); ylabel(['x_' num2str(n)],'fontsize',16);
end
%plot 2D
subplot(3*(nbVar-1),2,[2:2:2*(nbVar-1)]); hold on;
plot(Data1(2,:), Data1(3,:), 'x', 'markerSize', 4, 'color', [.8 0 0]);
plot(Data2(2,:), Data2(3,:), 'x', 'markerSize', 4, 'color', [0 .8 0]);
axis([min(expData(2,:))-0.02 max(expData(2,:))+0.02 min(expData(3,:))-0.02 max(expData(3,:))+0.02]);
xlabel('x_1','fontsize',16); ylabel('x_2','fontsize',16);

%% Plot of the GMM encoding results
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%plot 1D
for n=1:nbVar-1
  subplot(3*(nbVar-1),2,4+(n-1)*2+1); hold on;
  plotGMM(Mu1([1,n+1],:), Sigma1([1,n+1],[1,n+1],:), [.8 0 0], 1);
  plotGMM(Mu2([1,n+1],:), Sigma2([1,n+1],[1,n+1],:), [0 .8 0], 1);
  axis([min(expData(1,:)) max(expData(1,:)) min(expData(n+1,:))-0.02 max(expData(n+1,:))+0.02]);
  xlabel('t','fontsize',16); ylabel(['x_' num2str(n)],'fontsize',16);
end
%plot 2D
subplot(3*(nbVar-1),2,4+[2:2:2*(nbVar-1)]); hold on;
plotGMM(Mu1([2,3],:), Sigma1([2,3],[2,3],:), [.8 0 0], 1);
plotGMM(Mu2([2,3],:), Sigma2([2,3],[2,3],:), [0 .8 0], 1);
axis([min(expData(2,:))-0.02 max(expData(2,:))+0.02 min(expData(3,:))-0.02 max(expData(3,:))+0.02]);
xlabel('x_1','fontsize',16); ylabel('x_2','fontsize',16);

%% Plot of the GMR regression results
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%plot 1D
for n=1:nbVar-1
  subplot(3*(nbVar-1),2,8+(n-1)*2+1); hold on;
  plotGMM(expData([1,n+1],:), expSigma(n,n,:), [0 0 .8], 3);
  axis([min(expData(1,:)) max(expData(1,:)) min(expData(n+1,:))-0.02 max(expData(n+1,:))+0.02]);
  xlabel('t','fontsize',16); ylabel(['x_' num2str(n)],'fontsize',16);
end
%plot 2D
subplot(3*(nbVar-1),2,8+[2:2:2*(nbVar-1)]); hold on;
plotGMM(expData([2,3],:), expSigma([1,2],[1,2],:), [0 0 .8], 2);
axis([min(expData(2,:))-0.02 max(expData(2,:))+0.02 min(expData(3,:))-0.02 max(expData(3,:))+0.02]);
xlabel('x_1','fontsize',16); ylabel('x_2','fontsize',16);

pause;
close all;
