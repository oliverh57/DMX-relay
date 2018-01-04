#include <EEPROM.h>
#include <DMXSerial2.h>

// Constants for program

const int relay_0 = 9;  //Relay control pins
const int relay_1 = 6; 
const int relay_2 = 11;
const int relay_3 = 12;


const int rxStatusPin = 10;

const int cont = 5; //control pin for tranciver

#define DefaultLevel   HIGH //default when no DMX 

// define the RGB output color 
void rgb(byte r, byte g, byte b)
{
  /*if(r > 100){
    digitalWrite(RedPin,   HIGH);
  }else{
    digitalWrite(RedPin,   LOW);
  }
  
  analogWrite(RedPin,   r);
  analogWrite(GreenPin, g);
  analogWrite(BluePin,  b);*/
} // rgb()

// see DMXSerial2.h for the definition of the fields of this structure
const uint16_t my_pids[] = {E120_DEVICE_HOURS, E120_LAMP_HOURS};
struct RDMINIT rdmInit = {
  "Oliver Hynds", // Manufacturer Label
  1, // Device Model ID
  "DMX relay rec Arduino", // Device Model Label
  4, // footprint
  (sizeof(my_pids)/sizeof(uint16_t)), my_pids
};


void setup () {


  // initialize the Serial interface to be used as an RDM Device Node.
  // There are several constants that have to be passed to the library so it can reposonse to the
  // corresponding commands for itself.
  DMXSerial2.init(&rdmInit, processCommand);

  uint16_t start = DMXSerial2.getStartAddress();


  // set default values to dark red
  // this color will be shown when no signal is present for the first 5 seconds.
  DMXSerial2.write(start + 0, 30);
  DMXSerial2.write(start + 1,  0);
  DMXSerial2.write(start + 2,  0);
  
  // enable pwm outputs
  pinMode(relay_0,   OUTPUT); // sets the digital pin as output
  pinMode(relay_1, OUTPUT);
  pinMode(relay_2,  OUTPUT);
  pinMode(relay_3,  OUTPUT);
  
  pinMode(rxStatusPin,  OUTPUT);
 
  
  pinMode(cont,  OUTPUT);
  digitalWrite(cont, LOW);
  

} // setup()


void loop() {
  // Calculate how long no data backet was received
  unsigned long lastPacket = DMXSerial2.noDataSince();

  if (DMXSerial2.isIdentifyMode()) {
    // RDM Command for Indentification was sent.
    // Blink the device.
    unsigned long now = millis();
    if (now % 1000 < 500) {
      rgb(200, 200, 200);
    } else {
      rgb(0, 0, 0);
    } // if
    
  } else if (lastPacket < 30000) {
    
    if(DMXSerial2.readRelative(0) >= 127){
      digitalWrite(relay_0, HIGH);
    }else{
      digitalWrite(relay_0, LOW);
    }
    
    if(DMXSerial2.readRelative(1) >= 127){
      digitalWrite(relay_1, HIGH);
    }else{
      digitalWrite(relay_1, LOW);
    }
    
    if(DMXSerial2.readRelative(2) >= 127){
      digitalWrite(relay_2, HIGH);
    }else{
      digitalWrite(relay_2, LOW);
    }
    
    if(DMXSerial2.readRelative(3) >= 127){
      digitalWrite(relay_3, HIGH);
    }else{
      digitalWrite(relay_3, LOW);
    }
    
    
    


  } else {
    // Show default color, when no data was received since 30 seconds or more.
    digitalWrite(relay_0, DefaultLevel);
    digitalWrite(relay_1, DefaultLevel);
    digitalWrite(relay_2, DefaultLevel);
    digitalWrite(relay_3, DefaultLevel);
  } // if
  
  // check for unhandled RDM commands
  DMXSerial2.tick();
} // loop()


// This function was registered to the DMXSerial2 library in the initRDM call.
// Here device specific RDM Commands are implemented.
bool8 processCommand(struct RDMDATA *rdm, uint16_t *nackReason)
{
  byte CmdClass       = rdm->CmdClass;     // command class
  uint16_t Parameter  = rdm->Parameter;	   // parameter ID
  bool8 handled = false;

// This is a sample of how to return some device specific data
  if (Parameter == SWAPINT(E120_DEVICE_HOURS)) { // 0x0400
    if (CmdClass == E120_GET_COMMAND) {
      if (rdm->DataLength > 0) {
        // Unexpected data
        *nackReason = E120_NR_FORMAT_ERROR;
      } else if (rdm->SubDev != 0) {
        // No sub-devices supported
        *nackReason = E120_NR_SUB_DEVICE_OUT_OF_RANGE;
      } else {
        rdm->DataLength = 4;
        rdm->Data[0] = 0;
        rdm->Data[1] = 0;
        rdm->Data[2] = 2;
        rdm->Data[3] = 0;
        handled = true;
      }
    } else if (CmdClass == E120_SET_COMMAND) {
      // This device doesn't support set
      *nackReason = E120_NR_UNSUPPORTED_COMMAND_CLASS;
    }

  } else if (Parameter == SWAPINT(E120_LAMP_HOURS)) { // 0x0401
    if (CmdClass == E120_GET_COMMAND) {
      if (rdm->DataLength > 0) {
        // Unexpected data
        *nackReason = E120_NR_FORMAT_ERROR;
      } else if (rdm->SubDev != 0) {
        // No sub-devices supported
        *nackReason = E120_NR_SUB_DEVICE_OUT_OF_RANGE;
      } else {
        rdm->DataLength = 4;
        rdm->Data[0] = 0;
        rdm->Data[1] = 0;
        rdm->Data[2] = 0;
        rdm->Data[3] = 1;
        handled = true;
      }
    } else if (CmdClass == E120_SET_COMMAND) {
      // This device doesn't support set
      *nackReason = E120_NR_UNSUPPORTED_COMMAND_CLASS;
    }
  } // if
  
  return handled;
} // processCommand


// End.
