% findGPS1
%
% A simple script that tests all possible 32 PRN codes and under defined
% doppler bins, doing GPS signal acquisition.  Then what is reported is
% ratio of the max to the next highest max.  Typically a value >1.75 
% indicates the detection of the GPS signal
%
% GNSS Laboratory  April-2006  Univ of Colorado
% Updated: Rev 1.1 29 April 2008 Michael Ball SiGe
%-------------------------------------------------------------------------
clear;close all;clc;
% ------------------------------------------------------------------------
%
% User Data Input
%
% ------------------------------------------------------------------------
disp('GPS Signal Acquisition Testing');disp(' ');
%disp(  'files in current directory:')
%dir
%
disp('Input data file should be: C:\SiGe_SE4120L_CU_USBdriver\bin\gnss.bin');disp(' ');
disp('File conversion IQ data output file: C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin');disp(' ');
%
disp('Mode    Msps    Bits  Data    Sync');
disp('------------------------------------');
disp('1:     8.184     2     IQ     pulse ');
disp('2:     4.092     2     IQ     pulse');
disp('3:     4.092     4     IQ     pulse');
disp('8:     5.456     2     real   parallel');
disp('9:    16.368     1     real   parallel');
disp('10:   16.368     2     real   parallel');
disp('12:    5.456     1     real   parallel');
disp('13:   16.368     2     real   parallel');
disp('14:   16.368     2     IQ     parallel');
disp('15:    8.184     2     IQ     parallel');
disp('16:    5.456     2     IQ     parallel');
disp('17:    4.092     2     IQ     parallel');
disp('18:   16.368     4     IQ     parallel');
disp('19:    8.184     4     IQ     parallel');
disp('20:    5.456     4     IQ     parallel');
disp('21:    4.092     4     IQ     parallel');disp(' ');
mode=input('Enter data output mode of captured data (1-21): ');

while (mode<1)||(mode>21)||(mode==4)||(mode==5)||(mode ==6)||(mode==7)||(mode==11);
    mode = input('Enter valid Mode selection (1-21): ');
end

query = input('Input data file size in bytes: "1": 1048576 (1MB) or "2": other ? ');
if query == 1
bytes = 1048576;
else bytes=input('Enter alternative file size in bytes: ');
end

query = input('Enter TCXO freq in Hz: "1": 16.368e6 or "2": other ? ');
if query == 1
sampfreq = 16.368e6;
else sampfreq = input('Enter alternative TCXO freq in Hz (eg 16.3676e6): ');
end

query = input('Enter one sided doppler search space in Hz: "1": 6000 or "2": other ? ');
if query == 1
doppspace = 6000;
else doppspace = input('Enter alternative search space (Hz): ');
end

switch mode
    case {1}
        [bytes]=fileConvert_2bit_IQ_pulse(bytes);  % fileConvert_2bit_IQ_pulse.m
        disp('Output file size in bytes:'); disp(bytes); % bytes - 2
        sampfreq = sampfreq/2;      % nominal sampling frequency
        realiq = 2;                 % 1 = real, 2 = IQ
        intfreq = 0;                % Baseband IQ mode
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin';
        xmax = 1.5; % Data plot max amplitude
    case {2}
        [bytes]=fileConvert_2bit_IQ_pulse(bytes);  % fileConvert_2bit_IQ_pulse.m
        disp('Output file size in bytes:'); disp(bytes); % bytes - 2
        sampfreq = sampfreq/4;      % nominal sampling frequency
        realiq = 2;                 % 1 = real, 2 = IQ
        intfreq = 0;                % Baseband IQ mode
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin';
        xmax = 1.5; % Data plot max amplitude
    case {3}
        [bytes]=fileConvert_4bit_IQ_pulse(bytes); % fileConvert_4bit_IQ_pulse.m
        disp('Output file size in bytes:'); disp(bytes); % bytes/2 - 4
        sampfreq = sampfreq/4;      % nominal sampling frequency
        realiq = 2;                 % 1 = real, 2 = IQ
        intfreq = 0;                % Baseband IQ mode 
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin';
        xmax = 3.5; % Data plot max amplitude
    case {8}
        % bytes = bytes;
        sampfreq = sampfreq/3;      % nominal sampling frequency
        realiq = 1;                 % 1 = real, 2 = IQ
        intfreq = sampfreq/4;       % nominal IF is 4.092MHz
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss.bin';
        xmax = 3.5; % Data plot max amplitude
    case {9}
        % bytes = bytes;
        sampfreq = sampfreq/1;      % nominal sampling frequency
        realiq = 1;                 % 1 = real, 2 = IQ
        intfreq = sampfreq/4;       % nominal IF is 4.092MHz           
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss.bin';
        xmax = 1.5; % Data plot max amplitude
    case {10}
        % bytes = bytes;
        sampfreq = sampfreq/1;      % nominal sampling frequency
        realiq = 1;                 % 1 = real, 2 = IQ
        intfreq = sampfreq/4;       % nominal IF is 4.092MHz           
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss.bin';
        xmax = 3.5; % Data plot max amplitude
    case {12}
        % bytes = bytes;
        sampfreq = sampfreq/3;      % nominal sampling frequency
        realiq = 1;                 % 1 = real, 2 = IQ
        intfreq = sampfreq/4;       % nominal IF is 4.092MHz            
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss.bin';
        xmax = 1.5; % Data plot max amplitude
    case {13}
        % bytes = bytes;
        sampfreq = sampfreq/1;      % nominal sampling frequency
        realiq = 1;                 % 1 = real, 2 = IQ
        intfreq = sampfreq/4;       % nominal IF is 4.092MHz            
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss.bin';
        xmax = 3.5; % % Data plot max amplitude
    case {14}        
        [bytes]=fileConvert_2bit_IQ_parallel(bytes); % fileConvert_2bit_IQ_parallel.m
        disp('Output file size in bytes:'); disp(bytes);     % 2*bytes
        sampfreq = sampfreq/1;      % nominal sampling frequency
        realiq = 2;                 % 1 = real, 2 = IQ
        intfreq = 0;                % Baseband IQ mode
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin';
        xmax = 1.5; % % Data plot max amplitude
    case {15}     
        [bytes]=fileConvert_2bit_IQ_parallel(bytes); % fileConvert_2bit_IQ_parallel.m
        disp('Output file size in bytes:'); disp(bytes);     % 2*bytes
        sampfreq = sampfreq/2;      % nominal sampling frequency
        realiq = 2;                 % 1 = real, 2 = IQ
        intfreq = 0;                % Baseband IQ mode              
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin';
        xmax = 1.5; % Data plot max amplitude
    case {16}
        [bytes]=fileConvert_2bit_IQ_parallel(bytes); % fileConvert_2bit_IQ_parallel.m
        disp('Output file size in bytes:'); disp(bytes);     % 2*bytes
        sampfreq = sampfreq/3;      % nominal sampling frequency
        realiq = 2;                 % 1 = real, 2 = IQ
        intfreq = 0;                % Baseband IQ mode      
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin';
        xmax = 1.5; % Data plot max amplitude
    case {17}
        [bytes]=fileConvert_2bit_IQ_parallel(bytes); % fileConvert_2bit_IQ_parallel.m
        disp('Output file size in bytes:'); disp(bytes);     % 2*bytes
        sampfreq = sampfreq/4;      % nominal sampling frequency
        realiq = 2;                 % 1 = real, 2 = IQ
        intfreq = 0;                % Baseband IQ mode      
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin';
        xmax = 1.5; % Data plot max amplitude
    case {18}
        [bytes]=fileConvert_4bit_IQ_parallel(bytes); % fileConvert_4bit_IQ_parallel.m
        disp('Output file size in bytes:'); disp(bytes);     % Display ouput file size
        sampfreq = sampfreq/1;      % nominal sampling frequency
        realiq = 2;                 % 1 = real, 2 = IQ
        intfreq = 0;                % Baseband IQ mode     
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin';
        xmax = 3.5; % Data plot max amplitude
    case {19} 
        [bytes]=fileConvert_4bit_IQ_parallel(bytes); % fileConvert_4bit_IQ_parallel.m
        disp('Output file size in bytes:'); disp(bytes);     % Display ouput file size
        sampfreq = sampfreq/2;      % nominal sampling frequency
        realiq = 2;                 % 1 = real, 2 = IQ
        intfreq = 0;                % Baseband IQ mode    
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin';
        xmax = 3.5; % Data plot max amplitude
    case {20}
        [bytes]=fileConvert_4bit_IQ_parallel(bytes); % fileConvert_4bit_IQ_parallel.m
        disp('Output file size in bytes:'); disp(bytes);     % Display ouput file size
        sampfreq = sampfreq/3;      % nominal sampling frequency
        realiq = 2;                 % 1 = real, 2 = IQ
        intfreq = 0;                % Baseband IQ mode    
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin';
        xmax = 3.5; % Data plot max amplitude
    case {21}
        [bytes]=fileConvert_4bit_IQ_parallel(bytes); % fileConvert_4bit_IQ_parallel.m
        disp('Output file size in bytes:'); disp(bytes);     % Display ouput file size
        sampfreq = sampfreq/4;      % nominal sampling frequency
        realiq = 2;                 % 1 = real, 2 = IQ
        intfreq = 0;                % Baseband IQ mode    
        fname='C:\SiGe_SE4120L_CU_USBdriver\bin\gnss_out.bin';
        xmax = 3.5; % Data plot max amplitude
    otherwise
end
% ------------------------------------------------------------------------
%
% Processing...
%
% ------------------------------------------------------------------------
%
%open and read in the data
fid=fopen(fname,'rb');

if (fid==-1)
    disp('Could not open that data file, check filename/directory and try again...')
    return
end

data=fread(fid,bytes,'schar');  % read in xMB of samples
fclose(fid);

disp(' ');disp('Generating Plots...')
%Real samples should be: R0, R1, R2, R3, ...
%IQ samples should be SI0, SQ0, SI1, SQ1, SI2, SQ2, ...
if (realiq==2)  %must be IQ
    datain=data(1:2:bytes);
    dataquad=data(2:2:bytes);
    data=datain + i* dataquad;
else data = -data; % Remove driver sign bit inversion
end

%do a simple plot of the data (requires signal processing toolbox)
figure(1)
if (realiq==1) % Real IF data
    subplot(2,2,1.5),plot((0:1/sampfreq:50/sampfreq),data(1:size(0:1/sampfreq:50/sampfreq,2)))
    title('First 50 data points (inverted gnss.bin)');xlabel('time (sec)');ylabel('Amplitude');  axis ([0 50/sampfreq -xmax xmax]);
    subplot(2,2,3),hist(data,-4:4);title('Real Samples');
    subplot(2,2,4),psd(data,8192,sampfreq/1e6);title('8,192 point FFT');xlabel('Freq (MHz)');axis tight
else  % Baseband IQ data
    subplot(3,2,1),plot((0:1/sampfreq:50/sampfreq),datain(1:size(0:1/sampfreq:50/sampfreq,2)))
    title('First 50 In-phase Samples');xlabel('time (sec)');ylabel('Amplitude'); axis ([0 50/sampfreq -xmax xmax]);
    subplot(3,2,2),plot((0:1/sampfreq:50/sampfreq),dataquad(1:size(0:1/sampfreq:50/sampfreq,2)))
    title('First 50 Quad Samples');xlabel('time (sec)');ylabel('Amplitude'); axis ([0 50/sampfreq -xmax xmax]);
    subplot(3,2,3),hist(datain,-4:4);title('In-phase Samples')
    subplot(3,2,4),hist(dataquad,-4:4);title('Quad Samples')
    subplot(3,2,5.5),psd(data,8192,sampfreq/1e6);title('8,192 point FFT');xlabel('Freq (MHz)');axis tight
end
pause(1)

disp('Doing acquisition now...')
%now do acquisition
for sv=1:32
    [fq_est(sv),cp_est(sv),c_meas(sv)]=acquireGPS(sv,intfreq,doppspace,data',sampfreq);
    
% simplify output for spreadsheet etc,.
%    disp(['PRN',int2str(sv),' has Acquisition Metric of : ',num2str(c_meas(sv))])
    disp([num2str(c_meas(sv))])

    pause(0.1)
end

%plot the metrics
figure(2)
plot(1:32,c_meas,'x');axis tight; title('Acquisition Metrics');xlabel('PRN #');grid

