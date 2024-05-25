void (*gLog)(String);

#include "./ukvm_arduino_utils/ukvm_utils.h"
UTILS utils;

#include "./ukvm_arduino_serial/ukvm_serial.h"
UKVM_Serial* ukvm_serial;


int outs[8] = {5, 6, 7, 8, 9, 10, 11, 12};
int currentOut = 5;

int currentIdx = -1;

int getIdxPin(int idx){ //index starts at 1
  return outs[idx - 1];
}

void setup() {
  ukvm_serial = new UKVM_Serial();
  ukvm_serial->init();
  ukvm_serial->onCommand = onSerialCommand;
  gLog = ukvm_serial->log;

  for (int i = 1; i < 9; i++){
    pinMode(getIdxPin(i), OUTPUT);
    //digitalWrite(getIdxPin(currentIdx), HIGH);
  }
}

void loop() {
  ukvm_serial->checkSerial();
}

void setOutput(int idx){
  if (idx == currentIdx) return;
  if (idx < 1 || idx > 8) return;

  deselectAll();


  delay(100);
  currentIdx = idx;
  //gLog("Activating output " + String(idx) + " at pin " + getIdxPin(currentIdx));
  digitalWrite(getIdxPin(currentIdx), HIGH);
}

void deselectAll(){
  for (int i = 1; i < 9; i++){
    digitalWrite(getIdxPin(currentIdx), LOW);
  }
}

void onAnyCommand(String cmd, String val){
  String arg1 = utils.parseStr(val, 0, ",");
  String arg2 = utils.parseStr(val, 1, ",");
  
  if (cmd == "get_info"){
    gLog("{\"id\": \"usb_switch\"}");
  }

  if (cmd == "select"){
    int targetIdx = arg1.toInt();
    if (targetIdx > 0 && targetIdx < 9){
      setOutput(targetIdx);
    } else {
      gLog("Invalid target index");
    }
  }

  if (cmd == "get_selected"){
      gLog("{selected: " + String(currentIdx) + "}");
  }

  if (cmd == "deselect_all"){
    deselectAll();
  }
}


void onSerialCommand(String cmd, String val){
  gLog("[From serial] cmd: " + cmd + "    value: " + val);
  onAnyCommand(cmd, val);
}