%% data values
data = [0.5, 0.7, 1, 1, 1;
        0.5, 1,   1, 1, 1];
    
%% data coords x,y,z in [-0.5,0.5] 
coords = [-0.3, -0.1, 0, 0.5, 0.3;
           0.2,    0, 0,   0, 0.3;
              0,    0, 0,   0, 0];
%% sectors
sectors = [0,0,0,0,0,0,0,2,5];

%%
size(data)
size(coords)

%%
cuda_mex_kernel(single(data(:,:)),single(coords(:,:)),single(sectors));