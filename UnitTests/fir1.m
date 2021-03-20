function [b,a] = fir1(N,Wn,Ftype,Wind)
%FIR1 FIR filter design using the window method.
% B = FIR1(N,Wn) designs an N'th order lowpass FIR digital filter
% and returns the filter coefficients in length N+1 vector B.
% The cut-off frequency Wn must be between 0 < Wn < 1.0, with 1.0
% corresponding to half the sample rate.
%
% If Wn is a two-element vector, Wn = [W1 W2], FIR1 returns an
% order N bandpass filter with passband W1 < W < W2.
% B = FIR1(N,Wn,'high') designs a highpass filter.
% B = FIR1(N,Wn,'stop') is a bandstop filter if Wn = [W1 W2].
% For highpass and bandstop filters, N must be even.
%
% By default FIR1 uses a Hamming window. Other available windows,
% including Boxcar, Hanning, Bartlett, Blackman, Kaiser and Chebwin
% can be specified with an optional trailing argument. For example,
% B = FIR1(N,Wn,bartlett(N+1)) uses a Bartlett window.
% B = FIR1(N,Wn,'high',chebwin(N+1,R)) uses a Chebyshev window.
%
% FIR1 is an M-file implementation of program 5.2 in the IEEE
% Programs for Digital Signal Processing tape. See also FIR2,
% FIRLS, REMEZ, BUTTER, CHEBY1, CHEBY2, YULEWALK, FREQZ and FILTER.

% Author(s): L. Shure
% L. Shure, 4-5-90, revised
% Copyright (c) 1984-94 by The MathWorks, Inc.
% $Revision: 1.6 $ $Date: 1994/01/25 17:59:09 $

% Reference(s):
% [1] "Programs for Digital Signal Processing", IEEE Press
% John Wiley & Sons, 1979, pg. 5.2-1.

nw = 0;
a = 1;
if nargin == 3
if ~isstr(Ftype);
	nw = max(size(Ftype));
	Wind = Ftype;
	Ftype = [];
end
elseif nargin == 4
	nw = max(size(Wind));
else
	Ftype = [];
end

Btype = 1;
if nargin > 2 & max(size(Ftype)) > 0
	Btype = 3;
end
if max(size(Wn)) == 2
	Btype = Btype + 1;
end

N = N + 1;
odd = rem(N, 2);
if (Btype == 3 | Btype == 4)
	if (~odd)
		disp('For highpass and bandstop filters, order must be even.')
		disp('Order is being increased by 1.')
		N = N + 1;
		odd = 1;
	end
end
if nw ~= 0 & nw ~= N
	error('The window length must be the same as the filter length.')
end
if nw > 0
	wind = Wind;
else % replace the following with the default window of your choice.
	wind = hamming(N);
end
%
% to use Chebyshev window, you must specify ripple
% ripple=60;
% wind=chebwin(N, ripple);
%
% to use Kaiser window, beta must be supplied
% att = 60; % dB of attenuation desired in sidelobe
% beta = .1102*(att-8.7);
% wind = kaiser(N,beta);

fl = Wn(1)/2;
if (Btype == 2 | Btype == 4)
	fh = Wn(2)/2;
	if (fl >= .5 | fl <= 0 | fh >= .5 | fh <= 0.)
		error('Frequencies must fall in range between 0 and 1.')
	end
	c1=fh-fl;
if (c1 <= 0)
	error('Wn(1) must be less than Wn(2).')
end
else
	c1=fl;
	if (fl >= .5 | fl <= 0)
		error('Frequency must lie between 0 and 1')
	end
end

nhlf = fix((N + 1)/2);
i1=1 + odd;

if Btype == 1 % lowpass
	if odd
		b(1) = 2*c1;
	end
	xn=(odd:nhlf-1) + .5*(1-odd);
	c=pi*xn;
	c3=2*c1*c;
	b(i1:nhlf)=(sin(c3)./c);
	b = real([b(nhlf:-1:i1) b(1:nhlf)].*wind(:)');
	gain = abs(polyval(b,1));
	b = b/gain;
	return;

elseif Btype ==2 % bandpass
	b(1) = 2*c1;
	xn=(odd:nhlf-1)+.5*(1-odd);
	c=pi*xn;
	c3=c*c1;
	b(i1:nhlf)=2*sin(c3).*cos(c*(fl+fh))./c;
	b=real([b(nhlf:-1:i1) b(1:nhlf)].*wind(:)');
	gain = abs(polyval(b,exp(sqrt(-1)*pi*(fl+fh))));
	b = b/gain;
	return;

elseif Btype == 3 % highpass
	b(1)=2*c1;
	xn=(odd:nhlf-1);
	c=pi*xn;
	c3=2*c1*c;
	b(i1:nhlf)=sin(c3)./c;
	att=60.;
	b=real([b(nhlf:-1:i1) b(1:nhlf)].*wind(:)');
	b(nhlf)=1-b(nhlf);
	b(1:nhlf-1)=-b(1:nhlf-1);
	b(nhlf+1:N)=-b(nhlf+1:N);
	gain = abs(polyval(b,-1));
	b = b/gain;
	return;

elseif Btype == 4 % bandstop
	b(1) = 2*c1;
	xn=(odd:nhlf-1)+.5*(1-odd);
	c=pi*xn;
	c3=c*c1;
	b(i1:nhlf)=2*sin(c3).*cos(c*(fl+fh))./c;
	b=real([b(nhlf:-1:i1) b(1:nhlf)].*wind(:)');
	b(nhlf)=1-b(nhlf);
	b(1:nhlf-1)=-b(1:nhlf-1);
	b(nhlf+1:N)=-b(nhlf+1:N);
	gain = abs(polyval(b,1));
	b = b/gain;
	return;
end