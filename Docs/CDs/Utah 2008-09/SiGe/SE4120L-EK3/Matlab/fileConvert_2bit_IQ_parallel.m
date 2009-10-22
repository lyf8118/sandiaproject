function [bytes]=fileConvert_2bit_IQ_parallel(bytes)
%fileConvert_2bit_IQ_parallel.m
%
%-------------------------------------------------------------------------
% File conversion utility : Convert 2-bit IQ parallel
% from USB Driver to I0, Q0, I1, Q1... bytes
%
% Updated: Rev 1.1 29 April 2008 Michael Ball SiGe
%-------------------------------------------------------------------------
%clear;clc;close all;
% ***********************************************************************
% Simple file conversion to change 1x bytes, to 2x bytes
%
%               fileConvert Output Data
% Byte 1         Ix (+1,-1) - 1-bit
% Byte 2         Qx (+1,-1) - 1-bit
% 
%               Driver Output Data
% Byte 1         Si Sq (-3,-1/+1,+3) - 2-bit
%
%               SE4120L Output Data (2-bit I/Q, parallel)
% Byte 1
% bits:     b7,    b6,    b5,    b4,    b3,    b2,    b1,    b0
% data:     x,      x,     x,    x,     x,     x,     Sq,    Si
%
% ***********************************************************************

% Input file size: bytes
% Output file size: 2*bytes

%define the input filename
fname_in='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss.bin';

% define the output filename
fname_out='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin';

%open and read in the data
fid=fopen(fname_in,'rb');
if (fid==-1)
    disp('Could not open that data file, check filename/directory and try again...')
    return
end

data_in=fread(fid,bytes,'schar');  % Read as 8 bit signed 
fclose(fid);

% Output file size is 2*bytes 
out_bytes = bytes*2;
data_out = ones(out_bytes,1);  % Prefill matrix with 1

tic % starts a stopwatch timer. 
      
index = 1;

for j = 1: bytes % Conversion of Sq Si input bytes
      
        if data_in(j) > 0                   % Remove driver sign bit inversion
            data_out(index) = -1;           % Si=-1, else Si=1
       end
            index = index +1;               % Increment to next output byte
        if (data_in(j) < 3) && (data_in(j) > -3)              % 
           data_out(index) = -1;            % Sq=-1, else Sq=1
       end
        
        index = index +1;                   % Next output byte
             
end

bytes = out_bytes; % Output file size is 2*bytes

toc % prints the elapsed time since tic was used

% Save the file
% open and write the data

fid = fopen(fname_out,'wb');

if (fid==-1)
    disp('Could not create that data file, check filename/directory and try again...')
    return
end

fwrite(fid,data_out,'schar'); % write as 8 bit signed 
fclose(fid);

