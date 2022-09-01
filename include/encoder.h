#include <avr/io.h>

class Encoder {
    public: 
        Encoder();
        int position();
        void increment();
        void decrement();
    private:
        int pos;
};