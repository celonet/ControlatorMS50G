void GetConfiguration()
{
  int mode = GetData(0, Mode);
  if (mode == 255) {
    UpdateMemory(0, 0);
  } else {
    Mode = mode;
  }

  SetPath(1, patch1);
  SetPath(2, patch2);
  SetPath(3, patch3);
  SetPath(4, patch4);
  
  ShowConfiguration();
}

void SetPath(int position, byte &path) {
  int pathConfig = GetData(position, path);
  if (pathConfig != 255)
    path = pathConfig;
}

void ShowConfiguration()
{
  WriteMessage("Patch Mode: " + ParseConfiguration(Mode));
  WriteMessage("Patch 1: " + (String)patch1);
  WriteMessage("Patch 2: " + (String)patch2);
  WriteMessage("Patch 3: " + (String)patch3);
  WriteMessage("Patch 4: " + (String)patch4);
}

/*
  0 - Program Change
  1 - Bank
  2 - Configure
  3 - Tuner
  4 - Scroll
*/
String ParseConfiguration(int mode) {
  switch (mode)
  {
    case ProgramChange:
      return "Program Change";
    case Bank:
      return "Bank";
    case ConfigureBank:
      return "Configure Bank";
    case Tuner:
      return "Tuner";
    case Scroll:
      return "Scroll";
    default:
      return "Configuration Not Found";
  }
}

int nextMode(int mode) {
  return mode == ProgramChange ? Bank : ProgramChange;
}
