/****************************************************************************************************************************
  AsyncFSWebServer.ino
  
  Dead simple AsyncFSWebServer for Teensy41 QNEthernet
  
  For Teensy41 with QNEthernet using Teensy FS (SD, PSRAM, SQI/QSPI Flash, etc.)
   
  AsyncFSWebServer_Teensy41 is a library for the Teensy41 with QNEthernet
  
  Based on and modified from ESPAsyncWebServer (https://github.com/me-no-dev/ESPAsyncWebServer)
  Built by Khoi Hoang https://github.com/khoih-prog/AsyncFSWebServer_Teensy41
  Licensed under GPLv3 license
 *****************************************************************************************************************************/

#include "defines.h"

#include <LittleFS.h>

#include <AsyncFSEditor_Teensy41.h>

// SKETCH BEGIN
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %ld :)", client->id());
    client->ping();
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  }
  else if (type == WS_EVT_ERROR)
  {
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  }
  else if (type == WS_EVT_PONG)
  {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char*)data : "");
  }
  else if (type == WS_EVT_DATA)
  {
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    String msg = "";

    if (info->final && info->index == 0 && info->len == len)
    {
      //the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

      if (info->opcode == WS_TEXT)
      {
        for (size_t i = 0; i < info->len; i++)
        {
          msg += (char) data[i];
        }
      }
      else
      {
        char buff[3];

        for (size_t i = 0; i < info->len; i++)
        {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }

      Serial.printf("%s\n", msg.c_str());

      if (info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    }
    else
    {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if (info->index == 0)
      {
        if (info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");

        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

      if (info->opcode == WS_TEXT)
      {
        for (size_t i = 0; i < len; i++)
        {
          msg += (char) data[i];
        }
      }
      else
      {
        char buff[3];

        for (size_t i = 0; i < len; i++)
        {
          sprintf(buff, "%02x ", (uint8_t) data[i]);
          msg += buff ;
        }
      }

      Serial.printf("%s\n", msg.c_str());

      if ((info->index + len) == info->len)
      {
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);

        if (info->final)
        {
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");

          if (info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}

const char * hostName = "esp-async";
const char* http_username = "admin";
const char* http_password = "admin";

#define USING_PSRAM                 true
#define USING_SPI_FLASH             false
#define USING_SPI_FRAM              false
#define USING_PROGRAM_FLASH         false
#define USING_QSPI_FLASH            false
#define USING_SPI_NAND              false
#define USING_QPI_NAND              false

#if USING_PSRAM

  #define MYPSRAM 8         // compile time PSRAM size
  #define MYBLKSIZE 2048    // 2048
  
  EXTMEM char buf[MYPSRAM * 1024 * 1024];  // USE DMAMEM for more memory than ITCM allows - or remove
  
  char szDiskMem[] = "RAM_DISK";
  
  LittleFS_RAM myfs;
  
  #warning USING_PSRAM
  
  #elif USING_SPI_FLASH
  #define chipSelect        6
  
  LittleFS_SPIFlash myfs;
  
  char szDiskMem[] = "SPI_DISK";

  #warning USING_SPI_FLASH

#elif USING_SPI_FRAM

  LittleFS_SPIFram myfs;
  
  char szDiskMem[] = "FRAM_DISK";
  
  #warning USING_SPI_FRAM

#elif USING_PROGRAM_FLASH

  LittleFS_Program myfs;
  
  char szDiskMem[] = "PRO_DISK";
  
  #warning USING_PROGRAM_FLASH

#elif USING_QSPI_FLASH

  LittleFS_QSPIFlash myfs;
  
  char szDiskMem[] = "QSPI_DISK";
  
  #warning USING_QSPI_FLASH

#elif USING_SPI_NAND

  LittleFS_SPINAND myfs;
  
  char szDiskMem[] = "SPI_NAND";
  
  #warning USING_SPI_NAND

#elif USING_QPI_NAND

  LittleFS_QPINAND myfs;
  
  char szDiskMem[] = "QSPI_NAND";
  
  #warning USING_QPI_NAND

#endif

// Set for SPI usage
//const int FlashChipSelect = 10; // AUDIO BOARD
const int FlashChipSelect = 4; // PJRC Mem board 64MB on #5, #6 : NAND 1Gb on #3, 2GB on #4
//const int FlashChipSelect = 5; // PJRC Mem board 64MB on #5, #6 : NAND 1Gb on #3, 2GB on #4
//const int FlashChipSelect = 6; // digital pin for flash chip CS pin

void initFS()
{
  Serial.print("Initializing LittleFS ...");

  // see if the Flash is present and can be initialized:
  // Note:  SPI is default so if you are using SPI and not SPI for instance
  //        you can just specify myfs.begin(chipSelect).
#if USING_PSRAM
  if (!myfs.begin(buf, sizeof(buf)))
#elif USING_SPI_FLASH
  if (!myfs.begin(chipSelect, SPI))
#elif USING_SPI_FRAM
  if (!myfs.begin( FlashChipSelect ))
#elif USING_QSPI_FLASH
  if (!myfs.begin())
#elif USING_SPI_NAND
  if (!myfs.begin( FlashChipSelect ))
#elif USING_PROGRAM_FLASH
  if (!myfs.begin(1024 * 1024 * 6))
#else
  if (!myfs.begin())
#endif
  {
    Serial.printf("Error starting FS");

    while (1)
    {
      // Error, so don't do anything more - stay stuck here
    }
  }

  Serial.println("LittleFS initialized.");
}

void setup()
{
  Serial.begin(115200);
  while (!Serial && millis() < 5000);

  delay(200);

  Serial.print("\nStart AsyncFSBrowser on "); Serial.print(BOARD_NAME);
  Serial.print(" with "); Serial.println(SHIELD_TYPE);
  Serial.println(ASYNC_FSWEBSERVER_TEENSY41_VERSION);

#if USING_DHCP
  // Start the Ethernet connection, using DHCP
  Serial.print("Initialize Ethernet using DHCP => ");
  Ethernet.begin();
#else
  // Start the Ethernet connection, using static IP
  Serial.print("Initialize Ethernet using static IP => ");
  Ethernet.begin(myIP, myNetmask, myGW);
  Ethernet.setDNSServerIP(mydnsServer);
#endif

  if (!Ethernet.waitForLocalIP(5000))
  {
    Serial.println("Failed to configure Ethernet");

    if (!Ethernet.linkStatus())
    {
      Serial.println("Ethernet cable is not connected.");
    }

    // Stay here forever
    while (true)
    {
      delay(1);
    }
  }

  if (!Ethernet.waitForLink(5000))
  {
    Serial.println(F("Failed to wait for Link"));
  }
  else
  {
    Serial.print("IP Address = ");
    Serial.println(Ethernet.localIP());
  }

  initFS();

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  events.onConnect([](AsyncEventSourceClient * client)
  {
    client->send("hello!", NULL, millis(), 1000);
  });

  server.addHandler(&events);

  server.addHandler(new AsyncFSEditor(&myfs, http_username, http_password));

  server.serveStatic("/", &myfs, "/").setDefaultFile("index.htm");

  server.onNotFound([](AsyncWebServerRequest * request) 
  {
    Serial.printf("NOT_FOUND: ");
    if (request->method() == HTTP_GET)
      Serial.printf("GET");
    else if (request->method() == HTTP_POST)
      Serial.printf("POST");
    else if (request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if (request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if (request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if (request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if (request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
      
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if (request->contentLength()) 
    {
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    
    for (i = 0; i < headers; i++) 
    {
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();

    for (i = 0; i < params; i++)
    {
      AsyncWebParameter* p = request->getParam(i);

      if (p->isFile())
      {
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      }
      else if (p->isPost())
      {
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
      else
      {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }

    request->send(404);
  });

  server.onFileUpload([](AsyncWebServerRequest * request, const String & filename, size_t index, uint8_t *data, size_t len, bool final)
  {
    if (!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());

    Serial.printf("%s", (const char*)data);

    if (final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index + len);
  });

  server.onRequestBody([](AsyncWebServerRequest * request, uint8_t *data, size_t len, size_t index, size_t total)
  {
    if (!index)
      Serial.printf("BodyStart: %u\n", total);

    Serial.printf("%s", (const char*)data);

    if (index + len == total)
      Serial.printf("BodyEnd: %u\n", total);
  });

  server.begin();
}

void loop()
{
  ws.cleanupClients();
}
