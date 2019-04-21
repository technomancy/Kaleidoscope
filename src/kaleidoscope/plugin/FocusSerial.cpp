/* -*- mode: c++ -*-
 * Kaleidoscope-FocusSerial -- Bidirectional communication plugin
 * Copyright (C) 2017, 2018  Keyboard.io, Inc
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <Kaleidoscope-FocusSerial.h>

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

namespace kaleidoscope {
namespace plugin {

char FocusSerial::command_[32];

void FocusSerial::drain(void) {
  if (FocusIO.available())
    while (FocusIO.peek() != '\n')
      FocusIO.read();
}

EventHandlerResult FocusSerial::beforeReportingState() {
  if (FocusIO.available() == 0)
    return EventHandlerResult::OK;

  uint8_t i = 0;
  do {
    command_[i++] = FocusIO.read();

    if (FocusIO.peek() == '\n')
      break;
  } while (command_[i - 1] != ' ' && i < 32);
  if (command_[i - 1] == ' ')
    command_[i - 1] = '\0';
  else
    command_[i] = '\0';

  Kaleidoscope.onFocusEvent(command_);

  FocusIO.println(F("\r\n."));

  drain();

  if (FocusIO.peek() == '\n')
    FocusIO.read();

  return EventHandlerResult::OK;
}

bool FocusSerial::handleHelp(const char *command,
                             const char *help_message) {
  if (strcmp_P(command, PSTR("help")) != 0)
    return false;

  FocusIO.println((const __FlashStringHelper *)help_message);
  return true;
}

EventHandlerResult FocusSerial::onFocusEvent(const char *command) {
  handleHelp(command, PSTR("help"));
  return EventHandlerResult::OK;
}

void FocusSerial::printBool(bool b) {
  FocusIO.print((b) ? F("true") : F("false"));
}

}
}

kaleidoscope::plugin::FocusSerial Focus;
