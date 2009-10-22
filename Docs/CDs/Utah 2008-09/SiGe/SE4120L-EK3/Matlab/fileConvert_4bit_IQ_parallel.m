function [bytes]=fileConvert_4bit_IQ_parallel(bytes)
%fileConvert_4bit_IQ_parallel.m
%
%-------------------------------------------------------------------------
% File conversion utility : Convert 4-bit IQ parallel
% from USB Driver to I0, Q0, I1, Q1... bytes
%
% Updated: Rev 1.1 29 April 2008 Michael Ball SiGe
%-------------------------------------------------------------------------
% clear;clc;close all;
% ***********************************************************************
% Simple file conversion to change 1x byte, to 2x bytes
%
%               fileConvert Output Data
% Byte 1         Ix (+3,1,-3,-1) - 2-bit
% Byte 2         Qx (+3,1,-3,-1) - 2-bit
% 
%               Driver Output Data
% Byte 1         Si Sq Mi Mq 
%                -15,-13,-11,-9,-7,-5,-3,-1/+1,+3,+5,+7,+9,+11,+13,+15
%
%               SE4120L Output Data (4-bit I/Q, parallel) 
% Byte 1
% bits:     b7,    b6,    b5,    b4,    b3,    b2,    b1,    b0
% data:     x,      x,     x,    x,     Mq,    Mi,    Sq,    Si
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

% test data
%for j = 1: bytes
%    data_in(j) = -15;
%end

for j = 1: bytes % Conversion of Si Sq Mi Mq input bytes
        
        dataByte = data_in(j);  % Determine Si (SIGN bit)
        if dataByte > 0         % Remove driver sign bit inversion
            I_SIGN = -1;
        else I_SIGN = 1;
        end
        
        dataByte = dataByte*(-I_SIGN) - 8;
        
        if dataByte > 0         % Determine Mq
            Q_MAG = 3;            
        else Q_MAG = 1;
        end

        % Shift towards zero by 4
        dataByte = dataByte - 4*(Q_MAG-2);
               
        if dataByte > 0         % Determine Mi
            I_MAG = 3;
        else I_MAG = 1;
        end           
        
        % Shift towards zero by 2
        dataByte = dataByte - 2*(I_MAG - 2);

        % dataByte = abs(dataByte) - 2;
       
        if dataByte > 0         % Determine Sq
             Q_SIGN = 1;       
       	else Q_SIGN = -1;
        end                  
        
        data_out(index) = I_SIGN*I_MAG; %*Q_SIGN; % I output byte
        index = index +1;               % Increment to next output byte

        data_out(index) = Q_SIGN*Q_MAG; %*I_MAG; % Q output byte
        index = index +1;               % Increment to next output byte
        
end

bytes = out_bytes;

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

