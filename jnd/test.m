clc
clear all
close all

img=imread('5.png');
if size(img,3)==3
    img=rgb2gray(img);
end

%tic
[jnd_map, jnd_LA, jnd_LC, jnd_PM, jnd_PM_p]= func_JND_modeling_pattern_complexity(img);
%toc

figure;
imshow(jnd_map, []);
figure;
imshow(jnd_LC,  []);
figure;
imshow(jnd_PM,  []);
figure;
imshow(jnd_PM_p,[]);
