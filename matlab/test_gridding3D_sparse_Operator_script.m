%% testscript with operator usage
clear all; close all; clc;

%% add bin to path
addpath ../bin
addpath(genpath('../../daten'));
addpath(genpath('./gpuNUFFT'));
%% Load data
%load 20111017_Daten_MREG;
load MREG_data_Graz;
%load 20111013_MREG_data_Graz_SoS;
%load 20111024_MREG_Data_MID_65_2mm_Full_Brain;

%% sensmaps
E = struct(E);
smaps = getfield(E,'sensmaps');

%%
smaps_il = zeros([2,size(smaps{1}),length(smaps)]);
for k = 1:length(smaps),
    smaps_il(1,:,:,:,k) = real(smaps{k});%.*E.nufftStruct.sn ./ max(E.nufftStruct.sn(:));
    smaps_il(2,:,:,:,k) = imag(smaps{k});%.*E.nufftStruct.sn./ max(E.nufftStruct.sn(:));
end;
smaps = squeeze(smaps_il(1,:,:,:,:) + 1i*smaps_il(2,:,:,:,:));
clear smaps_il;
%% Perform RegpuNUFFT with Kaiser Besser Kernel 64
osf = 1.25;%1,1.25,1.5,1.75,2
wg = 3;%3-7
sw = 8;
imwidth = 64;
k = E.nufftStruct.om'./(2*pi);
w = ones(E.trajectory_length,1);

%freiburg implementation
G3D = gpuNUFFT(k,w,imwidth,osf,wg,sw,E.imageDim,'sparse',E);
%% one call for all coils
res = zeros(E.imageDim);
kspace = reshape(data,[E.trajectory_length E.numCoils]);
%[imgRegrid_kb,kernel] = grid3D(kspace,k,w,imwidth,osf,wg,sw,'deappo');
tic
imgRegrid_kb = G3D'*kspace;
size(imgRegrid_kb);
exec_time = toc;
disp(['execution time adjoint: ', num2str(exec_time)]);
%% SENS corr
imgRegrid_kb = imgRegrid_kb .* conj(smaps);
%imgRegrid_kb = imgRegrid_kb(:,:,:,:) .* conj(smaps(:,:,:,:));
%% res = SoS of coil data
res = sqrt(sum(abs(imgRegrid_kb).^2,4));
%%
slice = 25;
z_ref = z; %z4em9
%figure, imshow(imresize(abs(res(:,:,slice)),4),[]), title('gpuNUFFT all coils at once');
%figure, imshow(imresize(abs(z_ref(:,:,slice)),4),[]), title('reference (CG)');
res_curr = abs(res(:,:,slice));
%save(['../../daten/results/MREG_abs_slice25'], 'res_gpuNUFFT');
load MREG_abs_slice25;
diff = (res_curr(:) - res_gpuNUFFT(:))' * (res_curr(:) - res_gpuNUFFT(:))
%% check forward gpuNUFFT using solution z
%z_pad = padarray(z_ref,[0 0 10]);
%%
tic
dataRadial = G3D*z;
exec_time = toc;
disp(['execution time forward: ', num2str(exec_time)]);
disp(num2str(size(dataRadial)));
%%
tic
imgRegrid = G3D'*dataRadial;
exec_time = toc;
disp(['execution time adjoint: ', num2str(exec_time)]);