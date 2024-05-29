function [jnd_map, jnd_LA, jnd_LC, jnd_PM, jnd_PM_p] = func_JND_modeling_pattern_complexity(img)

if ~isa(img, 'double')
    img = double(img);
end

% luminance adaptation
jnd_LA = func_bg_lum_adapt( img );

% luminance contrast masking
L_c = func_lum_cots_mask( img ); % variance value
a1 = 0.115*16;
a2 = 26;
jnd_LC = ( a1*L_c.^2.4 ) ./ ( L_c.^2+a2^2 ); % transducer

% content complexity
P_c = func_con_cmlx( img );
a3=0.3;
a4=2.7;
a5=1;
C_t = ( a3*P_c .^a4 ) ./ ( P_c.^2+a5^2 ); % complexity transducer

% pattern maksing
jnd_PM = L_c .* C_t;

% edge protection
edge_protect = func_edge_protect( img );

% edge protection as scale factor
jnd_PM_p = jnd_PM .* edge_protect;

% visual masking
jnd_VM = jnd_LC.*jnd_PM_p ;

% JND map
jnd_map = jnd_LA + jnd_VM - 0.3*min( jnd_LA, jnd_VM );
[row, col] = size( img );
valid_mask = zeros( row, col );
r = 3;
valid_mask( r:row-r,r:col-r ) = 1;
jnd_map = jnd_map .* valid_mask;
return;

% luminance adaptation
function jnd_lum_adapt = func_bg_lum_adapt( img0 )
% JND threshold due to Luminance Adaption
min_lum = 32;
alpha = 0.7;
B=[1 1 1 1 1
   1 2 2 2 1
   1 2 0 2 1
   1 2 2 2 1
   1 1 1 1 1]; % filter
bg_lum0 = floor( filter2(B, img0) / 32 );
bg_lum = func_bg_adjust( bg_lum0, min_lum ); % adjust the luminance on dark region~(near 0)
[col, row] = size(img0);
bg_jnd = lum_jnd; % visuable threshold due to bg lum.
jnd_lum = zeros(col, row); % calculate the LA for each pixel
for x = 1:col
    for y = 1:row        
        jnd_lum(x,y) = bg_jnd( bg_lum(x,y)+1 );
    end
end
jnd_lum_adapt = alpha * jnd_lum;
return;

%--------------------------------------
function bg_jnd = lum_jnd
% visuable threshold due to bg lum.
bg_jnd = zeros(256, 1); 
T0 = 17;
gamma = 3 / 128;
for k = 1 : 256
    lum = k-1;
    if lum <= 127
        bg_jnd(k) = T0 * (1 - sqrt( lum/127)) + 3;
    else
        bg_jnd(k) = gamma * (lum-127) + 3;
    end
end
return;

%------------------------------------------
function bg_lum = func_bg_adjust( bg_lum0, min_lum )
% adjust the luminance on dark region~(near 0)
[row, col] = size( bg_lum0 );
bg_lum = bg_lum0;
for x = 1 : row
    for y = 1  :col
        if bg_lum( x,y ) <= 127
            bg_lum(x,y) = round( min_lum + bg_lum(x,y)*(127-min_lum)/127 );
        end
    end
end

% luminance_contrast
function L_c = func_lum_cots_mask( img )
% calculate the luminance contrast for each pixel
R = 2;
ker = ones( 2*R+1 ) / ( 2*R+1 )^2;
mean_mask = imfilter( img, ker ); % mean value of each pixel
mean_img_sqr = mean_mask.^2; % square mean
img_sqr = img.^2; % square
mean_sqr_img = imfilter( img_sqr, ker ); % mean square
var_mask = mean_sqr_img - mean_img_sqr; % variance
var_mask(var_mask<0) = 0;
[row,col] = size( img );
valid_mask = zeros(row,col);
valid_mask(R+1:end-R,R+1:end-R) = 1;
var_mask = var_mask .* valid_mask;
L_c = sqrt( var_mask );

function cmlx_mat = func_con_cmlx( img )
cmlx_map = func_cmlx_num_compute( img );
r = 3;
sig = 1;
fker = fspecial( 'gaussian', r, sig );
cmlx_mat = imfilter( cmlx_map, fker );

function cmlx = func_cmlx_num_compute( img )
% set parameters
r = 1;
nb = r*8; % neighborhood size
otr = 6; % threshold for judging similar orientaion
kx = [ -1 0 1; -1 0 1; -1 0 1 ]/3;
ky = kx';
% Angle step. (coordinate)
sps=zeros(nb,2);
as = 2*pi/nb;
for i = 1:nb
    sps(i,1) = -r*sin((i-1)*as);
    sps(i,2) = r*cos((i-1)*as);
end
% osvp computation
imgd = padarray( img, [r r], 'symmetric' );
[ row, col ] = size( imgd );
Gx = imfilter( imgd, kx ); % gradient along x 
Gy = imfilter( imgd, ky ); % gradient along x
Cimg = sqrt( Gx.^2 + Gy.^2 ); % gradient
Cvimg = zeros( row,col ); % valid pixels (unshooth region)
Cvimg( Cimg>=5 ) = 1; % selecting unshooth region

Oimg = round( atan2(Gy, Gx)/pi*180 ); % algle value
Oimg( Oimg > 90 ) = Oimg( Oimg > 90 ) - 180; % [-90 90]
Oimg( Oimg < -90 ) = 180 + Oimg( Oimg < -90 );
Oimg = Oimg + 90; % [ 0 180 ]
Oimg( Cvimg==0 ) = 180+2*otr;
Cvimgc = Cvimg( r+1:row-r,r+1:col-r );
Oimg_norm = round( Oimg/2/otr );% normalize with threshold 2*otr
onum = round( 180/2/otr )+1; % the type number of orientation bin

for x = 0 : onum % for central pixel
    Oimgc_valid(:,:,x+1) = Oimg_norm==x;
end

% complexity
ssr_no_zero = Oimgc_valid( 1+1:row-2*r+1, 1+1:col-2*r+1, :)~=0; 

cmlx = sum( ssr_no_zero, 3 ); % calculate the rule number
cmlx( Cvimgc==0 ) = 1; % set the rule number of plain as 1
cmlx( 1:r, : ) = 1; % set the rule number for the image boundary
cmlx( end-r+1:end, : ) = 1;
cmlx( :, 1:r ) = 1;
cmlx( :, end-r+1:end ) = 1;

function edge_protect = func_edge_protect( img )
% protect the edge region since the HVS is highly sensitive to it
r = 1;
kx = [ -1 0 1; -2 0 2; -1 0 1 ]/3;
ky = kx';

% osvp computation
imgd = padarray( img, [r r], 'symmetric' );
[ row, col ] = size( imgd );
Gx = imfilter( imgd, kx ); % gradient along x 
Gy = imfilter( imgd, ky ); % gradient along x
Cimg = sqrt( Gx.^2 + Gy.^2 ); % gradient

img_edge = Cimg>30;
img_supedge = 1-1*double(img_edge);
gaussian_kernal = fspecial('gaussian',5,0.8);
edge_protect = filter2(gaussian_kernal,img_supedge);
edge_protect = edge_protect( r+1:row-r,r+1:col-r );
return;