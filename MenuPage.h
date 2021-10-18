#pragma once


#include <inttypes.h>

#include "progmemref.hpp"





enum RenderMenuAction: uint8_t {
    Show,
    FocusExit,
    Timer,
    ScrollUp,
    ScrollDown,

    Mask = 63,
    FBoosted = 64
    
};


template<typename LcdDriver>
struct ListLinePage {
    struct TextLine;
    using Callback = void(*)(void*, LcdDriver&, RenderMenuAction, uint8_t);

    constexpr ListLinePage(const ProgmemPtr<const char> name,const TextLine* lines, uint8_t count): 
        name(name.get_ptr()), lines(lines), count(count) {}

    struct TextLine {
        using Callback = typename ListLinePage::Callback;
        constexpr TextLine(): next_page(nullptr), callback(nullptr), is_callback(false) {}
        constexpr TextLine(ProgmemPtr<const ListLinePage> page):
            next_page(page.get_ptr()), callback(nullptr), is_callback(false) {}
        constexpr TextLine(Callback callback, void* user_ptr = nullptr):
            user_ptr(user_ptr), callback(callback), is_callback(true) {}
        const bool is_callback;
        union {
            void* user_ptr;
            const ListLinePage* next_page;
        };
        const Callback callback;
    };
   
    
    const char* name;
    const TextLine* lines;
    const uint8_t count;

};
