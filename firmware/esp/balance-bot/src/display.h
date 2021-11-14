#ifndef _DISPLAY_H_
#define _DISPLAY_H_

class Display
{
    public:
        Display();
        void Begin();
        void DisplayText(const char * text, int waitms = 0);
};



#endif // _DISPLAY_H_
