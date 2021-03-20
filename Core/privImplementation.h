/*	PersonalFME - Gateway linking analog radio selcalls to internet communication services
Copyright(C) 2010-2021 Ralf Rettig (www.personalfme.de)

This program is free software: you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.If not, see <http://www.gnu.org/licenses/>
*/
#pragma once
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <boost/signals2.hpp>
#include "SeqData.h"
#include "Search.h"
#include "AudioInput.h"
#include "AudioInputParam.h"
#include "AudioSignalPreserver.h"
#include "SequencePasser.h"
#include "AudioSignalReader.h"
#include "AudioFullDownsampler.h"
#include "FIRfilter.h"

/*@{*/
/** \ingroup Core
*/

/**	\ingroup Core
*	Pimple idiom for hiding the private implementation details of the CAudioInput class
*/
class Core::CAudioInput::CPrivImplementation {
public:
	CPrivImplementation(void) : isRecording(false), isInit(false) {};
	virtual ~CPrivImplementation(void){};
	void SetParameters(Processing::CAudioDevice device, std::string audioSettingsFileName, std::function<void(const Utilities::CSeqData&)> foundCallback, std::function<void(const std::string&)> runtimeErrorCallback, std::shared_ptr<RecordingParam> recordingParams);
	void SetFileNames(const std::string& parameterFileName, const std::string& specializedParameterFileName);
	void GetAvailableAudioDevices( std::vector<Processing::CAudioDevice>& devices, Processing::CAudioDevice& stdDevice, double& maxStandardSamplingFreq, const boost::filesystem::path& audioSettingsFile );
	bool IsDeviceAvailable( double& maxStandardSamplingFreq, const Processing::CAudioDevice& device, const boost::filesystem::path& audioSettingsFile ) const;
	void SetAudioDevice(const Processing::CAudioDevice& device);
	void StartAudioInput(void);
	void StopAudioInput(void);
	bool IsRunning(void);
	void GetBasicInformation(double& samplingFreqProcessing, double& samplingFreqInput, std::string& parameterFileName, std::string& specializedParameterFileName) const;
	void ResetSearcher(std::shared_ptr< Core::General::CSearch<float> > newSearcher);
	void ResetSequencePasser(std::shared_ptr< Core::Audio::CSequencePasser<float> > newSequencePasser);
	void InitializeAudioReader(const Processing::CAudioDevice& device, const std::string& audioSettingsFileName, std::function<void(const std::string&)> runtimeErrorCallback, const double& samplingFreqInput);
	static void GetPortaudioVersion(std::string& versionString, int& buildNumber, std::string& licenseText);
	static void GetAlglibVersion(std::string& versionString, std::string& dateString, std::string& licenseText);
protected:
	void MainThread(void);
	static void LoadParameters(std::string audioSettingsFileName, Core::CAudioInputParam &params);
	static std::vector<double> GetPossibleSamplingFreqs(const Processing::CAudioDevice& device, const int& numChannels, const std::vector<double>& standardSamplingFreqs);
	void GetAudioReaderParams(const Processing::CAudioDevice& device, const std::string& audioSettingsFileName, double& samplingFreqInput, int& downsamplingFactorProc, double& cutoffFreqProc, int& downsamplingFactorRec, double& cutoffFreqRec, const double& requestedRecSamplingFreq = Core::NO_RECORDING);
	static void GetBestWorkingParameters(const std::vector<double>& possibleSamplingFreqs, const double& maxRequiredProcFreq, const double& requestedRecSamplingFreq, double& samplingFreqInput, int& downsamplingFactorProc, double& cutoffFreqProc, int& downsamplingFactorRec, double& cutoffFreqRec);
	static void GetRelevantAudioSettings(const std::string& audioSettingsFileName, double& transWidthProc, double& transWidthRec, float& mainThreadCycleTime);
	
	Audio::CAudioSignalPreserver<float> dataPreserver;
	Audio::CAudioSignalReader<float> dataReader;
	Processing::Filter::CFIRfilter<float> simpleDownsampler;
	Audio::CAudioFullDownsampler<float> fullDownsampler;
	std::shared_ptr< Audio::CSequencePasser<float> > sequencePasser;
	std::shared_ptr< Core::General::CSearch<float> > searchCode;
	float mainThreadCycleTime;
	double samplingFreqProcessing;
	double samplingFreqInput;
	std::string parameterFileName;
	std::string specializedParameterFileName;
	bool isRecording;
	boost::signals2::signal < void ( const std::string& ) > runtimeErrorSignal;
private:
	CPrivImplementation(const CPrivImplementation &);					// prevent copying
    CPrivImplementation & operator= (const CPrivImplementation &);		// prevent assignment

	mutable boost::mutex mainThreadMutex;
	std::unique_ptr<boost::thread> threadMain;
	mutable boost::shared_mutex parameterMutex;
	bool isInit;
};
/*@}*/