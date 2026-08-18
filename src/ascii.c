#include "ascii.h"

void ascii_time(int hours, int minutes, int seconds, int y, int x)
{
    ascii_digit(hours / 10,   y, x);
    ascii_digit(hours % 10,   y, x + 6);

    ascii_colon(y, x + 11);

    ascii_digit(minutes / 10, y, x + 13);
    ascii_digit(minutes % 10, y, x + 19);

    ascii_colon(y, x + 24);

    ascii_digit(seconds / 10, y, x + 26);
    ascii_digit(seconds % 10, y, x + 32);
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
