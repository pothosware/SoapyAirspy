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
#include <algorithm>

static std::vector<SoapySDR::Kwargs> findAirspy(const SoapySDR::Kwargs &args)
{
    std::vector<SoapySDR::Kwargs> results;
    
    airspy_lib_version_t asVersion;
    airspy_lib_version(&asVersion);
    
    SoapySDR_setLogLevel(SOAPY_SDR_DEBUG);
    
    SoapySDR_logf(SOAPY_SDR_DEBUG, "AirSpy Lib v%d.%d rev %d", asVersion.major_version, asVersion.minor_version, asVersion.revision);

    int numDevices = 0;
   
    std::vector< struct airspy_device * > foundDevices;
   
    int status = 0;
   
    // if (args.count("serial") != 0) {
    //     std::stringstream serialstr;
    //
    //     uint32_t serialNum[2];
    //     std::string serial_in(args.at("serial"));
    //     std::replace( serial_in.begin(), serial_in.end(), ':', ' ');
    //     serialstr.str(serial_in);
    //     serialstr << std::hex;
    //     serialstr >> serialNum[0];
    //     serialstr >> serialNum[1];
    //
    //     uint64_t serial64 = ((uint64_t) serialNum[0] << 32) | serialNum[1]);
    //
    //     SoapySDR_logf(SOAPY_SDR_DEBUG, "Serial? '%s' %u %u 64: %llu", serialstr.str().c_str(), serialNum[0], serialNum[1], serial64);
    //
    //     struct airspy_device *searchDev = nullptr;
    //     status = airspy_open_sn(&searchDev, serial64);
    //
    //     SoapySDR_logf(SOAPY_SDR_DEBUG, "Search done..");
    //
    //     if (status == AIRSPY_SUCCESS) {
    //         foundDevices.push_back(searchDev);
    //     } else {
    //         SoapySDR_logf(SOAPY_SDR_DEBUG, "Error finding by serial..");
    //     }
    // } else
    {    
        for (int i = 0, iMax = MAX_DEVICES; i < iMax; i++) {
            struct airspy_device *searchDev = nullptr;
            status = airspy_open(&searchDev);
        
            if (status != AIRSPY_SUCCESS) {
                break;
            }
        
            foundDevices.push_back(searchDev);
        }
    }
   
    SoapySDR_logf(SOAPY_SDR_DEBUG, "%d AirSpy boards found.", foundDevices.size());
    int devId = 0;
    
    for (std::vector< struct airspy_device * >::iterator i = foundDevices.begin(); i != foundDevices.end(); i++) {
        uint8_t id = AIRSPY_BOARD_ID_INVALID;
        airspy_read_partid_serialno_t serial;
        
        status = airspy_board_id_read(*i, &id);
		if (status != AIRSPY_SUCCESS) {
            continue;
		}
        
		status = airspy_board_partid_serialno_read(*i, &serial);
		if (status != AIRSPY_SUCCESS) {
			continue;
		}

        std::string boardName(airspy_board_id_name((enum airspy_board_id)id));
        std::stringstream serialstr;
        
        serialstr.str("");
        serialstr << std::hex << serial.serial_no[2] << ":" << serial.serial_no[3];
        
        SoapySDR_logf(SOAPY_SDR_DEBUG, "Serial %s", serialstr.str().c_str());        

        SoapySDR::Kwargs soapyInfo;

        soapyInfo["device_id"] = std::to_string(devId);
        soapyInfo["label"] = boardName + "(" + serialstr.str() + ")";
        soapyInfo["serial"] = serialstr.str();
        devId++;
                
        // if (args.count("serial") != 0) {
        //     if (args.at("serial") != soapyInfo.at("serial")) {
        //         continue;
        //     }
        //     SoapySDR_logf(SOAPY_SDR_DEBUG, "Found device by serial %s", soapyInfo.at("serial").c_str());
        // } else
        if (args.count("device_id") != 0) {
            if (args.at("device_id") != soapyInfo.at("device_id")) {
                continue;
            }
            SoapySDR_logf(SOAPY_SDR_DEBUG, "Found device by device_id %s", soapyInfo.at("device_id").c_str());
        }
        
        results.push_back(soapyInfo);
    }
   
    for (std::vector< struct airspy_device * >::iterator i = foundDevices.begin(); i != foundDevices.end(); i++) {
        airspy_close(*i);
    }

    return results;
}

static SoapySDR::Device *makeAirspy(const SoapySDR::Kwargs &args)
{
    return new SoapyAirspy(args);
}

static SoapySDR::Registry registerAirspy("airspy", &findAirspy, &makeAirspy, SOAPY_SDR_ABI_VERSION);
