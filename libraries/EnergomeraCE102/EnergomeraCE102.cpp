#include "EnergomeraCE102.h"

EnergomeraCE102::EnergomeraCE102()
{

}

void EnergomeraCE102::BadCommand() {
    startCePacket();
    sendByteToRS485(0);
    endCEPacket();
}

void EnergomeraCE102::ReadTariffSum(uint16_t AddrD) {
    sendCommandToCE(AddrD, ReadTariffSum_Command);
}

void EnergomeraCE102::ReadSerialNumber(uint16_t AddrD) {
    sendCommandToCE(AddrD, ReadSerialNumber_Command);
}

void EnergomeraCE102::Ping(uint16_t AddrD) {
    sendCommandToCE(AddrD, Ping_Command);
}

void EnergomeraCE102::sendCommandToCE(int AddrD, int Command) {
    startCePacket();

    crc8 = 0;
    sendByteToCE(OPT_CH); crc8= crc8tab[crc8 ^ OPT_CH];


    unsigned char AddrDH = (unsigned char) (AddrD >> 8);
    unsigned char  AddrDL = (unsigned char ) (AddrD & 0xff);
    sendByteToCE(AddrDL); crc8= crc8tab[crc8 ^ AddrDL];
    sendByteToCE(AddrDH); crc8= crc8tab[crc8 ^ AddrDH];

    sendByteToCE(0); crc8= crc8tab[crc8 ^ 0];
    sendByteToCE(0); crc8= crc8tab[crc8 ^ 0];

    // PAL:
    unsigned char passwd[4] = {0x0,0x0,0x0,0x0}; // TODO: user pass
    for (i=0;i<4;i++) { // FIXME: magic 4 bytes
        sendByteToCE(passwd[i]); crc8= crc8tab[crc8 ^ passwd[i]];
    }

    // serv:
    unsigned char MessageLength = 0; // TODO:
    if (Command==ReadTariffSum_Command) {
        MessageLength=1;
    }

    unsigned char serv = DIRECT_REQ_CH + CLASS_ACCESS_CH + MessageLength;
    sendByteToCE(serv); crc8= crc8tab[crc8 ^ serv];

    unsigned char CommandH = (unsigned char) (Command >> 8);
    unsigned char CommandL =  (unsigned char ) (Command & 0xff);
    sendByteToCE(CommandH); crc8= crc8tab[crc8 ^ CommandH];
    sendByteToCE(CommandL); crc8= crc8tab[crc8 ^ CommandL];

    // if Ping -- No Additional Data in PAL
    if (Command==ReadTariffSum_Command) {
        sendByteToCE(0); crc8= crc8tab[crc8 ^ 0];
    }

    sendByteToCE(crc8);

    endCEPacket();
}

void EnergomeraCE102::sendByteToCE(uint16_t outByte)
{
    if (outByte==END_CH) {
        sendByteToRS485(END_REPL_1_CH);
        sendByteToRS485(END_REPL_2_CH);
    }
    else if (outByte==ESC_CH) {
        sendByteToRS485(ESC_REPL_1_CH);
        sendByteToRS485(ESC_REPL_1_CH);
    }
    else {
        sendByteToRS485(outByte);
    }
}


void EnergomeraCE102::endCEPacket()
{
    sendByteToRS485(END_CH);
}

void EnergomeraCE102::startCePacket()
{
    sendByteToRS485(END_CH);
}

void EnergomeraCE102::sendByteToRS485(uint16_t outByte)
{
    digitalWrite(SerialTxControl, RS485Transmit);
    Serial.write(outByte);
    Serial.flush();
    digitalWrite(SerialTxControl, RS485Receive);
    delay(1000);
}
