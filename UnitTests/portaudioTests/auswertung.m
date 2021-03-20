%	PersonalFME - Gateway linking analog radio selcalls to internet communication services
% Copyright(C) 2010-2021 Ralf Rettig (www.personalfme.de)
%
% This program is free software: you can redistribute it and / or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with this program.If not, see <http://www.gnu.org/licenses/>

sampleFreq = 44100;
load test.txt
time = ( 1:size(test,1) ) ./ sampleFreq; 
signal = test(:,1);
plot( time, signal )

% spectral analysis
length = size(signal,1);
spectrum = fft(signal, length) / size(signal,1);
spectrum = 2*abs( (spectrum(1:length/2+1) ) );
f = sampleFreq / 2 * linspace( 0, 1, length / 2 + 1);
spectrum = sgolayfilt( spectrum, 3, 51 );
spectrum = spectrum ./ max(spectrum);

%plot( f, spectrum );
%axis([1000 2800 0 1]);