void UpdateMemory(int pos, byte value)
{
  EEPROM.update(pos, value);
}

void UpdateMemory(int pos, int value)
{
  EEPROM.update(pos, value);
}

int GetData(int pos, byte patch)
{
  return EEPROM.get(pos, patch);
}
