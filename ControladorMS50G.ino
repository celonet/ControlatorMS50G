#include <stdio.h>
#include <MIDI.h>
#include <Usb.h>
#include <usbhub.h>
#include <usbh_midi.h>
#include <SoftPWM.h>
#include <EEPROM.h>
#include <Bounce2.h>

#define BUTTON_MODE A0
#define BUTTON_PATCH1 A1
#define BUTTON_PATCH2 A2
#define BUTTON_PATCH3 A3
#define BUTTON_PATCH4 A4

#define LED_MODE_R 4
#define LED_MODE_G 3
#define LED_MODE_B 2

#define LED_PATCH1 5
#define LED_PATCH2 6
#define LED_PATCH3 7
#define LED_PATCH4 8

#define INTERVAL 50

USB Usb;
USBHub Hub1(&Usb);
USBH_MIDI  Midi(&Usb);

//Arduino MIDI library v4.2 compatibility
#ifdef MIDI_CREATE_DEFAULT_INSTANCE
MIDI_CREATE_DEFAULT_INSTANCE();
#endif

Bounce debouncerMode = Bounce();
Bounce debouncerPath1 = Bounce();
Bounce debouncerPath2 = Bounce();
Bounce debouncerPath3 = Bounce();
Bounce debouncerPath4 = Bounce();

byte Mode = 0;
byte OldMode = 0;
byte program = 0;
unsigned long previousMillisMode = 0;
unsigned long previousMillisTunner = 0;
unsigned long previousMillisScroll = 0;
const long interval = 300;

const int ProgramChange = 0;
const int Bank = 1;
const int ConfigureBank = 2;
const int Tuner = 3;
const int Scroll = 4;

const int firstProgram = 0;
const int lastProgram = 49;

bool patch1Active = false;
bool patch2Active = false;
bool patch3Active = false;
bool patch4Active = false;

byte patch1;
byte patch2;
byte patch3;
byte patch4;
const byte PatchOff = 0x32;
const byte TunnerOn = 0x7f;
const byte TunnerOff = 0x00;

void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);
  pinMode(LED_MODE_R, OUTPUT);
  pinMode(LED_MODE_G, OUTPUT);
  pinMode(LED_MODE_B, OUTPUT);
  pinMode(LED_PATCH1, OUTPUT);
  pinMode(LED_PATCH2, OUTPUT);
  pinMode(LED_PATCH3, OUTPUT);
  pinMode(LED_PATCH4, OUTPUT);

  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  WriteMessage("Initializing controller");

  if (Usb.Init() == -1) {
    WriteMessage("USB Host Shield not found!");
  }
  else {
    Initialize();
  }

  WriteMessage("GetConfiguration");
  GetConfiguration();

  WriteMessage("Controller Initilized");

  debouncerMode.attach(BUTTON_MODE, INPUT_PULLUP);
  debouncerMode.interval(INTERVAL);

  debouncerPath1.attach(BUTTON_PATCH1, INPUT_PULLUP);
  debouncerPath1.interval(INTERVAL);

  debouncerPath2.attach(BUTTON_PATCH2, INPUT_PULLUP);
  debouncerPath2.interval(INTERVAL);

  debouncerPath3.attach(BUTTON_PATCH3, INPUT_PULLUP);
  debouncerPath3.interval(INTERVAL);

  debouncerPath4.attach(BUTTON_PATCH4, INPUT_PULLUP);
  debouncerPath4.interval(INTERVAL);

  ConfigureLedByMode(Mode);
}

void loop() {

  debouncerMode.update();
  debouncerPath1.update();
  debouncerPath2.update();
  debouncerPath3.update();
  debouncerPath4.update();

  bool isMode = debouncerMode.fell() || debouncerMode.rose();
  bool isPath1 = debouncerPath1.fell() || debouncerPath1.rose();
  bool isPath2 = debouncerPath2.fell() || debouncerPath2.rose();
  bool isPath3 = debouncerPath3.fell() || debouncerPath3.rose();
  bool isPath4 = debouncerPath4.fell() || debouncerPath4.rose();

    uint8_t msg[4];
  if (MIDI.read()) {
    //msg[0] = MIDI.getType();
    WriteMessage("Read MSG: ");
  }

  if ( isMode) {
    bool doubleClick = millis() - previousMillisMode <= interval;
    ChangeMode(doubleClick);
    previousMillisMode = millis();
  } else if ( isPath1 ) {
    bool doubleClick = millis() - previousMillisScroll <= interval;
    if (doubleClick) {
      ChangeScroll();
    } else  {
      TooglePatch1();
    }
    previousMillisScroll = millis();
  } else if ( isPath2 )
    TooglePatch2();
  else if ( isPath3 )
    TooglePatch3();
  else if ( isPath4 ) {
    bool doubleClick = millis() - previousMillisTunner <= interval;
    if (doubleClick) {
      ChangeTunner();
    } else  {
      TooglePatch4();
    }
    previousMillisTunner = millis();
  }
}

void ChangeScroll() {
  WriteMessage("Scroll Mode Enable");
  SendMidiRequestPatch();
  OldMode = Mode;
  Mode = Scroll;
  ConfigureLedByMode(Mode);
}

void ChangeTunner() {
  WriteMessage("Tunner Mode Enable");
  SendMidiTunner(TunnerOn);
  OldMode = Mode;
  Mode = Tuner;
  ConfigureLedByMode(Mode);
}

void ChangeMode(bool doubleClick) {
  WriteMessage("Button Mode click.");

  BackOldMode();

  OffLeds();

  byte oldMode = Mode;
  if (doubleClick) {
    Mode = ConfigureBank;
  }
  else {
    byte newMode = nextMode(oldMode);
    Mode = newMode;
    UpdateMemory(0, newMode);
  }

  WriteMessage("Mode change of " + ParseConfiguration(oldMode) + " to " + ParseConfiguration(Mode));

  ConfigureLedByMode(Mode);
}

void BackOldMode() {
  if (Mode == Tuner) {
    SendMidiTunner(TunnerOff);
    Mode = OldMode;
    ConfigureLedByMode(Mode);
  } else if (Mode == Scroll) {
    Mode = OldMode;
    ConfigureLedByMode(Mode);
  }
}

void ChangePath(bool &active, byte &patchToSet, int pathPosition) {

  if (Mode == Bank) {
    int pathState = active;

    InactiveButtons();

    program = pathState ? PatchOff : patchToSet;
    active = !pathState;

    Execute(program);
    WriteMessage("Active: " + (String)active + ", Program " + (String)program);
  } else if (Mode == ConfigureBank) {
    WriteMessage("Save position " + (String)pathPosition + ", Program " + (String)program);
    UpdateMemory(pathPosition, program);
    patchToSet = program;
  }
}

void Up() {
  int oldProgram = program;
  int newProgram = firstProgram;

  if (program + 1 > lastProgram)
  {
    newProgram = firstProgram;
    program = firstProgram;
  }
  else
    newProgram = ++program;

  Execute(newProgram);

  WriteChangeProgram(oldProgram, newProgram);
}

void Down() {
  int oldProgram = program;
  int newProgram = firstProgram;

  if (program == firstProgram) {
    newProgram = lastProgram;
    program = lastProgram;
  }
  else
    newProgram = --program;

  Execute(newProgram);

  WriteChangeProgram(oldProgram, newProgram);
}

void TooglePatch1() {
  LogTooglePath(patch1Active, 1);

  BackOldMode();

  if (Mode == ProgramChange) {
    Blink(LED_PATCH1);
    Up();
  } else {
    ChangePath(patch1Active, patch1, 1);
    if (Mode == ConfigureBank)
      Blink(LED_PATCH1);
    else
      changeLeds(patch1Active ? HIGH : LOW, LOW, LOW, LOW);
  }
}

void TooglePatch2() {
  LogTooglePath(patch2Active, 2);

  BackOldMode();

  if (Mode == ProgramChange) {
    Blink(LED_PATCH2);
    Down();
  } else if (Mode == Scroll) {
    WriteMessage("Scrool Right");
  } else {
    ChangePath(patch2Active, patch2, 2);
    if (Mode == ConfigureBank)
      Blink(LED_PATCH2);
    else
      changeLeds(LOW, patch2Active ? HIGH : LOW, LOW, LOW);
  }
}

void TooglePatch3() {
  LogTooglePath(patch3Active, 3);

  BackOldMode();

  if (Mode == ProgramChange)
    return;

  ChangePath(patch3Active, patch3, 3);
  if (Mode == ConfigureBank)
    Blink(LED_PATCH3);
  else if (Mode == Scroll) {
    WriteMessage("Scrool Left");
  }
  else
    changeLeds(LOW, LOW, patch3Active ? HIGH : LOW, LOW);
}

void TooglePatch4() {
  LogTooglePath(patch4Active, 4);

  BackOldMode();

  if (Mode == ProgramChange)
    return;

  ChangePath(patch4Active, patch4, 4);

  if (Mode == ConfigureBank)
    Blink(LED_PATCH4);
  else
    changeLeds(LOW, LOW, LOW, patch4Active ? HIGH : LOW);
}
