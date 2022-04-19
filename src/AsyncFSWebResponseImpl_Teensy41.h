/****************************************************************************************************************************
  AsyncFSWebResponseImpl_Teensy41.h - Dead simple AsyncFSWebServer for Teensy41 QNEthernet
  
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

#pragma once

#ifndef ASYNCWEBSERVERRESPONSEIMPL_TEENSY41_H_
#define ASYNCWEBSERVERRESPONSEIMPL_TEENSY41_H_

#ifdef Arduino_h
  // arduino is not compatible with std::vector
  #undef min
  #undef max
#endif

#include <vector>
// It is possible to restore these defines, but one can use _min and _max instead. Or std::min, std::max.


class AsyncBasicResponse: public AsyncWebServerResponse
{
  private:
    String _content;

  public:
    AsyncBasicResponse(int code, const String& contentType = String(), const String& content = String());
    void _respond(AsyncWebServerRequest *request);

    size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time);

    bool _sourceValid() const
    {
      return true;
    }
};

class AsyncAbstractResponse: public AsyncWebServerResponse
{
  private:
    String _head;
    // Data is inserted into cache at begin().
    // This is inefficient with vector, but if we use some other container,
    // we won't be able to access it as contiguous array of bytes when reading from it,
    // so by gaining performance in one place, we'll lose it in another.
    std::vector<uint8_t> _cache;
    size_t _readDataFromCacheOrContent(uint8_t* data, const size_t len);
    size_t _fillBufferAndProcessTemplates(uint8_t* buf, size_t maxLen);

  protected:
    AwsTemplateProcessor _callback;

  public:
    AsyncAbstractResponse(AwsTemplateProcessor callback = nullptr);
    void _respond(AsyncWebServerRequest *request);
    size_t _ack(AsyncWebServerRequest *request, size_t len, uint32_t time);

    bool _sourceValid() const
    {
      return false;
    }

    virtual size_t _fillBuffer(uint8_t *buf __attribute__((unused)), size_t maxLen __attribute__((unused)))
    {
      return 0;
    }
};

#ifndef TEMPLATE_PLACEHOLDER
  #define TEMPLATE_PLACEHOLDER '%'
#endif

#define TEMPLATE_PARAM_NAME_LENGTH 32

class AsyncFileResponse: public AsyncAbstractResponse 
{
  private:
    File _content;
    String _path;
    void _setContentType(const String& path);
    
  public:
   
    AsyncFileResponse(FS* fs, const String& path, const String& contentType=String(), bool download=false, 
                      AwsTemplateProcessor callback=nullptr);                                
                      
    AsyncFileResponse(File content, const String& path, const String& contentType=String(), bool download=false, 
                      AwsTemplateProcessor callback=nullptr);
    ~AsyncFileResponse();
    
    bool _sourceValid() /*const */
    { 
      // KH TODO
      //return !!(_content);
      return(_content.available() != 0);
      //return (true); 
    }
    
    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

class AsyncStreamResponse: public AsyncAbstractResponse
{
  private:
    Stream *_content;

  public:
    AsyncStreamResponse(Stream &stream, const String& contentType, size_t len, AwsTemplateProcessor callback = nullptr);

    bool _sourceValid() const
    {
      return !!(_content);
    }

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

class AsyncCallbackResponse: public AsyncAbstractResponse
{
  private:
    AwsResponseFiller _content;
    size_t _filledLength;

  public:
    AsyncCallbackResponse(const String& contentType, size_t len, AwsResponseFiller callback, AwsTemplateProcessor templateCallback = nullptr);

    bool _sourceValid() const
    {
      return !!(_content);
    }

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

class AsyncChunkedResponse: public AsyncAbstractResponse
{
  private:
    AwsResponseFiller _content;
    size_t _filledLength;

  public:
    AsyncChunkedResponse(const String& contentType, AwsResponseFiller callback, AwsTemplateProcessor templateCallback = nullptr);

    bool _sourceValid() const
    {
      return !!(_content);
    }

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

class cbuf;

class AsyncProgmemResponse: public AsyncAbstractResponse 
{
  private:
    const uint8_t * _content;
    size_t _readLength;
    
  public:
    AsyncProgmemResponse(int code, const String& contentType, const uint8_t * content, size_t len, AwsTemplateProcessor callback=nullptr);
    bool _sourceValid() const { return true; }
    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
};

class AsyncResponseStream: public AsyncAbstractResponse, public Print
{
  private:
    cbuf *_content;

  public:
    AsyncResponseStream(const String& contentType, size_t bufferSize);
    ~AsyncResponseStream();

    bool _sourceValid() const
    {
      return (_state < RESPONSE_END);
    }

    virtual size_t _fillBuffer(uint8_t *buf, size_t maxLen) override;
    size_t write(const uint8_t *data, size_t len);
    size_t write(uint8_t data);
    using Print::write;
};

#endif /* ASYNCWEBSERVERRESPONSEIMPL_TEENSY41_H_ */
