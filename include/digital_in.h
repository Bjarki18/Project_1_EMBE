#include <stdint.h>

class Digital_in {
    public: 
        Digital_in(uint8_t pin_number);
        void init();
        bool is_hi();
        bool is_lo();
    private:
        uint8_t pinMask;
};