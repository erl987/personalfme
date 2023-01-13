#   PersonalFME - Gateway linking analog radio selcalls to internet communication services
# Copyright(C) 2010-2023 Ralf Rettig (www.personalfme.de)
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

"""Plot test case results for the quality of the FME-detection software"""

import pandas as pd
from matplotlib import pyplot as plt

TEST_PARAM_FILE_PATH = '../../build/UnitTests/fmeDetectionTests/testRequirements.txt'  # test input parameters
TEST_RESULT_FILE_PATH = '../../build/UnitTests/fmeDetectionTests/testResults.txt'  # test results

# load test data
parameters_df = pd.read_csv(TEST_PARAM_FILE_PATH, sep='\t', header=0)
result_df = pd.read_csv(TEST_RESULT_FILE_PATH, sep='\t', header=0)

# extract parameters to be plotted
x = parameters_df['deltaF1']
y = parameters_df['SNR']
test_is_successful = result_df['isSuccessfull']

# plotting
fail_indices = []
success_indices = []
for index, is_successful in enumerate(test_is_successful):
    if is_successful == 0:
        fail_indices.append(index)
    else:
        success_indices.append(index)

fig = plt.figure()
ax = fig.add_subplot(projection='3d')
ax.scatter(x[fail_indices], y[fail_indices], 0, 'ob')
ax.scatter(x[success_indices], y[success_indices], 1, 'or')

plt.title('Multiple detections result')
ax.set_xlabel('deltaF1 / Hz')
ax.set_ylabel('SNR')
ax.set_zlabel('is successful')

plt.show()
