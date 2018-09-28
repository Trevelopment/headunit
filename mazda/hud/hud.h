#ifndef MZD_HUD_H
#define MZD_HUD_H

#include <stdint.h>
#include <string>

struct NaviData {
  std::string event_name;
  int32_t turn_side;
  int32_t turn_event;
  int32_t turn_number;
  int32_t turn_angle;
  int32_t distance;
  int32_t time_until;
  uint8_t previous_msg;
};


void hud_start();
void hud_stop();
bool hud_installed();
void hud_send(uint32_t diricon, uint16_t distance, std::string street, u_int8_t msg);
void hud_update();
#endif
