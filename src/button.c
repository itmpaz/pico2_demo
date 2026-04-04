#include "button.h"

// Internal state
static bool     _button_last_raw_state    = false;
static bool     _button_stable_state      = false;
static bool     _button_prev_stable_state = false;

static uint32_t _debounce_start_ms        = 0;
static bool     _debounce_active          = false;

static uint32_t _last_click_time_ms       = 0;
static bool     _waiting_for_dblclick     = false;

static uint32_t _singleclick_counter      = 0;
static uint32_t _doubleclick_counter      = 0;




bool button_sleep(uint32_t delay)
{
    uint32_t c = _singleclick_counter + _doubleclick_counter;
    for(int i=0;i<delay;i++)
    {   button_update();       
        if (c != _singleclick_counter + _doubleclick_counter)
            return true;
        sleep_ms(1); 
    }
    return false;
}



void button_init() {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);          // active-low wiring (pressed = GND)

    _button_last_raw_state    = false;
    _button_stable_state      = false;
    _button_prev_stable_state = false;
    _debounce_active          = false;
    _waiting_for_dblclick     = false;
    _last_click_time_ms       = 0;
    _singleclick_counter      = 0;
    _doubleclick_counter      = 0;
}


void button_update() {
    uint32_t now_ms = to_ms_since_boot(get_absolute_time());

    // GPIO is active-low (pull-up), so invert: pressed = true
    bool raw = !gpio_get(BUTTON_PIN);

    // ------------------------------------------------------------------ //
    // 1.  Debounce logic
    // ------------------------------------------------------------------ //
    if (raw != _button_last_raw_state) {
        // Raw signal changed – restart the debounce timer
        _debounce_start_ms     = now_ms;
        _debounce_active       = true;
        _button_last_raw_state = raw;
    }

    if (_debounce_active &&
        (now_ms - _debounce_start_ms) >= BUTTON_DEBOUNCE_TIME_MS) {
        // Signal stable long enough – latch it
        _debounce_active          = false;
        _button_prev_stable_state = _button_stable_state;   // save old state
        _button_stable_state      = raw;                    // accept new state
    }

    // ------------------------------------------------------------------ //
    // 2.  Edge detection – rising edge = button pressed
    //     NOTE: _button_prev_stable_state is ONLY updated inside the
    //     debounce block above, so this comparison is valid every tick.
    // ------------------------------------------------------------------ //
    bool rising_edge = (_button_stable_state && !_button_prev_stable_state);

    if (rising_edge) {
        // Consume the edge by syncing prev to current
        _button_prev_stable_state = _button_stable_state;

        if (_waiting_for_dblclick &&
            (now_ms - _last_click_time_ms) <= BUTTON_DBLCLICK_MAXTIME_MS) {
            // Second click within window → double-click
            _doubleclick_counter++;
            printf("DBL\n");
            _waiting_for_dblclick = false;
        } else {
            // First click – start waiting for a possible second click
            _last_click_time_ms   = now_ms;
            _waiting_for_dblclick = true;
        }
    }

    // ------------------------------------------------------------------ //
    // 3.  Double-click window expired → confirm as single click
    // ------------------------------------------------------------------ //
    if (_waiting_for_dblclick &&
        (now_ms - _last_click_time_ms) > BUTTON_DBLCLICK_MAXTIME_MS) {
        _singleclick_counter++;
        printf("SNGL\n");
        _waiting_for_dblclick = false;
    }
}



uint32_t button_get_singleclick_counter() {
    return _singleclick_counter;
}


uint32_t button_get_doubleclick_counter() {
    return _doubleclick_counter;
}

void button_clickreset()
{
    _doubleclick_counter=0;
    _singleclick_counter=0;
}
