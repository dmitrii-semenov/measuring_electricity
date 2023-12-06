#include <code_functions.h>
#include <oled.h>

void Initial_display(void)
{
    oled_init(OLED_DISP_ON);                                        
    oled_clrscr();
    oled_charMode(NORMALSIZE);
    oled_gotoxy(0, 0);
    oled_puts("Measuring:");
    oled_gotoxy(0, 1);
    oled_puts("Current");
    oled_gotoxy(0, 2);
    oled_puts("Voltage");
    oled_gotoxy(0, 3);
    oled_puts("Resistance");
    oled_gotoxy(0, 4);
    oled_puts("Capacitance");
    oled_gotoxy(0, 5);
    oled_puts("-------------------------");
}

void Clear_values(void)
{
    oled_gotoxy(14, 1);
    oled_puts("       ");
    oled_gotoxy(14, 2);
    oled_puts("       ");
    oled_gotoxy(14, 3);
    oled_puts("       ");
    oled_gotoxy(14, 4);
    oled_puts("       ");
    oled_gotoxy(20, 1);
    oled_puts(" ");
    oled_gotoxy(20, 2);
    oled_puts(" ");
    oled_gotoxy(20, 3);
    oled_puts(" ");
    oled_gotoxy(20, 4);
    oled_puts(" ");
}