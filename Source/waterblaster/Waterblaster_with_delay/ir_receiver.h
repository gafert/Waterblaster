
#ifndef EMBEDDED_SYSTEMS_IR_H
#define EMBEDDED_SYSTEMS_IR_H

//Initialize IR Receiver
void init_IRReceiver();

bool isIrRightActive;
bool isIrLeftActive;
bool isIrUpActive;
bool isIrDownActive;
bool isIrShootActive;

#define REMOTEADDRESS 0b11100000

#define CODERIGHT_samsung 0b01000110
#define CODELEFT_samsung 0b10100110
#define CODEUP_samsung 0b00000110
#define CODEDOWN_samsung 0b10000110
#define CODEOK_samsung 0b00010110

#endif
