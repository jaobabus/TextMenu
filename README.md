# TextMenu

Library for text displays
tested on LCD1602

May be used for many displays

TextMenu<LiquidCrystalDriver> text_menu1(lcd1);
TextMenu<LiquidCrystalDriver> text_menu2(lcd2);


Example:

class TestTarget {
public:
    TestTarget() {}
    
    static void static_callback(void* pthis, LcdDriver& lcd, RenderMenuAction act, uint8_t count) {
        reinterpret_cast<TestTarget*>(pthis)->callback(lcd, act, count); 
    }
    void callback(LcdDriver& lcd, RenderMenuAction action, uint8_t count) {
        // lcd: draw target
        // count: count key clicked (if key clicked more thar 1 times between call on_key)
        // do 
    }

};


TestTarget target;

PROGMEM const Page::TextLine line = {
    {&TestTarget::static_callback, static_cast<void*>(&target)},
};

PROGMEM const char page_name[] = {"Test page"};
ListLinePage<LcdDriver> page = {
    {page_name, &line, 1}
};

LiquidCrystal lcd_
LiquidCrystalDriver lcd(lcd_);
TextMenu<LiquidCrystalDriver> text_menu(lcd, &page); // lcd driver, start page


void loop() {
  bool key_up = get_state_of_some_key1_clicked(), key_down = get_state_of_some_key1_clicked();
  bool enter = get_state_of_some_key1_clicked();
  
  bool enter_down = get_state_of_some_key1_pressed();
  
  if (key_up)
      text_menu.on_key_up(1, enter_down);
  if (key_down)
      text_menu.on_key_down(1, enter_down);
  if (enter)
      text_menu.on_key_enter();

}




