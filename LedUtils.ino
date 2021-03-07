void changeLeds(int path1, int path2, int path3, int path4) {
  digitalWrite(LED_PATCH1, path1);
  digitalWrite(LED_PATCH2, path2);
  digitalWrite(LED_PATCH3, path3);
  digitalWrite(LED_PATCH4, path4);
}

void ChangeLedRGB(int r, int g, int b) {
  digitalWrite(LED_MODE_R, r);
  digitalWrite(LED_MODE_G, g);
  digitalWrite(LED_MODE_B, b);
}

void OffLeds() {
  changeLeds(LOW, LOW, LOW, LOW);
}


void OnLeds() {
  changeLeds(HIGH, HIGH, HIGH, HIGH);
}

void LedGreen() {
  ChangeLedRGB(0, 255, 0);
}

void LedBlue() {
  ChangeLedRGB(0, 0, 255);
}

void LedRed() {
  ChangeLedRGB(255, 0, 0);
}

void LedAmarelo() {
  ChangeLedRGB(255, 50, 0);
}

void LedRoxo() {
  ChangeLedRGB(255, 0, 207);
}

void LedBranco() {
  ChangeLedRGB(HIGH, HIGH, HIGH);
}

void LedApagado() {
  ChangeLedRGB(LOW, LOW, LOW);
}


void AnimateLed(bool on) {
  if (on) {
    OnLeds();
    LedAmarelo();
  } else {
    OffLeds();
    LedApagado();
  }
}

void ConfigureLedByMode(int mode) {
  switch (mode)
  {
    case ProgramChange:
      LedGreen();
      break;
    case Bank:
      LedBlue();
      break;
    case ConfigureBank:
      LedRed();
      break;
    case Tuner:
      LedBranco();
      break;
    case Scroll:
      LedRoxo();
      break;
    default:
      LedApagado();
      break;
  }
}

void Blink(int ledPath) {
  digitalWrite(ledPath, HIGH);
  delay(200);
  digitalWrite(ledPath, LOW);
}
