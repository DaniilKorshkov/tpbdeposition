/*
 * Example sketch for the USBTMC Driver - developed by Naoya Imai
 */
#include <usbhub.h>

#include "usbtmc.h"

// Satisfy the IDE, which needs to see the include statement in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif

#include <SPI.h>

const char USB488Terminator = '\n';
const char SerialTerminator = '\n';

static bool isTransmitOnBin = false;

class USBTMCAsync : public USBTMCAsyncOper
{
public:
    void OnRcvdDescr(USB_DEVICE_DESCRIPTOR *pdescr, uint8_t *serialNumPtr, uint8_t serialNumLen);
    void OnReceived(uint8_t data);
    void OnReadStatusByte(uint8_t status);
    void OnFailed(USBTMCInformation info, uint8_t code);
};

void USBTMCAsync::OnRcvdDescr(USB_DEVICE_DESCRIPTOR *pdescr, uint8_t *serialNumPtr, uint8_t serialNumLen)
{
    Serial.print(F("ProductID:"));
    Serial.println(pdescr->idProduct, HEX);

    Serial.print(F("VendorID:"));
    Serial.println(pdescr->idVendor, HEX);

    Serial.print(F("SerialNumber:"));
    for (int i = 2; i < serialNumLen; i += 2)
    { // string is UTF-16LE encoded
        Serial.print((char)serialNumPtr[i]);
    }

    Serial.println("");
}

void USBTMCAsync::OnReceived(uint8_t data)
{
    Serial.write(data);
}

void USBTMCAsync::OnReadStatusByte(uint8_t status)
{
    char high;
    char low;
    uint8_t tmp;
    tmp = (status >> 4) & 0x0F;
    if (tmp < 0x0A)
        high = tmp + 0x30;
    else
        high = (tmp - 0x0A) + 0x41;

    tmp = status & 0x0F;
    if (tmp < 0x0A)
        low = tmp + 0x30;
    else
        low = (tmp - 0x0A) + 0x41;

    String text = "0x";
    text += high;
    text += low;

    Serial.println(text);
}

void USBTMCAsync::OnFailed(USBTMCInformation info, uint8_t code)
{
    if (info == USBTMCInformation::ReceiveheaderNakAndTimeouted)
    {
        Serial.println(F("Receive timeout occured"));
    }
    else if (info == USBTMCInformation::ReceivepayloadNakAndTimeouted)
    {
        Serial.println(F("Receive timeout occured"));
    }
    else if (info == USBTMCInformation::AbortbulkinSucceed)
    {
        Serial.println(F("Abort Bulkin Succeed"));
    }
    else if (info == USBTMCInformation::ClaerSucceed)
    {
        Serial.println(F("Clear Succeed"));
    }
    else
    {
        Serial.print(F("USBTMCInformation = "));
        Serial.print(static_cast<int16_t>(info));

        if (info == USBTMCInformation::InitiateabortbulkoutFailed ||
            info == USBTMCInformation::InitiateabortbulkinFailed)
        {
            Serial.print(F(" USBTMC_status = "));
        }
        else
        {
            Serial.print(F(" rcode = "));
        }

        Serial.print(code, HEX);
        Serial.println(F("h"));
    }
}

USB Usb;
// USBHub Hub1(&Usb);
USBTMCAsync UsbtmcAsync;
USBTMC Usbtmc(&Usb, &UsbtmcAsync);












const int ANINE = A9;
const float RESISTANCE = 1.0;
const float MIN_WATTAGE = 20.0;
const float MAX_WATTAGE = 30.0;

void setup()
{
    

    Usbtmc.TimeStep(0); // Try to change timestep when you can not receive all of the data.
                        // Some test and measurement instruments can not respond quickly.



    pinMode(ANINE,INPUT);


    Usb.Task();
    Usbtmc.Run();
      
}



void loop()
{
    Usb.Task();
    Usbtmc.Run();


    int raw_signal = analogRead(ANINE);
    float amperage = sqrt((( MAX_WATTAGE*raw_signal + MIN_WATTAGE*(1023-raw_signal)   )/(1023*RESISTANCE)));
   
   String param = "CURR ";
   param += String(amperage);
   param += (char)USB488Terminator;
        
    Usbtmc.Transmit(param.length(), (uint8_t *)param.c_str());

    delay(50);

    
           
}

