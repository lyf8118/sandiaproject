function [bytes]=fileConvert_2bit_IQ_pulse(bytes)
% fileConvert_2bit_IQ_pulse.m
%
%-------------------------------------------------------------------------
% File conversion utility : Convert 2-bit IQ pulse sync
% from USB Driver to I0, Q0, I1, Q1... bytes
%
% Updated: Rev 1.1 29 April 2008 Michael Ball SiGe
%-------------------------------------------------------------------------
%clear;clc;close all;
% ***********************************************************************
% Simple file conversion to change 2x bytes, to 2x bytes
%
%               fileConvert Output Data
% Byte 1         Ix (+1,-1) - 1-bit
% Byte 2         Qx (+1,-1) - 1-bit
% 
%               Driver Output Data
% Byte 1         Si (-3,+3) - 1-bit
% Byte 2         Mi (-1,+1) - 1-bit
%
%               SE4120L Output Data (2-bit I/Q, Pulse) 
% Byte 1
% bits:    b7,    b6,    b5,    b4,    b3,    b2,    b1,     b0
% data:    x,      x,     x,    x,     x,     x,     Sync,   Si, 
% 
% Byte 2
% bits:     b7,    b6,    b5,    b4,    b3,    b2,    b1,     b0
% data:     x,      x,     x,     x,     x,     x,    Sync=1, Sq, 
%
% ***********************************************************************

% Input file size: bytes
% Output file size: bytes - 2

% define the input filename
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

% Output file always begins with Si, size is bytes - 2 to be safe, since sync=1 could be second byte at latest 
bytes = bytes - 2;

j = 1;
        while ((data_in(j) < 3) && (data_in(j) > -3)) % Find first input Si sync byte
        j = j+1;
        end
        
index = j;        

data_out = ones(bytes,1);  % Preallocate matrix as ones to save memory & time

tic % starts a stopwatch timer. 

for j = 1: bytes    
              
        if data_in(index) > 0       % Reverse the driver sign bit inversion
            data_out(j) = -1;   % and ignore the LSB sync
        end
        
        index = index +1;                    % 
end

toc % prints the elapsed time

% Save the file
% open and write the data

fid = fopen(fname_out,'wb');

if (fid==-1)
    disp('Could not create that data file, check filename/directory and try again...')
    return
end

fwrite(fid,data_out,'schar'); % write as 8 bit signed 
fclose(fid);
