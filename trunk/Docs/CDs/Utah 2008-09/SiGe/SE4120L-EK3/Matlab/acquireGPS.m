function [fq_est,cp_est,c_meas,testmat]=acquireGPS(sv,freq,maxdop,rawdat,fs);
%function [fq_est,cp_est,c_meas,testmat]=acquireGPS(sv,freq,maxdop,rawdat,fs);
%
% This function does the circular convolution approach to GNSS signal
% acquisition (based on the paper from Delft U.).  It takes as input:
%
%		sv:		the particular GPS-SPS C/A code or if sv==0 then the GLONASS
%				m-sequence is used.
%
%		freq:	the expected carrier frequency (in Hz)
%
%		maxdop:	the maximum deviation from the expected carrier frequency as
%				a result of doppler and LO frequency drift
%
%		rawdat:	the raw GNSS CMDA data, the program requires at least two full
%				code periods of data
%
%		fs:		the sampling frequency at which the data was collected
%
%outputs
%		fq_est:	the best estimate of true carrier frequency within plus/minus
%				(250 Hz/ncp).
%
%		cp_est:	the best estimate of code phase, specifically this give the
%				sample number (index) in the rawdat array where the code period
%				actually starts
%
%		c_meas:	a confidence measure, this is the ratio of the average of the
%				maximum value along with its two direct neighbors values to the
%				average of the remaining points calculated from being > 1/2 chip
%				away
%
% GNSS Laboratory  - Univ of Colorado  -  Jan 2004

nss=round((1.0e-3)*fs);ns=2.0*nss;
[aa,bb]=size(rawdat);
if (bb<2*ns)
    disp('Not Enough Input Data to do Correlation...')
    fq_est=0;cp_est=0;c_meas=0;testmat=0;testmatfull=0;
    return
end

inda=0;indb=0;

rawdat1=rawdat(1,1:ns);rawdat2=rawdat(1,ns+1:2*ns);

freqstp=500;
freqbins=size((freq-maxdop:freqstp:freq+maxdop),2);

%genrate ca code and frequencies
ca=cacode(sv,fs,ns);
argvec =(i*2*pi).*(0:1/fs:(ns-1)/fs);

%peform operation
testmat=zeros(1,nss);fq_est=0;
tempa=zeros(1,ns);tempb=zeros(1,ns);
tempc=zeros(1,ns);tempd=zeros(1,ns);
tempe=zeros(1,ns);tempf=zeros(1,ns);
indb=1;
tempa=fft(ca+i*ca);

%test different frequencies
for inda=freq-maxdop:freqstp:freq+maxdop
    tempb = exp(inda .* argvec); 
    tempc = (fft(rawdat1 .* tempb));tempd = (fft(rawdat2 .* tempb));
    tempe = abs(ifft(tempa .* conj(tempc)));tempf = abs(ifft(tempa .* conj(tempd)));
    if (max(tempe(1,1:nss))<max(tempf(1,1:nss)))
        tempe(1,:)=tempf(1,:);
    end
    if (max(testmat(1,:))<max(tempe(1,1:nss)))
        testmat(1,:)=tempe(1,1:nss);
        fq_est=inda;
    end
    indb=indb+1;
end

%sample in a half chip
nschip=round(fs/1.023e6);

%get the desired maximums
[tempb,tempc]=max(testmat);
cp_est= nss-tempc+1; 

%%% Now provide metric if the signal is detected or not; first get the max value
tempa=testmat(1,tempc);
% find the 2nd highest max more than 1.0 chips away, dependent on where max was found
if ((tempc > (nschip+1)) & (tempc < (nss-nschip)))
    tempb=max([testmat(1,1:tempc-(nschip+1)) testmat(1,tempc+(nschip+1):nss)]);
elseif (tempc <= (nschip+1))
    tempb=max(testmat(1,tempc+(nschip+1):nss-((nschip+1)-tempc)));
else
    tempb=max(testmat(1,1+tempc+nschip+1-nss:tempc-(nschip+1)));
end
c_meas=tempa/tempb;

