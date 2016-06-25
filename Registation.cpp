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
#include <SoapySDR/Registry.hpp>
#include <cstdlib> //malloc

static std::vector<SoapySDR::Kwargs> findAirspy(const SoapySDR::Kwargs &args)
{
    std::vector<SoapySDR::Kwargs> results;

    // int numDevices = endac.getDeviceCount();
    //
    // for (int i = 0; i < numDevices; i++) {
    //     RtAudio::DeviceInfo info = endac.getDeviceInfo(i);
    //     SoapySDR::Kwargs soapyInfo;
    //
    //     soapyInfo["device_id"] = std::to_string(i);
    //     soapyInfo["label"] = info.name;
    //     // soapyInfo["default_output"] = info.isDefaultOutput ? "True" : "False";
    //     // soapyInfo["default_input"] = info.isDefaultInput ? "True" : "False";
    //
    //     if (args.count("device_id") != 0)
    //     {
    //         if (args.at("device_id") != soapyInfo.at("device_id"))
    //         {
    //             continue;
    //         }
    //         SoapySDR_logf(SOAPY_SDR_DEBUG, "Found device by device_id %s", soapyInfo.at("device_id").c_str());
    //     }
    //
    //     results.push_back(soapyInfo);
    // }

    return results;
}

static SoapySDR::Device *makeAirspy(const SoapySDR::Kwargs &args)
{
    return new SoapyAirspy(args);
}

static SoapySDR::Registry registerAirspy("airspy", &findAirspy, &makeAirspy, SOAPY_SDR_ABI_VERSION);
