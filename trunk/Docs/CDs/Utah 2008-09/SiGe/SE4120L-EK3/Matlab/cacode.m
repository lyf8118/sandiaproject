function ca=cacode(svnum,fs,numsamp)
%function ca=cacode(svnum,fs,numsamp);
%  function to generate any of the 32 GPS C/A codes at a user
% specified sampling frequency.
%
% Input Arguments:
% svnum - the Satellite's PRN number
%         1-32 is traditional GPS PRN numbers
%        
% fs - desired sampling frequency 
% numsamp - number of samples to generate
%
% Output Argument
% ca - a vector containing the desired output sequence
%
% GNSS Laboratory  -  Univ of Colorado - Aug 2003

% Constants
coderate=1.023e6;  

g2s = [5;6;7;8;17;18;139;140;141;251;252;254;255;256;257;258;469;470;471; ...
       472;473;474;509;512;513;514;515;516;859;860;861;862;145;52;886;1012;130];

g2shift=g2s(svnum,1);
           
% Generate G1 code
     %   load shift register
          reg = -1*ones(1,10);
     %
     for i = 1:1023,
         g1(i) = reg(10);
         save1 = reg(3)*reg(10);
         reg(1,2:10) = reg(1:1:9);
         reg(1) = save1;
     end,
%
% Generate G2 code
%
     %   load shift register
           reg = -1*ones(1,10);
     %
     for i = 1:1023,
         g2(i) = reg(10);
         save2 = reg(2)*reg(3)*reg(6)*reg(8)*reg(9)*reg(10);
         reg(1,2:10) = reg(1:1:9);
         reg(1) = save2;
     end,
     %
     %    Shift G2 code
     %
     g2tmp(1,1:g2shift)=g2(1,1023-g2shift+1:1023);
     g2tmp(1,g2shift+1:1023)=g2(1,1:1023-g2shift);
     %
     g2 = g2tmp;
%
%  Form single sample C/A code by multiplying G1 and G2
%
ss_ca = g1.*g2;

cainseq=ceil(numsamp/((fs/coderate)*1023));
caholder=zeros(1,cainseq);
indb=1;
for inda=1:cainseq
   caholder(1,indb:indb+1022)=ss_ca;
   indb=indb+1023; 
end
indc=0:numsamp-1;
caindex=floor(coderate*indc/fs);
ca=caholder(caindex+1);
