#include "ascii.h"

int ascii_get_digit_x_offset(int digit_index)
{
    static const int offset[6] = {
        0, 6, 15, 21, 30, 36
    };

    if (digit_index < 0 || digit_index > 5) {
        return 0;
    }

    return offset[digit_index];
}

void ascii_time(int hours, int minutes, int seconds, int y, int x)
{
    ascii_digit(hours / 10,   y, x);
    ascii_digit(hours % 10,   y, x + 6);

    ascii_colon(y, x + 12);

    ascii_digit(minutes / 10, y, x + 15);
    ascii_digit(minutes % 10, y, x + 21);

    ascii_colon(y, x + 27);

    ascii_digit(seconds / 10, y, x + 30);
    ascii_digit(seconds % 10, y, x + 36);
}

void ascii_colon(int y, int x)
{
    mvprintw(y + 1, x, "█");
    mvprintw(y + 3, x, "█");
}

void ascii_digit(int digit, int y, int x)
{
    const char *digits[10][5] = {
        /* 0 */
        {
            " ███ ",
            "█   █",
            "█   █",
            "█   █",
            " ███ "
        },
        /* 1 */
        {
            "  █  ",
            " ██  ",
            "  █  ",
            "  █  ",
            " ████"
        },
        /* 2 */
        {
            " ███ ",
            "█   █",
            "   ██",
            " ██  ",
            "█████"
        },
        /* 3 */
        {
            "████ ",
            "    █",
            " ███ ",
            "    █",
            "████ "
        },
        /* 4 */
        {
            "█  █ ",
            "█  █ ",
            "█████",
            "   █ ",
            "   █ "
        },
        /* 5 */
        {
            "█████",
            "█    ",
            "████ ",
            "    █",
            "████ "
        },
        /* 6 */
        {
            " ███ ",
            "█    ",
            "████ ",
            "█   █",
            " ███ "
        },
        /* 7 */
        {
            "█████",
            "   █ ",
            "  █  ",
            " █   ",
            "█    "
        },
        /* 8 */
        {
            " ███ ",
            "█   █",
            " ███ ",
            "█   █",
            " ███ "
        },
        /* 9 */
        {
            " ███ ",
            "█   █",
            " ████",
            "    █",
            " ███ "
        }
    };

    if (digit < 0 || digit > 9)
        return;

    for (int row = 0; row < 5; row++) {
        mvprintw(y + row, x, "%s", digits[digit][row]);
    }
}
