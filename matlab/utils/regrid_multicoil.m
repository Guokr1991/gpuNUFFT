function img = regrid_multicoil(data, FT)
% 
% Regridding wrapper 3D multicoil data
% 

[nIntl,nRO,nCh]=size(data);

for ii = 1:nCh
    img(:,:,:,ii) = FT'*data(:,:,ii);
end