function [bytes]=fileConvert_4bit_IQ_pulse(bytes)
% fileConvert_4bit_IQ_pulse.m
%
%-------------------------------------------------------------------------
% File conversion utility : Convert 4-bit IQ pulse sync 4.092MSps
% from USB Driver to I0, Q0, I1, Q1... bytes
%
% Updated: Rev 1.1 29 April 2008 Michael Ball SiGe
%-------------------------------------------------------------------------
%clear;clc;close all;
% ***********************************************************************
% Simple file conversion to change 2x bytes, to 1x bytes
%
%               fileConvert Output Data
% Byte 1         Ix (+3,+1,-1,-3) - 2-bit
% Byte 2         Qx (+3,+1,-1,-3) - 2-bit
% 
%               Driver Output Data
% Byte 1         Si (-3,+3) - 1-bit
% Byte 2         Mi (-1,+1) - 1-bit
% Byte 3         Sq (-1,+1) - 1-bit
% Byte 4         Mq (-1,+1) - 1-bit
%
%               SE4120L Output Data (4-bit I/Q, Pulse, 4.092MSps) 
% Byte 1
% bits:     b7,    b6,    b5,    b4,    b3,    b2,    b1,     b0
% data:     x,      x,     x,    x,     x,     x,     sync=1, Si
% 
% Byte 2
% bits:     b7,    b6,    b5,    b4,    b3,    b2,    b1,     b0
% data:     x,      x,     x,     x,     x,     x,    sync=0, Mi
%
% Byte 3
% bits:     b7,    b6,    b5,    b4,    b3,    b2,    b1,     b0
% data:     x,      x,     x,     x,     x,     x,    sync=0, Sq
%
% Byte 4
% bits:     b7,    b6,    b5,    b4,    b3,    b2,    b1,     b0
% data:     x,      x,     x,     x,     x,     x,    sync=0, Mq
%
% ***********************************************************************

% Input file size: bytes
% Output file size: bytes/2 -4

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

% Output file size is bytes/2 - 4 to be safe, since sync=1 could be third byte at latest 
bytes = bytes/2 - 4;
data_out = zeros(bytes,1);  % Prefill matrix with -1

tic % starts a stopwatch timer. 

j = 1;
        while ((data_in(j) < 3) && (data_in(j) > -3)) % Find first input Si sync byte
        j = j+1;
        end
        
index = j;

for j = 1: bytes % Start conversion of S/M byte pairs at first Si sync
    
        dataByte1 = -data_in(index);    % Si/q byte, remove driver sign bit inversion
        index = index +1;                   % Increment to next Mi/q byte
        dataByte2 = -data_in(index);    % Mi/q byte, remove driver sign bit inversion
    
        if dataByte1 > 0                    % +ve S/M
             data_out(j) = 2 + dataByte2;   % If true, output 2-bit byte
        end
        
        if dataByte1 < 0                    % -ve S/M
             data_out(j) = -2 - dataByte2;  % If true, output 2-bit byte
        end

        index = index +1;                    % Increment to next Si/q bytes

end

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

