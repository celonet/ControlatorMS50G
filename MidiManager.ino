void Initialize() {

  int qdtErr = 0;
  int ret = 0;
  while (SendMidi(0) == false)
  {
    WriteMessage("Initializing" + FormatPoints(ret));
    AnimateLed(ret % 2);
    ret = ret == 4 ? 0 : ++ret;

    if (qdtErr == 10) {
      break;
      WriteMessage("Initialize Error!!!");
    }

    qdtErr++;

    delay(500);
  }
  AnimateLed(false);
}

void Execute(byte number) {
  if (number > 49 || number < 0)
    number = 0;

  WriteMessage("Change to program: " + (String)number);
  SendMidi(number);
}

bool SendMidi(int number) {
  bool sended = false;
  Usb.Task();
  if ( Usb.getUsbTaskState() == USB_STATE_RUNNING )
  {
    byte message[2];
    message[0] = 0xC0;               // 0xC0 is for Program Change
    message[1] = number;             // Number is the program/patch
    Midi.SendData(message);
    delay(10);
    sended = true;
  }
  return sended;
}

bool SendMidiTunner(int cmd) {
  bool sended = false;
  Usb.Task();
  if ( Usb.getUsbTaskState() == USB_STATE_RUNNING )
  {
    byte message[3];
    message[0] = 0xb0;              // 0xb0 is for MIDI Control Change
    message[1] = 0x4a;              // Tuner
    message[2] = cmd;             // On/Off
    Midi.SendData(message);
    delay(10);
    sended = true;
  }
  return sended;
}

void SendMidiRequestPatch(){
  bool sended = false;
  Usb.Task();
  if ( Usb.getUsbTaskState() == USB_STATE_RUNNING )
  {
    byte message[5];
    message[0] = 0xf0;
    message[1] = 0x52;
    message[2] = 0x00;
    message[3] = 0x58;
    message[4] = 0x29;
    message[5] = 0xf7;
    Midi.SendData(message);
    delay(10);
    sended = true;
  }
  return sended;  
}
