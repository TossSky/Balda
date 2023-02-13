#define WIN32_LEAN_AND_MEAN
#include <conio.h>
#include <windows.h>
#include <malloc.h>
#include <locale.h>
#include <stdio.h>

#include "wincon_new.h"

HANDLE g_shadow_buf = 0;
CHAR_INFO* g_shadow_buf_data = 0;
unsigned int g_shadow_buf_size = 0;

void con_init(short width, short height)
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD c;

    setlocale(LC_ALL, "Russian");

    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    c.X = width;
    c.Y = height;

    g_shadow_buf = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
        0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

//  ===============================================  ����������� �������  =======================================  1  ==
short con_width_new() {  ///  ������ con_width()
    HANDLE hWndConsole;
    if (hWndConsole = GetStdHandle(-12))
    {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if (GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo))
        {
            int widht = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
            return widht;
        }
        else
            return 0;
    }
    else
        return 0;
}

short con_height_new() {  ///  ������ con_height()
    HANDLE hWndConsole;
    if (hWndConsole = GetStdHandle(-12))
    {
        CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
        if (GetConsoleScreenBufferInfo(hWndConsole, &consoleInfo))
        {
            int height = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;
            return height;
        }
        else
            return 0;
    }
    else
        return 0;
}

int symbol_code(int mode) {
    ///  ������� ������ �������(mode == 0)/����������(mode == 1) ������, ������� ��� �� ����� � ���������� ��� ��� (� unsigned char) (0 - �������� ����������� �������)
    if (mode == 0) {  //  ���������
        int code;
        do {
            key_is_pressed();  //  ������ ������
            code = key_pressed_code();
            //  �������� �������
            if (code == KEY_BACK) {
                return 0;
            }
        } while (!(code >= 224 && code <= 255) && code != 480);
        //  ������� ������� ������ (������ �����)
        printf("%c", code);
        return code;

        /*switch (code) {
            case 160:  //  '�'
                return 224;
            case 161:  //  '�'
                return 225;
            case 162:  //  '�'
                return 226;
            case 163:  //  '�'
                return 227;
            case 164:  //  '�'
                return 228;
            case 165:  //  '�'
                return 229;
            case 166:  //  '�'
                return 230;
            case 167:  //  '�'
                return 231;
            case 168:  //  '�'
                return 232;
            case 169:  //  '�'
                return 233;
            case 170:  //  '�'
                return 234;
            case 171:  //  '�'
                return 235;
            case 172:  //  '�'
                return 236;
            case 173:  //  '�'
                return 237;
            case 174:  //  '�'
                return 238;
            case 175:  //  '�'
                return 239;
            case 224:  //  '�'
                return 240;
            case 225:  //  '�'
                return 241;
            case 226:  //  '�'
                return 242;
            case 227:  //  '�'
                return 243;
            case 228:  //  '�'
                return 244;
            case 229:  //  '�'
                return 245;
            case 230:  //  '�'
                return 246;
            case 231:  //  '�'
                return 247;
            case 232:  //  '�'
                return 248;
            case 233:  //  '�'
                return 249;
            case 234:  //  '�'
                return 250;
            case 235:  //  '�'
                return 251;
            case 236:  //  '�'
                return 252;
            case 237:  //  '�'
                return 253;
            case 238:  //  '�'
                return 254;
            case 239:  //  '�'
                return 255;
            case 8:  //  'Backspace'
                return 0;
            default:  //  ���������� �������
                return -1;
        }*/

    }
    else if (mode == 1) {  //  ����������
        int code;
        do {
            key_is_pressed();  //  ������ ������
            code = key_pressed_code();
            //  �������� �������
            if (code == KEY_BACK) {
                return 0;
            }
        } while (!(code >= 97 && code <= 122) && !(code >= 65 && code <= 90));
        //  ������� ������� ������ (������ �����)
        printf("%c", code);
        return code;
    }

    return -1;
}
//  =============================================================================================================  1  ==

short con_width()
{  ///  �������� �������
    CONSOLE_SCREEN_BUFFER_INFO cbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cbi);
    return (cbi.srWindow.Right - cbi.srWindow.Left) + 1;
}

short con_height()
{  ///  �������� �������
    CONSOLE_SCREEN_BUFFER_INFO cbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cbi);
    return (cbi.srWindow.Bottom - cbi.srWindow.Top) + 1;
}

void con_redraw()
{
    /**/
}

void con_draw_lock()
{
    HANDLE from = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE to = g_shadow_buf;
    CONSOLE_SCREEN_BUFFER_INFO cbi;
    SMALL_RECT rc;
    COORD zero_coord = { 0, 0 };
    CONSOLE_CURSOR_INFO ci;

    if (!g_shadow_buf)
        return;


    GetConsoleScreenBufferInfo(from, &cbi);
    if (g_shadow_buf_size < sizeof(CHAR_INFO) * cbi.dwSize.X * cbi.dwSize.Y)
    {
        if (g_shadow_buf_data)
            free(g_shadow_buf_data);
        g_shadow_buf_size = sizeof(CHAR_INFO) * cbi.dwSize.X * cbi.dwSize.Y;
        g_shadow_buf_data = malloc(g_shadow_buf_size);
        if (!g_shadow_buf_data)
            g_shadow_buf_size = 0;
    }

    rc.Left = 0;
    rc.Top = 0;
    rc.Right = cbi.dwSize.X - 1;
    rc.Bottom = cbi.dwSize.Y - 1;

    ReadConsoleOutput(from, g_shadow_buf_data, cbi.dwSize, zero_coord, &rc);
    WriteConsoleOutput(to, g_shadow_buf_data, cbi.dwSize, zero_coord, &rc);

    GetConsoleCursorInfo(from, &ci);
    SetConsoleCursorInfo(to, &ci);

    SetConsoleActiveScreenBuffer(g_shadow_buf);
}

void con_draw_release()
{
    SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
}

void con_set_color(short font, short background)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), font | (background << 4));
}

void clrscr()
{
    DWORD len;
    DWORD written;

    CONSOLE_SCREEN_BUFFER_INFO cbi;
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD c;
    c.X = 0;
    c.Y = 0;

    GetConsoleScreenBufferInfo(handle, &cbi);
    len = cbi.dwSize.X * cbi.dwSize.Y;

    FillConsoleOutputCharacterA(handle, ' ', len, c, &written);
    FillConsoleOutputAttribute(handle, cbi.wAttributes, len, c, &written);
}

void gotoxy(short x, short y)
{
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

void show_cursor(int show)
{
    CONSOLE_CURSOR_INFO cci;
    cci.dwSize = sizeof(cci);
    cci.bVisible = show;

    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);
}

void pause(int ms)
{
    Sleep((DWORD)ms);
}

int key_is_pressed()
{
    return _kbhit();
}

int key_pressed_code()
{
    int ch = _getch();
    if (ch == 0 || ch == 224)
        ch = 256 + _getch();
    return ch;
}
