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

import json

import numpy as np
import pandas as pd
from scipy.interpolate import interp1d
from scipy.signal import lfilter


def create_test_reference_data(down_sampling_factor, up_sampling_factor, a, b, is_fir, data_file_name,
                               reference_file_path):
    df = pd.read_csv(data_file_name, header=None, names=['y'])
    signal = df['y']

    # perform up-sampling
    if up_sampling_factor > 1:
        orig_indices = range(len(signal))
        up_sampling_indices = np.arange(orig_indices[0], orig_indices[-1], 1 / up_sampling_factor)
        interp = interp1d(orig_indices, signal)
        up_sampled_signal = interp(up_sampling_indices)
    else:
        up_sampled_signal = signal

    # perform filtering and down-sampling
    if down_sampling_factor > 1 or (down_sampling_factor == 1 and up_sampling_factor == 1):
        filtered_signal = lfilter(b, a, up_sampled_signal)
        reference_data = filtered_signal[::down_sampling_factor]
    else:
        reference_data = up_sampled_signal
    data = {
        'down_sampling_factor': down_sampling_factor,
        'up_sampling_factor': up_sampling_factor,
        'a': list(a),
        'b': list(b),
        'is_fir': is_fir,
        'reference_data': list(reference_data)
    }
    with open(reference_file_path, 'w') as json_file:
        json_file.write(json.dumps(data, indent=4))
