%	PersonalFME - Gateway linking analog radio selcalls to internet communication services
% Copyright(C) 2010-2022 Ralf Rettig (www.personalfme.de)
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

% support for unit testing
dataFileName = 'signal.txt';

% save parameters
save( 'downsamplingFactor.txt', 'downsamplingFactor', '-ascii', '-double', '-tabs' );
save( 'upsamplingFactor.txt', 'upsamplingFactor', '-ascii', '-double', '-tabs' );
save( 'a.txt', 'a', '-ascii', '-double', '-tabs' );
save( 'b.txt', 'b', '-ascii', '-double', '-tabs' );
save( 'isFIR.txt', 'isFIR', '-ascii', '-double', '-tabs' );

% load data
signal = load( dataFileName );

% perform upsampling
if ( upsamplingFactor > 1 )
	origIndices = 1:size(signal,1);
	upsamplingIndices = origIndices(1) : 1 / upsamplingFactor : origIndices(end);
	upsampledSignal = interp1( origIndices, signal, upsamplingIndices );
else
	upsampledSignal = signal;
end

% perform filtering and downsampling
if ( ( downsamplingFactor > 1 ) || ( ( downsamplingFactor == 1 ) && ( upsamplingFactor == 1 ) ) )
	filteredSignal = filter( b, a, upsampledSignal );
	resampledSignal = filteredSignal( 1 : downsamplingFactor : end );
else
	resampledSignal = upsampledSignal;
end

% save results
save( 'matlabProcessed.txt', 'resampledSignal', '-ascii', '-double', '-tabs' );