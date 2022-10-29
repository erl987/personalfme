#	PersonalFME - Gateway linking analog radio selcalls to internet communication services
# Copyright(C) 2010-2022 Ralf Rettig (www.personalfme.de)
#
# This program is free software: you can redistribute it and / or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.If not, see <http://www.gnu.org/licenses/>

from scipy.signal import cheby1, cheby2, firwin

from reference_data import create_test_reference_data

DATA_FILE_NAME = 'test-signal.txt'

# Data for testing IIR filter with high FC
reference_file_path = 'iir-filter-high-fc.json'
down_sampling_factor = 3
up_sampling_factor = 2
is_fir = False
# noinspection PyTupleAssignmentBalance
b, a = cheby1(8, 0.05, 0.5333)
create_test_reference_data(down_sampling_factor, up_sampling_factor, a, b, is_fir, DATA_FILE_NAME, reference_file_path)

# Data for testing FIR filter with up-sampling
reference_file_path = 'fir-filter-up-sampling.json'
down_sampling_factor = 1
up_sampling_factor = 3
is_fir = True
a = [1]
b = firwin(30, 0.6666)
create_test_reference_data(down_sampling_factor, up_sampling_factor, a, b, is_fir, DATA_FILE_NAME, reference_file_path)

# Data for testing IIR filter with up-sampling
reference_file_path = 'iir-filter-up-sampling.json'
down_sampling_factor = 1
up_sampling_factor = 3
is_fir = False
a = [1]
b = [1]
create_test_reference_data(down_sampling_factor, up_sampling_factor, a, b, is_fir, DATA_FILE_NAME, reference_file_path)

# Data for testing FIR filter with high FC
reference_file_path = 'fir-filter-high-fc.json'
down_sampling_factor = 1
up_sampling_factor = 1
is_fir = True
a = [1]
b = firwin(20, 0.3)
create_test_reference_data(down_sampling_factor, up_sampling_factor, a, b, is_fir, DATA_FILE_NAME, reference_file_path)

# Data for testing FIR filter with low FC
reference_file_path = 'fir-filter-low-fc.json'
down_sampling_factor = 1
up_sampling_factor = 1
is_fir = True
a = [1]
b = firwin(20, 0.1)
create_test_reference_data(down_sampling_factor, up_sampling_factor, a, b, is_fir, DATA_FILE_NAME, reference_file_path)

# Data for testing IIR filter with low FC
reference_file_path = 'iir-filter-low-fc.json'
down_sampling_factor = 1
up_sampling_factor = 1
is_fir = False
b, a = cheby2(20, 40, 0.2)
create_test_reference_data(down_sampling_factor, up_sampling_factor, a, b, is_fir, DATA_FILE_NAME, reference_file_path)

# Data for testing FIR filter with down-sampling
reference_file_path = 'fir-filter-down-sampling.json'
down_sampling_factor = 3
up_sampling_factor = 1
is_fir = True
a = [1]
b = firwin(30, 0.333)
create_test_reference_data(down_sampling_factor, up_sampling_factor, a, b, is_fir, DATA_FILE_NAME, reference_file_path)

# Data for testing IIR filter with down-sampling
reference_file_path = 'iir-filter-down-sampling.json'
down_sampling_factor = 3
up_sampling_factor = 1
is_fir = False
b, a = cheby1( 8, 0.05, 0.2666 )
create_test_reference_data(down_sampling_factor, up_sampling_factor, a, b, is_fir, DATA_FILE_NAME, reference_file_path)

# Data for testing FIR filter with re-sampling
reference_file_path = 'fir-filter-re-sampling.json'
down_sampling_factor = 3
up_sampling_factor = 2
is_fir = True
a = [1]
b = firwin(30, 0.666)
create_test_reference_data(down_sampling_factor, up_sampling_factor, a, b, is_fir, DATA_FILE_NAME, reference_file_path)

# Data for testing IIR filter with re-sampling
reference_file_path = 'iir-filter-re-sampling.json'
down_sampling_factor = 3
up_sampling_factor = 2
is_fir = False
b, a = cheby1(8, 0.05, 0.5333)
create_test_reference_data(down_sampling_factor, up_sampling_factor, a, b, is_fir, DATA_FILE_NAME, reference_file_path)

# Data for filter design test case
reference_file_path = 'filter-design.json'
down_sampling_factor = 3
up_sampling_factor = 1  # dummy value
is_fir = False  # dummy value

filter_order = 30  # this is a standard setting of the C++ class CFIRfilter<T>

# standard choice for the CFIRfilter<T>-class
wn = up_sampling_factor / down_sampling_factor
a = [1]
b = firwin(filter_order, wn)
create_test_reference_data(down_sampling_factor, up_sampling_factor, a, b, is_fir, DATA_FILE_NAME, reference_file_path)
