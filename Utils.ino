void WriteMessage(String message) {
  Serial.println(message);
}

void WriteChangeProgram(int oldProgram, int newProgram) {
  WriteMessage("Change program of " + (String)oldProgram + " to " + (String)newProgram);
}

String FormatPoints(int qtde) {
  switch (qtde)
  {
    case 1:
      return ".";
    case 2:
      return "..";
    case 3:
      return "...";
    default:
      return " ";
  }
}

void LogTooglePath(bool patchActive, int pathPosition) {
  String enabled = !patchActive ? "Enable" : "Disable";
  WriteMessage("Button Path " + (String)pathPosition + " " + enabled);
}
