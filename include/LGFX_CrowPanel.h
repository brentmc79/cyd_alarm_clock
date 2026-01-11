#ifndef LGFX_CROWPANEL_H
#define LGFX_CROWPANEL_H

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lgfx/v1/platforms/esp32s3/Panel_RGB.hpp>
#include <lgfx/v1/platforms/esp32s3/Bus_RGB.hpp>

class LGFX : public lgfx::LGFX_Device
{
  lgfx::Panel_RGB     _panel_instance;
  lgfx::Bus_RGB       _bus_instance;
  lgfx::Light_PWM     _light_instance;

public:
  LGFX(void)
  {
    {
      auto cfg = _panel_instance.config();

      cfg.memory_width  = 800;
      cfg.memory_height = 480;
      cfg.panel_width  = 800;
      cfg.panel_height = 480;

      cfg.offset_x = 0;
      cfg.offset_y = 0;

      _panel_instance.config(cfg);
    }

    {
      auto cfg = _bus_instance.config();
      cfg.panel = &_panel_instance;

      // Official pin mapping from Elecrow GitHub
      // https://github.com/Elecrow-RD/CrowPanel-5.0-HMI-ESP32-Display-800x480

      // Blue channel (B0-B4)
      cfg.pin_d0  = GPIO_NUM_8;   // B0
      cfg.pin_d1  = GPIO_NUM_3;   // B1
      cfg.pin_d2  = GPIO_NUM_46;  // B2
      cfg.pin_d3  = GPIO_NUM_9;   // B3
      cfg.pin_d4  = GPIO_NUM_1;   // B4

      // Green channel (G0-G5)
      cfg.pin_d5  = GPIO_NUM_5;   // G0
      cfg.pin_d6  = GPIO_NUM_6;   // G1
      cfg.pin_d7  = GPIO_NUM_7;   // G2
      cfg.pin_d8  = GPIO_NUM_15;  // G3
      cfg.pin_d9  = GPIO_NUM_16;  // G4
      cfg.pin_d10 = GPIO_NUM_4;   // G5

      // Red channel (R0-R4)
      cfg.pin_d11 = GPIO_NUM_45;  // R0
      cfg.pin_d12 = GPIO_NUM_48;  // R1
      cfg.pin_d13 = GPIO_NUM_47;  // R2
      cfg.pin_d14 = GPIO_NUM_21;  // R3
      cfg.pin_d15 = GPIO_NUM_14;  // R4

      // Control signals
      cfg.pin_henable = GPIO_NUM_40;  // DE (Data Enable)
      cfg.pin_vsync   = GPIO_NUM_41;  // VSYNC
      cfg.pin_hsync   = GPIO_NUM_39;  // HSYNC
      cfg.pin_pclk    = GPIO_NUM_0;   // Pixel clock
      cfg.freq_write  = 15000000;     // 15MHz pixel clock

      cfg.hsync_polarity    = 0;
      cfg.hsync_front_porch = 8;
      cfg.hsync_pulse_width = 4;
      cfg.hsync_back_porch  = 43;

      cfg.vsync_polarity    = 0;
      cfg.vsync_front_porch = 8;
      cfg.vsync_pulse_width = 4;
      cfg.vsync_back_porch  = 12;

      cfg.pclk_active_neg   = 1;
      cfg.pclk_idle_high    = 0;
      cfg.de_idle_high      = 0;

      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }

    {
      auto cfg = _light_instance.config();

      cfg.pin_bl = GPIO_NUM_2;  // Backlight control (verified from schematic)
      cfg.invert = false;
      cfg.freq   = 5000;
      cfg.pwm_channel = 0;

      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }

    setPanel(&_panel_instance);
  }
};

#endif // LGFX_CROWPANEL_H
