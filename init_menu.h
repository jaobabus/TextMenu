

#include "TextMenu.h"
#include "eepromref.hpp"




using LcdDriver = LiquidCrystalDriver;
using Page = ListLinePage<LcdDriver>;
using Line = Page::TextLine;
using Menu = TextMenu<LcdDriver>;
auto& back_page = Menu::back_page;


extern const Page start_page;
extern const Page settings_page;
extern const Page sensor_page[4];
extern const Page relay_page[4];


template<typename Type>
void eeprom_edit_duration(EepromRef<Type, Type> ref, const char* text, LcdDriver& lcd, RenderMenuAction act, uint8_t count) {
  static Type tmp;
    switch (act & RenderMenuAction::Mask) {
    case RenderMenuAction::Show:
        tmp = ref;
        break;
    case RenderMenuAction::FocusExit:
        ref = tmp;
        break;
    case RenderMenuAction::ScrollUp:
        tmp += count * (act & RenderMenuAction::FBoosted ? (tmp > 1800 ? 1800 : 60) : 1);
        break;
    case RenderMenuAction::ScrollDown:
        tmp -= count * (act & RenderMenuAction::FBoosted ? (tmp > 1800 ? 1800 : 60) : 1);
        break;
    default:
        break;
    }
    lcd.print(text);
    if (tmp >= 3600) {
        lcd.print(tmp / 3600);
        lcd.print(":");
    }
    if (tmp >= 60) {
        lcd.print((tmp / 60) % 60);
        lcd.print(":");
    }
    lcd.print(tmp % 60);
}

template<typename Type>
void eeprom_edit_int(EepromRef<Type, Type> ref, const char* text, LcdDriver& lcd, RenderMenuAction act, uint8_t count) {
  static Type tmp;
    switch (act & RenderMenuAction::Mask) {
    case RenderMenuAction::Show:
        tmp = ref;
        break;
    case RenderMenuAction::FocusExit:
        ref = tmp;
        break;
    case RenderMenuAction::ScrollUp:
        tmp += count * (act & RenderMenuAction::FBoosted ? 200 : 1);
        break;
    case RenderMenuAction::ScrollDown:
        tmp -= count * (act & RenderMenuAction::FBoosted ? 200 : 1);
        break;
    default:
        break;
    }
    lcd.print(text);
    lcd.print(tmp);
}

void eeprom_edit_sensor(EepromRef<uint8_t, uint8_t> ref, const char* text, LcdDriver& lcd, RenderMenuAction act, uint8_t count) {
    static uint8_t tmp;
    switch (act & RenderMenuAction::Mask) {
    case RenderMenuAction::Show:
        tmp = ref;
        break;
    case RenderMenuAction::FocusExit:
        ref = tmp;
        break;
    case RenderMenuAction::ScrollUp:
        tmp += count;
        break;
    case RenderMenuAction::ScrollDown:
        tmp -= count;
        break;
    default:
        break;
    }
    if (tmp >= _getn_of(sensor_page)) 
        tmp = 0;
    lcd.print(text);
    auto name = access<Page, const char>(sensor_page[tmp], &Page::name);
    lcd.pgm_print_text(name);
}

PROGMEM const char relay_modes[][7] = {{"normal"}, {"invert"}};

void eeprom_edit_relay_mode(EepromRef<uint8_t, uint8_t> ref, const char* text, LcdDriver& lcd, RenderMenuAction act, uint8_t count) {
    static uint8_t tmp;
    switch (act & RenderMenuAction::Mask) {
    case RenderMenuAction::Show:
        tmp = ref;
        break;
    case RenderMenuAction::FocusExit:
        ref = tmp;
        break;
    case RenderMenuAction::ScrollUp:
        tmp += count;
        break;
    case RenderMenuAction::ScrollDown:
        tmp -= count;
        break;
    default:
        break;
    }
    if (tmp >= _getn_of(relay_modes)) 
        tmp = 0;
    lcd.print(text);
    auto name = ProgmemPtr<const char>(relay_modes[tmp]);
    lcd.pgm_print_text(name);
}


struct SensorSetting {
    static void edit_interval(void* sens, LcdDriver& lcd, RenderMenuAction act, uint8_t count) {
        eeprom_edit_duration<decltype(interval)>(EepromRef<decltype(interval), decltype(interval)>(reinterpret_cast<SensorSetting*>(sens)->interval), "interv ", lcd, act, count);
    }
    static void edit_duration(void* sens, LcdDriver& lcd, RenderMenuAction act, uint8_t count) {
        eeprom_edit_duration<decltype(duration)>(EepromRef<decltype(duration), decltype(duration)>(reinterpret_cast<SensorSetting*>(sens)->duration), "dur ", lcd, act, count);
    }

    SensorSetting() {}

    uint16_t last_value;
    uint16_t interval;
    uint16_t duration;
};

struct RelaySetting {
    static void edit_sensor(void* sens, LcdDriver& lcd, RenderMenuAction act, uint8_t count) {
        eeprom_edit_sensor(EepromRef<decltype(sensor), decltype(sensor)>(reinterpret_cast<RelaySetting*>(sens)->sensor), "sensor ", lcd, act, count);
    }
    static void edit_duration(void* sens, LcdDriver& lcd, RenderMenuAction act, uint8_t count) {
        eeprom_edit_duration<decltype(duration)>(EepromRef<decltype(duration), decltype(duration)>(reinterpret_cast<RelaySetting*>(sens)->duration), "dur ", lcd, act, count);
    }
    static void edit_min_val(void* sens, LcdDriver& lcd, RenderMenuAction act, uint8_t count) {
        eeprom_edit_int<decltype(min_val)>(EepromRef<decltype(min_val), decltype(min_val)>(reinterpret_cast<RelaySetting*>(sens)->min_val), "min ", lcd, act, count);
    }
    static void edit_max_val(void* sens, LcdDriver& lcd, RenderMenuAction act, uint8_t count) {
        eeprom_edit_int<decltype(max_val)>(EepromRef<decltype(max_val), decltype(max_val)>(reinterpret_cast<RelaySetting*>(sens)->max_val), "max ", lcd, act, count);
    }
    static void edit_mode(void* sens, LcdDriver& lcd, RenderMenuAction act, uint8_t count) {
        eeprom_edit_relay_mode(EepromRef<decltype(mode), decltype(mode)>(reinterpret_cast<RelaySetting*>(sens)->mode), "mode ", lcd, act, count);
    }

    RelaySetting() {}

    uint8_t sensor;
    uint8_t mode;
    uint16_t duration;
    uint16_t min_val;
    uint16_t max_val;
};

extern SensorSetting sensor_setting[4];
extern RelaySetting relay_setting[4];



PROGMEM const Line start_lines[] = {
    {&settings_page}
};
PROGMEM const char start_page_name[] = {"start"};
PROGMEM const Page start_page(start_page_name, start_lines, _getn_of(start_lines));

PROGMEM const Line settings_lines[] = {
    {&back_page},

    {&sensor_page[0]},
    {&sensor_page[1]},
    {&sensor_page[2]},
    {&sensor_page[3]},
    {nullptr, nullptr},
    {&relay_page[0]},
    {&relay_page[1]},
    {&relay_page[2]},
    {&relay_page[3]},

};
PROGMEM const char settings_page_name[] = {"settings"};
PROGMEM const Page settings_page(settings_page_name, settings_lines, _getn_of(settings_lines));


EEMEM SensorSetting sensor_setting[4];

PROGMEM const Line sensor_lines[4][3] = {
    {
        {&back_page},
        {&SensorSetting::edit_duration, static_cast<void*>(&sensor_setting[0])},
        {&SensorSetting::edit_interval, static_cast<void*>(&sensor_setting[0])},
    },
    {
        {&back_page},
        {&SensorSetting::edit_duration, static_cast<void*>(&sensor_setting[1])},
        {&SensorSetting::edit_interval, static_cast<void*>(&sensor_setting[1])},
    },
    {
        {&back_page},
        {&SensorSetting::edit_duration, static_cast<void*>(&sensor_setting[2])},
        {&SensorSetting::edit_interval, static_cast<void*>(&sensor_setting[2])},
    },
    {
        {&back_page},
        {&SensorSetting::edit_duration, static_cast<void*>(&sensor_setting[3])},
        {&SensorSetting::edit_interval, static_cast<void*>(&sensor_setting[3])},
    },
};
PROGMEM const char sensor1_name[] = {"PH-meter"};
PROGMEM const char sensor2_name[] = {"sensor 2"};
PROGMEM const char sensor3_name[] = {"sensor 3"};
PROGMEM const char sensor4_name[] = {"sensor 4"};
PROGMEM const Page sensor_page[4] = {
    {sensor1_name, sensor_lines[0], _getn_of(sensor_lines[0])},
    {sensor2_name, sensor_lines[1], _getn_of(sensor_lines[1])},
    {sensor3_name, sensor_lines[2], _getn_of(sensor_lines[2])},
    {sensor4_name, sensor_lines[3], _getn_of(sensor_lines[3])},
};


EEMEM RelaySetting relay_setting[4];

PROGMEM const Line relay_lines[4][6] = {
    {
        {&back_page},
        {&RelaySetting::edit_sensor, static_cast<void*>(&relay_setting[0])},
        {&RelaySetting::edit_duration, static_cast<void*>(&relay_setting[0])},
        {&RelaySetting::edit_min_val, static_cast<void*>(&relay_setting[0])},
        {&RelaySetting::edit_max_val, static_cast<void*>(&relay_setting[0])},
        {&RelaySetting::edit_mode, static_cast<void*>(&relay_setting[0])},
    },
    {
        {&back_page},
        {&RelaySetting::edit_sensor, static_cast<void*>(&relay_setting[1])},
        {&RelaySetting::edit_duration, static_cast<void*>(&relay_setting[1])},
        {&RelaySetting::edit_min_val, static_cast<void*>(&relay_setting[1])},
        {&RelaySetting::edit_max_val, static_cast<void*>(&relay_setting[1])},
        {&RelaySetting::edit_mode, static_cast<void*>(&relay_setting[1])},
    },
    {
        {&back_page},
        {&RelaySetting::edit_sensor, static_cast<void*>(&relay_setting[2])},
        {&RelaySetting::edit_duration, static_cast<void*>(&relay_setting[2])},
        {&RelaySetting::edit_min_val, static_cast<void*>(&relay_setting[2])},
        {&RelaySetting::edit_max_val, static_cast<void*>(&relay_setting[2])},
        {&RelaySetting::edit_mode, static_cast<void*>(&relay_setting[2])},
    },
    {
        {&back_page},
        {&RelaySetting::edit_sensor, static_cast<void*>(&relay_setting[3])},
        {&RelaySetting::edit_duration, static_cast<void*>(&relay_setting[3])},
        {&RelaySetting::edit_min_val, static_cast<void*>(&relay_setting[3])},
        {&RelaySetting::edit_max_val, static_cast<void*>(&relay_setting[3])},
        {&RelaySetting::edit_mode, static_cast<void*>(&relay_setting[3])},
    },
};
PROGMEM const char relay1_name[] = {"relay X"};
PROGMEM const char relay2_name[] = {"relay Y"};
PROGMEM const char relay3_name[] = {"relay 3"};
PROGMEM const char relay4_name[] = {"relay 4"};
PROGMEM const Page relay_page[4] = {
    {relay1_name, relay_lines[0], _getn_of(relay_lines[0])},
    {relay2_name, relay_lines[1], _getn_of(relay_lines[1])},
    {relay3_name, relay_lines[2], _getn_of(relay_lines[2])},
    {relay4_name, relay_lines[3], _getn_of(relay_lines[3])},
};










struct Relay {
  Relay(uint8_t pin, EepromRef<RelaySetting> stt): pin(pin), settings(stt) {
    _TaskContainer::_global_init_ref().push(Task(this, reinterpret_cast<bool(*)(void*, uint32_t)>(&_check_to_action)));  
  }

private:
  EepromRef<RelaySetting> settings;

  const uint8_t pin;
  volatile uint32_t last_action;
  volatile bool action;

  bool _do_action(uint32_t ms) {
    if (ms - last_action > settings().duration * 1000ull) {
      digitalWrite(pin, false);
      action = false;
      return false;
    }
    return true;
  }

  bool _check_to_action(uint32_t ms) {
    if (action)
      return true;
    uint8_t sensor = settings().sensor;
    uint16_t vmin = settings().min_val;
    uint16_t vmax = settings().max_val;
    uint16_t value = EepromRef<uint16_t, uint16_t>(sensor_setting[sensor].last_value);
    if ((vmin < value && value < vmax) && settings().mode == 0 ||
        !(vmin < value && value < vmax) && settings().mode == 1) {
      last_action = ms;
      action = true;
      digitalWrite(pin, true);
      tasks.push(Task(this, reinterpret_cast<bool(*)(void*, uint32_t)>(&_do_action))); 
    }
    return true;
  }
  
};
