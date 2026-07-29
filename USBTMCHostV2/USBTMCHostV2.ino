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

const int ANINE = A9;
const float RESISTANCE = 1.0;
const float MIN_WATTAGE = 20.0;
const float MAX_WATTAGE = 30.0;

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

#define MAX_USBTMC_DEVICES 8

USB Usb;
USBHub Hub1(&Usb);

USBTMCAsync UsbtmcAsync[MAX_USBTMC_DEVICES] = { };
USBTMC Usbtmc[MAX_USBTMC_DEVICES] = {
    USBTMC(&Usb, &UsbtmcAsync[0]),
    USBTMC(&Usb, &UsbtmcAsync[1]),
    USBTMC(&Usb, &UsbtmcAsync[2]),
    USBTMC(&Usb, &UsbtmcAsync[3]),
    USBTMC(&Usb, &UsbtmcAsync[4]),
    USBTMC(&Usb, &UsbtmcAsync[5]),
    USBTMC(&Usb, &UsbtmcAsync[6]),
    USBTMC(&Usb, &UsbtmcAsync[7]),
};

void setup()
{
    Serial.begin(115200);
#if !defined(__MIPSEL__)
    while (!Serial)
        ; // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
    Serial.println(F("USBTMC Host Start"));

    if (Usb.Init() == -1)
        Serial.println(F("OSC did not start."));

    delay(200);

    pinMode(ANINE,INPUT);


    for (int i = 0; i < MAX_USBTMC_DEVICES; i++)
    {
        Usbtmc[i].TimeStep(0); // Try to change timestep when you can not receive all of the data.
                               // Some test and measurement instruments can not respond quickly.
    }
}

void loop()
{
    Usb.Task();

    for (int i = 0; i < MAX_USBTMC_DEVICES; i++)
    {
        Usbtmc[i].Run();
    }

    if (Usb.getUsbTaskState() != USB_STATE_RUNNING)
    {
        return;
    }

    if (isTransmitOnBin)
    {
        // #48196XXXX,,,
        while (Serial.available() > 0)
        {
            for (int i = 0; i < MAX_USBTMC_DEVICES; i++)
            {
                if (Usbtmc[i].IsConnected())
                {
                    Usbtmc[i].TransmitData(Serial.read());

                    if (Usbtmc[i].TransmitDone())
                    {
                        isTransmitOnBin = false;
                        break;
                    }
                }
            }
        }

        return;
    }

    int raw_signal = analogRead(ANINE);
    float amperage = sqrt((( MAX_WATTAGE*raw_signal + MIN_WATTAGE*(1023-raw_signal)   )/(1023*RESISTANCE)));
   
   String param = "CURR ";
   param += String(amperage);
   param += (char)USB488Terminator;

   //Serial.println("Raw signal: "); Serial.print(raw_signal,DEC); Serial.println(" Amperage: "); Serial.print(amperage,DEC); Serial.println("\n");
        
    for (int i = 0; i < MAX_USBTMC_DEVICES; i++)
    {
        if (Usbtmc[i].IsConnected() && Usbtmc[i].IsIdle())
        {
            Usbtmc[i].Transmit(param.length(), (uint8_t *)param.c_str());
        }
    }

    delay(100);
}

String serialReceive()
{
    static String tmpText = "";
    String receivedText = "";
    char rc;

    while (Serial.available() > 0)
    {
        rc = Serial.read();

        if ((rc == 0x00) || (!isAscii(rc)))
        {
            continue;
        }

        if (rc == SerialTerminator)
        {
            receivedText = tmpText;
            tmpText = "";
            break;
        }
        else
        {
            tmpText += rc;
        }
    }

    return receivedText;
}
