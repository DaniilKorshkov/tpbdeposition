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


#define MAX_USBTMC_DEVICES 8

const uint8_t RESPONSE_BUFFER_SIZE = 64;
const uint16_t REQUEST_DELAY_MS = 1000;
const uint8_t REQUEST_MAX_RETRIES = 4;

const int ANINE = A9;
const float RESISTANCE = 1.0;
const float MIN_WATTAGE = 20.0;
const float MAX_WATTAGE = 30.0;

const char USB488Terminator = '\n';
const char SerialTerminator = '\n';

static bool isTransmitOnBin = false;

// Readback state per device
enum RbState { RB_IDLE, RB_QUERY_SENT, RB_WAIT_RESPONSE, RB_DONE };
static RbState rbState[MAX_USBTMC_DEVICES] = { };
static String responseBuf[MAX_USBTMC_DEVICES];
static uint32_t querySentMillis[MAX_USBTMC_DEVICES] = { };
static uint8_t requestAttempts[MAX_USBTMC_DEVICES] = { };
static String ret_string = "";

class USBTMCAsync : public USBTMCAsyncOper
{
    uint8_t devIndex;
public:
    USBTMCAsync(uint8_t index) : devIndex(index) {}
    void OnRcvdDescr(USB_DEVICE_DESCRIPTOR *pdescr, uint8_t *serialNumPtr, uint8_t serialNumLen);
    void OnReceived(uint8_t data);
    void OnReadStatusByte(uint8_t status);
    void OnFailed(USBTMCInformation info, uint8_t code);
};

void USBTMCAsync::OnRcvdDescr(USB_DEVICE_DESCRIPTOR *pdescr, uint8_t *serialNumPtr, uint8_t serialNumLen)
{
    Serial.print(F("Dev["));
    Serial.print(devIndex);
    Serial.print(F("] ProductID:"));
    Serial.println(pdescr->idProduct, HEX);

    Serial.print(F("Dev["));
    Serial.print(devIndex);
    Serial.print(F("] VendorID:"));
    Serial.println(pdescr->idVendor, HEX);

    Serial.print(F("Dev["));
    Serial.print(devIndex);
    Serial.print(F("] SerialNumber:"));
    for (int i = 2; i < serialNumLen; i += 2)
    { // string is UTF-16LE encoded
        Serial.print((char)serialNumPtr[i]);
    }

    Serial.println("");
}

void USBTMCAsync::OnReceived(uint8_t data)
{
    if (rbState[devIndex] == RB_WAIT_RESPONSE)
    {
        if (responseBuf[devIndex].length() < RESPONSE_BUFFER_SIZE)
        {
            responseBuf[devIndex] += (char)data;
        }

        if (data == USB488Terminator)
        {
            rbState[devIndex] = RB_DONE;
        }
    }
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
    if (info == USBTMCInformation::RequestError)
    {
        // Device NAK'd the REQUEST_DEV_DEP_MSG_IN header because it is still
        // busy producing the CURR? response. Request() is retried from loop()
        // until the device accepts the header; a final error is printed there.
        return;
    }
    else if (info == USBTMCInformation::ReceiveheaderNakAndTimeouted)
    {
        Serial.println(F("Receive timeout occured"));
    }
    else if (info == USBTMCInformation::ReceivepayloadNakAndTimeouted)
    {
        Serial.println(F("Receive timeout occured"));
    }
    else if (info == USBTMCInformation::AbortbulkinSucceed)
    {
        // Expected after AbortReceive() terminates a completed CURR? readback.
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


// USB string descriptor in raw format: [length, 0x03, UTF-16LE chars].
// To discover actual serials: upload without SetTargetSerialNumber, observe
// the "Dev[X] SerialNumber:" lines in serial output, then fill in below.
//
// Example for serial "CN65290125" (10 chars -> 0x16 = 2 + 2*10):
//   static const uint8_t SERIAL_PSU[] PROGMEM = {
//       0x16, 0x03,
//       'C',0, 'N',0, '6',0, '5',0, '2',0,
//       '9',0, '0',0, '1',0, '2',0, '5',0
//   };
static const uint8_t SERIAL_PSU1[] PROGMEM = {
    0x16, 0x03,
    'C',0, 'N',0, '6',0, '5',0, '2',0,
    '8',0, '0',0, '1',0, '2',0, '9',0
};
static const uint8_t SERIAL_PSU2[] PROGMEM = {
    0x16, 0x03,
    'C',0, 'N',0, '6',0, '5',0, '2',0,
    '9',0, '0',0, '1',0, '2',0, '5',0
};

USB Usb;
USBHub Hub1(&Usb);

USBTMCAsync UsbtmcAsync[MAX_USBTMC_DEVICES] = {
    USBTMCAsync(0), USBTMCAsync(1), USBTMCAsync(2), USBTMCAsync(3),
    USBTMCAsync(4), USBTMCAsync(5), USBTMCAsync(6), USBTMCAsync(7),
};
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

    // Pin each USBTMC instance to a specific physical supply by serial number.
    // Remove or comment out SetTargetSerialNumber() for devices not present.
    Usbtmc[0].SetTargetSerialNumber(SERIAL_PSU1);
    Usbtmc[1].SetTargetSerialNumber(SERIAL_PSU2);

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
   
        String param = "";
   
       param += "CURR ";
       param += String(amperage);
       param += ", (@1,2,3,4)";
       
   
   param += (char)USB488Terminator;

   //Serial.println("Raw signal: "); Serial.print(raw_signal,DEC); Serial.println(" Amperage: "); Serial.print(amperage,DEC); Serial.println("\n");
        
    //for (int i = 0; i < MAX_USBTMC_DEVICES; i++)
    //{
    //    if (Usbtmc[i].IsConnected() && Usbtmc[i].IsIdle())
    //    {

            
    //        Usbtmc[i].Transmit(param.length(), (uint8_t *)param.c_str());

            
    //    }
    //}


    delay(100);


    String query = "MEAS:CURR?";
    query += " (@1,2,3,4)";
    query += (char)USB488Terminator;


    Serial.print(F("!START!"));
    for (int i = 0; i < MAX_USBTMC_DEVICES; i++)
    {
        if (!Usbtmc[i].IsConnected())
        {
            continue;
        }

        if (rbState[i] == RB_IDLE)
        {
            if (Usbtmc[i].IsIdle())
            {
                rbState[i] = RB_QUERY_SENT;
                requestAttempts[i] = 0;
                querySentMillis[i] = millis();
                Usbtmc[i].Transmit(param.length(), (uint8_t *)param.c_str());
                delay(50);
                Usbtmc[i].Transmit(query.length(), (uint8_t *)query.c_str());
            }
        }
        else if (rbState[i] == RB_QUERY_SENT)
        {
            if ((millis() - querySentMillis[i]) >= REQUEST_DELAY_MS)
            {
                Usbtmc[i].Request(RESPONSE_BUFFER_SIZE);
                if (Usbtmc[i].IsIdle())
                {
                    // Device NAK'd the REQUEST_DEV_DEP_MSG_IN header: it is still
                    // busy producing the CURR? response (the shield's USB transfer
                    // timeout is 5 s). Give it more time and retry.
                    requestAttempts[i]++;
                    if (requestAttempts[i] >= REQUEST_MAX_RETRIES)
                    {
                        printReadbackError(i);
                        Usbtmc[i].Clear();
                        rbState[i] = RB_IDLE;
                    }
                    else
                    {
                        querySentMillis[i] = millis();
                    }
                }
                else
                {
                    rbState[i] = RB_WAIT_RESPONSE;
                }
            }
        }
        else if (rbState[i] == RB_DONE)
        {
            if (!Usbtmc[i].IsIdle())
            {
                Usbtmc[i].AbortReceive();
            }
            printReadback(i);
            rbState[i] = RB_IDLE;
        }
        else if (rbState[i] == RB_WAIT_RESPONSE)
        {
            if (Usbtmc[i].IsIdle())
            {
                if (responseBuf[i].length() >= RESPONSE_BUFFER_SIZE)
                {
                    Usbtmc[i].AbortReceive();
                }
                printReadback(i);
                rbState[i] = RB_IDLE;
            }
        }
    }
    Serial.print(F("!END!"));
    delay(100);


    


}

void printReadback(uint8_t devIndex)
{
    responseBuf[devIndex].trim();
    Serial.print(F("Dev["));
    Serial.print(devIndex);
    Serial.print(F("] CURR: "));
    Serial.println(responseBuf[devIndex]);
    responseBuf[devIndex] = "";
}

void printReadbackError(uint8_t devIndex)
{
    Serial.print(F("Dev["));
    Serial.print(devIndex);
    Serial.println(F("] CURR? request timed out"));
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
