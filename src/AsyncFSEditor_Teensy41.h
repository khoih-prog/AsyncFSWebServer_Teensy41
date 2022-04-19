/****************************************************************************************************************************
  AsyncFSEditor_Teensy41.h - Dead simple AsyncFSWebServer for Teensy41 QNEthernet
  
  For Teensy41 with QNEthernet using Teensy FS (SD, PSRAM, SQI/QSPI Flash, etc.)
   
  AsyncFSWebServer_Teensy41 is a library for the Teensy41 with QNEthernet
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncFSWebServer_Teensy41
  
  Copyright (c) 2016 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
  as published bythe Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
  You should have received a copy of the GNU General Public License along with this program.  If not, see <https://www.gnu.org/licenses/>.  
 
  Version: 1.4.1

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.4.1   K Hoang      19/04/2022 Initial coding for Teensy 4.1 using built-in QNEthernet.
                                  Bump up version to v1.4.1 to sync with AsyncWebServer_STM32 v1.4.1
 *****************************************************************************************************************************/

#ifndef AsyncFSEditor_H_
#define AsyncFSEditor_H_

#include "AsyncFSWebServer_Teensy41.hpp"

#include <LittleFS.h>
#include <SD.h>
#include <SPI.h>

#include <AsyncFSWebServer_Teensy41.hpp>

class AsyncFSEditor: public AsyncWebHandler 
{
  private:

    FS* _fs;
   
    String _username;
    String _password; 
    bool _authenticated;
    uint32_t _startTime;
    
  public:

		AsyncFSEditor(FS* fs, const String& username=String(), const String& password=String());		

    virtual bool canHandle(AsyncWebServerRequest *request) override final;
    virtual void handleRequest(AsyncWebServerRequest *request) override final;
    virtual void handleUpload(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, 
                              size_t len, bool final) override final;
    virtual bool isRequestHandlerTrivial() override final {return false;}
};

#endif // AsyncFSEditor_H_
