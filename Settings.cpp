/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2015 Charles J. Cliffe

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "SoapyAirspy.hpp"

SoapyAirspy::SoapyAirspy(const SoapySDR::Kwargs &args)
{
    deviceId = -1;

    sampleRate = 48000;
    centerFrequency = 0;

    numBuffers = DEFAULT_NUM_BUFFERS;

    agcMode = false;

    bufferedElems = 0;
    resetBuffer = false;
    
    streamActive = false;
    sampleRateChanged.store(false);
    
    if (args.count("device_id") != 0)
    {
        try {
            deviceId = std::stoi(args.at("device_id"));
        } catch (const std::invalid_argument &) {
        }
        
        // int numDevices = dac.getDeviceCount();
        
        // if (deviceId < 0 || deviceId >= numDevices)
        // {
        //     throw std::runtime_error(
        //             "device_id out of range [0 .. " + std::to_string(numDevices) + "].");
        // }
  
        SoapySDR_logf(SOAPY_SDR_DEBUG, "Found Airspy device using 'device_id' = %d", deviceId);
    }
    
    if (deviceId == -1) {
        throw std::runtime_error("device_id missing.");
    }

    // RtAudio endac;
    //
    // devInfo = endac.getDeviceInfo(deviceId);
}

SoapyAirspy::~SoapyAirspy(void)
{
}

/*******************************************************************
 * Identification API
 ******************************************************************/

std::string SoapyAirspy::getDriverKey(void) const
{
    return "Airspy";
}

std::string SoapyAirspy::getHardwareKey(void) const
{
    return "Airspy";
}

SoapySDR::Kwargs SoapyAirspy::getHardwareInfo(void) const
{
    //key/value pairs for any useful information
    //this also gets printed in --probe
    SoapySDR::Kwargs args;

    args["origin"] = "https://github.com/pothosware/SoapyAirspy";
    args["device_id"] = std::to_string(deviceId);

    return args;
}

/*******************************************************************
 * Channels API
 ******************************************************************/

size_t SoapyAirspy::getNumChannels(const int dir) const
{
    return (dir == SOAPY_SDR_RX) ? 1 : 0;
}

/*******************************************************************
 * Antenna API
 ******************************************************************/

std::vector<std::string> SoapyAirspy::listAntennas(const int direction, const size_t channel) const
{
    std::vector<std::string> antennas;
    antennas.push_back("RX");
    // antennas.push_back("TX");
    return antennas;
}

void SoapyAirspy::setAntenna(const int direction, const size_t channel, const std::string &name)
{
    // TODO
}

std::string SoapyAirspy::getAntenna(const int direction, const size_t channel) const
{
    return "RX";
    // return "TX";
}

/*******************************************************************
 * Frontend corrections API
 ******************************************************************/

bool SoapyAirspy::hasDCOffsetMode(const int direction, const size_t channel) const
{
    return false;
}

/*******************************************************************
 * Gain API
 ******************************************************************/

std::vector<std::string> SoapyAirspy::listGains(const int direction, const size_t channel) const
{
    //list available gain elements,
    //the functions below have a "name" parameter
    std::vector<std::string> results;

    // results.push_back("TUNER");

    return results;
}

bool SoapyAirspy::hasGainMode(const int direction, const size_t channel) const
{
    return true;
}

void SoapyAirspy::setGainMode(const int direction, const size_t channel, const bool automatic)
{
    agcMode = automatic;
    SoapySDR_logf(SOAPY_SDR_DEBUG, "Setting AGC: %s", automatic ? "Automatic" : "Manual");
}

bool SoapyAirspy::getGainMode(const int direction, const size_t channel) const
{
    return agcMode;
}

void SoapyAirspy::setGain(const int direction, const size_t channel, const double value)
{
    //set the overall gain by distributing it across available gain elements
    //OR delete this function to use SoapySDR's default gain distribution algorithm...
    SoapySDR::Device::setGain(direction, channel, value);
}

void SoapyAirspy::setGain(const int direction, const size_t channel, const std::string &name, const double value)
{
    // if (name == "TUNER")
    // {
    //     audioGain = value;
    //     SoapySDR_logf(SOAPY_SDR_DEBUG, "Setting Audio Gain: %f", audioGain);
    // }
}

double SoapyAirspy::getGain(const int direction, const size_t channel, const std::string &name) const
{
    // if ((name.length() >= 2) && (name.substr(0, 2) == "TUNER"))
    // {
    //     return audioGain;
    // }

    return 0;
}

SoapySDR::Range SoapyAirspy::getGainRange(const int direction, const size_t channel, const std::string &name) const
{
    return SoapySDR::Range(0, 100);
}

/*******************************************************************
 * Frequency API
 ******************************************************************/

void SoapyAirspy::setFrequency(
        const int direction,
        const size_t channel,
        const std::string &name,
        const double frequency,
        const SoapySDR::Kwargs &args)
{
    if (name == "RF")
    {
        centerFrequency = (uint32_t) frequency;
        resetBuffer = true;
        SoapySDR_logf(SOAPY_SDR_DEBUG, "Setting center freq: %d", centerFrequency);
    }
}

double SoapyAirspy::getFrequency(const int direction, const size_t channel, const std::string &name) const
{
    if (name == "RF")
    {
        return (double) centerFrequency;
    }

    return 0;
}

std::vector<std::string> SoapyAirspy::listFrequencies(const int direction, const size_t channel) const
{
    std::vector<std::string> names;
    names.push_back("RF");
    return names;
}

SoapySDR::RangeList SoapyAirspy::getFrequencyRange(
        const int direction,
        const size_t channel,
        const std::string &name) const
{
    SoapySDR::RangeList results;
    if (name == "RF")
    {
        results.push_back(SoapySDR::Range(0, 6000000000));
    }
    return results;
}

SoapySDR::ArgInfoList SoapyAirspy::getFrequencyArgsInfo(const int direction, const size_t channel) const
{
    SoapySDR::ArgInfoList freqArgs;

    // TODO: frequency arguments

    return freqArgs;
}

/*******************************************************************
 * Sample Rate API
 ******************************************************************/

void SoapyAirspy::setSampleRate(const int direction, const size_t channel, const double rate)
{
    SoapySDR_logf(SOAPY_SDR_DEBUG, "Setting sample rate: %d", sampleRate);

    if (sampleRate != rate) {
        sampleRate = rate;
        resetBuffer = true;
        sampleRateChanged.store(true);
    }
}

double SoapyAirspy::getSampleRate(const int direction, const size_t channel) const
{
    return sampleRate;
}

std::vector<double> SoapyAirspy::listSampleRates(const int direction, const size_t channel) const
{
    std::vector<double> results;

    // RtAudio endac;
    // RtAudio::DeviceInfo info = endac.getDeviceInfo(deviceId);

    // std::vector<unsigned int>::iterator srate;
    //
    // for (srate = info.sampleRates.begin(); srate != info.sampleRates.end(); srate++) {
    //     results.push_back(*srate);
    // }

    return results;
}

void SoapyAirspy::setBandwidth(const int direction, const size_t channel, const double bw)
{
    SoapySDR::Device::setBandwidth(direction, channel, bw);
}

double SoapyAirspy::getBandwidth(const int direction, const size_t channel) const
{
    return SoapySDR::Device::getBandwidth(direction, channel);
}

std::vector<double> SoapyAirspy::listBandwidths(const int direction, const size_t channel) const
{
    std::vector<double> results;

    return results;
}

/*******************************************************************
 * Settings API
 ******************************************************************/

SoapySDR::ArgInfoList SoapyAirspy::getSettingInfo(void) const
{
    SoapySDR::ArgInfoList setArgs;
 
    // // Sample Offset
    // SoapySDR::ArgInfo sampleOffsetArg;
    // sampleOffsetArg.key = "sample_offset";
    // sampleOffsetArg.value = "0";
    // sampleOffsetArg.name = "Stereo Sample Offset";
    // sampleOffsetArg.description = "Offset stereo samples for off-by-one audio inputs.";
    // sampleOffsetArg.type = SoapySDR::ArgInfo::STRING;
    //
    // std::vector<std::string> sampleOffsetOpts;
    // std::vector<std::string> sampleOffsetOptNames;
    //
    // sampleOffsetOpts.push_back("-2");
    // sampleOffsetOptNames.push_back("-2 Samples");
    // sampleOffsetOpts.push_back("-1");
    // sampleOffsetOptNames.push_back("-1 Samples");
    // sampleOffsetOpts.push_back("0");
    // sampleOffsetOptNames.push_back("0 Samples");
    // sampleOffsetOpts.push_back("1");
    // sampleOffsetOptNames.push_back("1 Samples");
    // sampleOffsetOpts.push_back("2");
    // sampleOffsetOptNames.push_back("2 Samples");
    //
    // sampleOffsetArg.options = sampleOffsetOpts;
    // sampleOffsetArg.optionNames = sampleOffsetOptNames;
    //
    // setArgs.push_back(sampleOffsetArg);
 
    return setArgs;
}

void SoapyAirspy::writeSetting(const std::string &key, const std::string &value)
{
    // if (key == "sample_offset") {
    //     try {
    //         int sOffset = std::stoi(value);
    //
    //         if (sOffset >= -2 && sOffset <= 2) {
    //             sampleOffset = sOffset;
    //         }
    //     } catch (std::invalid_argument e) { }
    // }
}

std::string SoapyAirspy::readSetting(const std::string &key) const
{
    // if (key == "sample_offset") {
    //     return std::to_string(sampleOffset);
    // }
    
    // SoapySDR_logf(SOAPY_SDR_WARNING, "Unknown setting '%s'", key.c_str());
    return "";
}
