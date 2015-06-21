
// status LED

class Status
{
    private:
        int pin;
        float level;
        unsigned int blink_t;
        float blink_level;

    public:
        Status(int pin);

        // set constant ouput level
        void set(float level);

        // set temporary output level
        void blink(float level);

        void update();
};

