#pragma once


#include "MenuPage.h"
#include <static_stack.hpp>




template<typename LcdDriver>
class TextMenu {
public:
    PROGMEM static const char back_page_name[];
    PROGMEM static const ListLinePage<LcdDriver> back_page;
    using TextLine = typename ListLinePage<LcdDriver>::TextLine;

public:
    TextMenu(LcdDriver& lcd, const ListLinePage<LcdDriver>* start_page, bool skip_empty_lines = false): 
        lcd(lcd), active_page(start_page),
        skip_empty_lines(skip_empty_lines),
        active_line(0), line_pos(0), focus(false)
    {}

    void on_timer() {
        _show_line(access<ListLinePage<LcdDriver>, const TextLine>(active_page, &ListLinePage<LcdDriver>::lines)[active_line], line_pos, 1, RenderMenuAction::Timer);
    }

    void show() {
        _show_lines();
    }

    void on_key_up(uint8_t count = 1, bool boosted = false) {
        auto line = access<ListLinePage<LcdDriver>, const TextLine>(active_page, &ListLinePage<LcdDriver>::lines)[active_line];
        if (focus) {
            _show_line(line, line_pos, count, RenderMenuAction::ScrollDown | (boosted ? RenderMenuAction::FBoosted : 0));
        }
        else {
            active_line--;
            auto count = access<ListLinePage<LcdDriver>, const uint8_t>(active_page, &ListLinePage<LcdDriver>::count)();
            if (active_line >= count) {
                active_line = count - 1;
                line_pos = min(lcd.y_size, count) - 1;
            }
            else if (line_pos != 0)
                line_pos--;
            _show_lines();
        }
    }

    void on_key_enter() {
        auto line = access<ListLinePage<LcdDriver>, const TextLine>(active_page, &ListLinePage<LcdDriver>::lines)[active_line];
        if (access<TextLine, const typename TextLine::Callback>(line, &TextLine::callback)) {
            focus = !focus;
            if (!focus)
                _show_line(line, line_pos, 1, RenderMenuAction::FocusExit);
        }
        else {
            if (access<TextLine, const ListLinePage<LcdDriver>>(line, &TextLine::next_page) == &back_page) {
                active_page = stack_pages.back();
                stack_pages.pop_back();
            }
            else if (access<TextLine, const ListLinePage<LcdDriver>>(line, &TextLine::next_page) == nullptr)
                return;
            else {
                stack_pages.push_back(active_page);
                active_page = access<TextLine, const ListLinePage<LcdDriver>>(line, &TextLine::next_page);
            }
            active_line = 0;
            line_pos = 0;
            focus = false;
        }
        _show_lines();
    }

    void on_key_down(uint8_t count = 1, bool boosted = false) {
        auto line = access<ListLinePage<LcdDriver>, const TextLine>(active_page, &ListLinePage<LcdDriver>::lines)[active_line];
        //std::cout << typeid(line).name();
        if (focus) {
            _show_line(line, line_pos, count, RenderMenuAction::ScrollUp | (boosted ? RenderMenuAction::FBoosted : 0));
        }
        else {
            active_line++;
            if (active_line >= access<ListLinePage<LcdDriver>, const uint8_t>(active_page, &ListLinePage<LcdDriver>::count)()) {
                active_line = 0;
                line_pos = 0;
            }
            else if (line_pos < lcd.y_size - 1)
                line_pos++;
            _show_lines();
        }
    }

    bool has_focus() const noexcept {
        return focus;
    }

    bool skip_empty_lines;

private:
    void _show_lines() {
        lcd.clear();
        uint8_t begin = active_line - line_pos;
        auto count = access<ListLinePage<LcdDriver>, const uint8_t>(active_page, &ListLinePage<LcdDriver>::count)();
        uint8_t end = begin + min(lcd.y_size, count);
        auto lines = access<ListLinePage<LcdDriver>, const TextLine>(active_page, &ListLinePage<LcdDriver>::lines);
        for (uint8_t i = begin; i < end; i++)
            _show_line(lines[i], i - begin, 1, RenderMenuAction::Show, false);
    }
    void _show_line(ProgmemRef<typename ListLinePage<LcdDriver>::TextLine const> line, uint8_t y, uint8_t count, RenderMenuAction event = RenderMenuAction::Show, bool cll = true) {
        if (cll) lcd.clear_line(y);
        lcd.set_cursor_pos(0, y);
        if (access<TextLine, const typename TextLine::Callback>(line, &TextLine::callback)()) {
            access<TextLine, const typename TextLine::Callback>(line, &TextLine::callback)()(access<TextLine, void*>(line, &TextLine::user_ptr)(), lcd, event, count);
        }
        else if (access<TextLine, const ListLinePage<LcdDriver>>(line, &TextLine::next_page)) {
            auto page = access<TextLine, const ListLinePage<LcdDriver>>(line, &TextLine::next_page);
            auto name = access<ListLinePage<LcdDriver>, const char*>(page, &ListLinePage<LcdDriver>::name)();
            uint8_t size = min(strlen_P(name), 16);
            static char buf[16];
            memcpy_P(buf, name, size + 1);
            lcd.print_text(buf, size);
        }
        lcd.set_cursor_pos(lcd.x_size - 1, y);
        if (y == line_pos) {
            if (focus)
                lcd.print_text("<", 1);
            else
                lcd.print_text("-", 1);
        }
    }


    static_stack<ProgmemPtr<const ListLinePage<LcdDriver>>, 16> stack_pages;
    LcdDriver& lcd;
    ProgmemPtr<const ListLinePage<LcdDriver>> active_page;
    uint8_t active_line;
    uint8_t line_pos;
    bool focus;

};


template<typename T>
const char TextMenu<T>::back_page_name[] = {"back"};
template<typename T>
const ListLinePage<T> TextMenu<T>::back_page = {TextMenu<T>::back_page_name, nullptr, 0};
