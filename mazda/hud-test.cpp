#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <dbus/dbus.h>
#include <poll.h>
#include <inttypes.h>
#include <cmath>
#include <functional>
#include <condition_variable>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <thread>
#include <iostream>


#include <dbus-c++/dbus.h>
#include <dbus-c++/glib-integration.h>
#include <sys/time.h>

#include "nm/mzd_nightmode.h"
#include "gps/mzd_gps.h"
#include "hud/hud.h"

#include "audio.h"
#include "main.h"
#include "command_server.h"
#include "callbacks.h"
#include "glib_utils.h"
#include "config.h"


#include <stdint.h>
#include <string>

static void hud_test_func(std::condition_variable& quitcv, std::mutex& quitmutex)
{


}

DBus::Glib::BusDispatcher dispatcher;

int main (int argc, char *argv[])
{
  DBus::default_dispatcher = &dispatcher;
  printf("DBus::Glib::BusDispatcher attached\n");

  printf("Connecting to DBUS\n");
  hud_start();
  printf("hud installed %d\n", hud_installed());

  uint32_t i = 1;
  uint8_t msg = 1;
  std::string test_string;
  while(true)
  {
    char buff[100];
    snprintf(buff, sizeof(buff), "DIRICON %u", i);
    test_string = buff;

    hud_send(i, 1000, test_string, msg);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    i++;
    if(msg <= 8){
      msg++;
    }
    else
    {
      msg = 1;
    }
  }
}
