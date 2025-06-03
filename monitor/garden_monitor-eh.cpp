#include <iostream>
#include <string>
#include <ncurses.h>
#include "json.hpp" // nlohmann/json

using json = nlohmann::json;

void draw_chart(WINDOW* win, const std::vector<int>& values) {
    werase(win);
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);

    int max_val = *std::max_element(values.begin(), values.end());
    for (size_t i = 0; i < values.size(); ++i) {
        int bar_height = (max_val > 0) ? (values[i] * max_y / max_val) : 0;
        for (int y = max_y - 1; y >= max_y - bar_height; --y) {
            mvwprintw(win, y, i, "|");
        }
    }
    box(win, 0, 0);
    wrefresh(win);
}

int main() {
    initscr();
    noecho();
    cbreak();
    timeout(1000); // non-blocking getch

    WINDOW* win = newwin(10, 60, 1, 1);
    std::vector<int> light_values;

    while (true) {
        // Simulate JSON input (replace with real UDP or stdin)
        std::string input = R"({"sensor": "light", "value": 45})";

        try {
            json j = json::parse(input);
            if (j["sensor"] == "light") {
                light_values.push_back(j["value"]);
                if (light_values.size() > 60)
                    light_values.erase(light_values.begin());
            }
        } catch (...) {
            // Ignore malformed input
        }

        draw_chart(win, light_values);

        int ch = getch();
        if (ch == 'q') break;
    }

    delwin(win);
    endwin();
    return 0;
}
