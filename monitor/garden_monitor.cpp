#include "json.hpp" // This is nlohmann/json
#include <cstring>
#include <curses.h>
#include <iostream>
#include <ncurses.h>
#include <netinet/in.h>
#include <string>
#include <unistd.h>
#include <time.h>

#define PORT 4210 // Match UDP_PORT on the ESP32
#define BUFFER_SIZE 1024
#define LIGHT_LEVEL_LOW_PAIR 1
#define LIGHT_LEVEL_MEDIUM_PAIR 2
#define LIGHT_LEVEL_HIGH_PAIR 3
#define TEXT_NORMAL_PAIR 4
#define TEXT_ERROR_PAIR 5
#define TEXT_MESSAGE_PAIR 6

using json = nlohmann::json;

void draw_box_with_title(WINDOW *win, const char *title) {
    box(win, 0, 0); // Draw the default border

    // Calculate position to center the title
    int max_x;
    getmaxyx(win, std::ignore, max_x);
    int title_start = (max_x - strlen(title)) / 2;

    // Print the title on the top border (row 0)
    mvwprintw(win, 0, title_start, " %s ", title); // Spaces to cleanly overwrite border

    wrefresh(win);
}

void draw_chart(WINDOW *win, const std::vector<int> &values, bool colored, const char *title) {
  werase(win);
  int max_y, max_x;
  getmaxyx(win, max_y, max_x);

  size_t display_width = std::min(values.size(), static_cast<size_t>(max_x - 2));
  int max_val = *std::max_element(values.begin(), values.end());


  // Draw level markers on the right side with color coding
  int num_markers = 6;
  for (int i = 0; i <= num_markers; ++i) {
    int marker_y = 1 + i * (max_y - 3) / num_markers;
    int marker_val = max_val - (i * max_val / num_markers);

    if (colored) {
      int color_pair = LIGHT_LEVEL_LOW_PAIR;
      if (marker_val > max_val * 0.66)
        color_pair = LIGHT_LEVEL_HIGH_PAIR;
      else if (marker_val > max_val * 0.33)
        color_pair = LIGHT_LEVEL_MEDIUM_PAIR;

      wattron(win, COLOR_PAIR(color_pair));
      mvwprintw(win, marker_y, max_x - 5, "%4d |", marker_val);
      wattroff(win, COLOR_PAIR(color_pair));
    } else {
      mvwprintw(win, marker_y, max_x - 5, "%4d |", marker_val);
    }
  }

  for (size_t i = 0; i < display_width - 5; ++i) {
    int bar_height = (max_val > 0) ? (values[i] * (max_y - 1) / max_val) : 0;
    if (colored) {
      int color_pair = 0;
      if (bar_height < 4) color_pair = LIGHT_LEVEL_LOW_PAIR;
      else if (bar_height <= 7) color_pair = LIGHT_LEVEL_MEDIUM_PAIR;
      else color_pair = LIGHT_LEVEL_HIGH_PAIR;
      wattron(win, COLOR_PAIR(color_pair));
      for (int y = max_y - 2; y >= max_y - 2 - bar_height; --y) {
        mvwprintw(win, y, i + 1, "*");
      }
      wattroff(win, COLOR_PAIR(color_pair));
    } else {
      for (int y = max_y - 2; y >= max_y - 2 - bar_height; --y) {
        mvwprintw(win, y, i + 1, "/");
      }
    }
  }

  draw_box_with_title(win, title);
  wrefresh(win);
}

int main() {
  // Buffer for reading input from ESP32
  char buffer[BUFFER_SIZE];
  // list of light values.
  std::vector<int> light_sensor_values;
  std::vector<int> soil_moisture_1_values;
  std::vector<int> soil_moisture_2_values;
  std::vector<int> soil_moisture_3_values;
  std::vector<int> soil_moisture_4_values;
  int console_max_x, console_max_y;

  // Initialize ncurses
  initscr();
  cbreak();
  noecho();

  getmaxyx(stdscr, console_max_y, console_max_x);

  int chart_height = (console_max_y - 5) / 3;

  // initialize the light values all to 0.
  light_sensor_values.assign(console_max_x, 0);
  soil_moisture_1_values.assign(console_max_x / 2, 0);
  soil_moisture_2_values.assign(console_max_x / 2, 0);
  soil_moisture_3_values.assign(console_max_x / 2, 0);
  soil_moisture_4_values.assign(console_max_x / 2, 0);

  // Create the NCurses Window
  WINDOW* log_window = newwin(7, console_max_x - 2, console_max_y - 6, 1);
  scrollok(log_window, true);

  WINDOW* light_sensor_win = newwin(chart_height, console_max_x - 2, 1, 1);
  WINDOW* soil_moisture_sensor_1_win = newwin(chart_height, console_max_x / 2 - 1,
                                              chart_height + 1, 1);
  WINDOW* soil_moisture_sensor_2_win = newwin(chart_height, console_max_x / 2 - 1,
                                              chart_height + 1, console_max_x / 2);
  WINDOW* soil_moisture_sensor_3_win = newwin(chart_height, console_max_x / 2 - 1,
                                              chart_height * 2 + 1, 1);
  WINDOW* soil_moisture_sensor_4_win = newwin(chart_height, console_max_x / 2 - 1,
                                              chart_height * 2 + 1, console_max_x / 2);
  bool colored = has_colors();
  if (colored) {
    start_color();
    init_pair(TEXT_NORMAL_PAIR, COLOR_GREEN, COLOR_BLACK);
    init_pair(TEXT_ERROR_PAIR, COLOR_WHITE, COLOR_RED);
    init_pair(TEXT_MESSAGE_PAIR, COLOR_BLUE, COLOR_BLACK);
    init_pair(LIGHT_LEVEL_HIGH_PAIR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(LIGHT_LEVEL_LOW_PAIR, COLOR_BLUE, COLOR_BLACK);
    init_pair(LIGHT_LEVEL_MEDIUM_PAIR, COLOR_GREEN, COLOR_BLACK);
  } else {
    printf("Your terminal does not support color\n");
    mvprintw(chart_height + 3, 1, "No Color Support\n.");
  }

  clear();
  refresh();

  if (colored) { wattron(log_window, COLOR_PAIR(TEXT_ERROR_PAIR)); }
  wprintw(log_window, "Waiting for input.\n");
  if (colored) { wattroff(log_window, COLOR_PAIR(TEXT_ERROR_PAIR)); }
  wrefresh(log_window);

  draw_chart(light_sensor_win, light_sensor_values, colored, "Light Sensor");
  draw_chart(soil_moisture_sensor_1_win, soil_moisture_1_values, colored, "Soil Moisture Sensor 1");
  draw_chart(soil_moisture_sensor_2_win, soil_moisture_2_values, colored, "Soil Moisture Sensor 2");
  draw_chart(soil_moisture_sensor_3_win, soil_moisture_3_values, colored, "Soil Moisture Sensor 3");
  draw_chart(soil_moisture_sensor_4_win, soil_moisture_4_values, colored, "Soil Moisture Sensor 4");

  refresh();

  // Create socket
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    endwin();
    perror("socket creation failed");
    return 1;
  }

  // Bind to port
  sockaddr_in servaddr{};
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = INADDR_ANY;
  servaddr.sin_port = htons(PORT);

  if (bind(sockfd, (const sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    endwin();
    perror("bind failed");
    close(sockfd);
    return 1;
  }

  // Loop to receive and display messages
  while (true) {
    socklen_t len = sizeof(servaddr);
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0, (sockaddr *)&servaddr, &len);

    if (n > 0) {
      buffer[n] = '\0';

      // Debug statement
      time_t my_time = time(NULL);
      #ifdef DEBUG
      if (colored) { wattron(log_window, COLOR_PAIR(TEXT_NORMAL_PAIR)); }
      wprintw(log_window, "Received: %s %s", buffer, ctime(&my_time));
      if (colored) { wattroff(log_window, COLOR_PAIR(TEXT_NORMAL_PAIR)); }
      #endif

      try {
        json parsed = json::parse(buffer);
        if (parsed.contains("type") && parsed["type"] == "light") {
          if (parsed["data"].contains("lux")) {
            int lux_value = parsed["data"]["lux"];
            light_sensor_values.insert(light_sensor_values.begin(), lux_value);

            // If the light values vector is too big, cut off the end.
            if (light_sensor_values.size() > (unsigned int)console_max_x)
              light_sensor_values.erase(light_sensor_values.end());
          }
        } else if (parsed.contains("type") && parsed["type"] == "status") {
          if (parsed["data"].contains("message")) {
            if (colored) { wattron(log_window, COLOR_PAIR(TEXT_MESSAGE_PAIR)); }
            std::string status_message = parsed["data"]["message"].get<std::string>();
            wprintw(log_window, "%s, %s", status_message.c_str(), ctime(&my_time));
            if (colored) { wattroff(log_window, COLOR_PAIR(TEXT_MESSAGE_PAIR)); }
          }
        } else if (parsed.contains("type") && parsed["type"] == "soilMoisture") {
          if (parsed["data"].contains("units") && parsed["data"].contains("sensorNum")) {
            int sm_value = parsed["data"]["units"];
            int sm_number = parsed["data"]["sensorNum"];
            switch (sm_number) {
            case 1:
              soil_moisture_1_values.insert(soil_moisture_1_values.begin(),
                                            sm_value);
              if (soil_moisture_1_values.size() >
                  (unsigned int)console_max_x / 2)
                soil_moisture_1_values.erase(soil_moisture_1_values.end());
              break;
            case 2:
              soil_moisture_2_values.insert(soil_moisture_2_values.begin(),
                                            sm_value);
              if (soil_moisture_2_values.size() >
                  (unsigned int)console_max_x / 2)
                soil_moisture_2_values.erase(soil_moisture_2_values.end());
              break;
            case 3:
              soil_moisture_3_values.insert(soil_moisture_3_values.begin(),
                                            sm_value);
              if (soil_moisture_3_values.size() >
                  (unsigned int)console_max_x / 2)
                soil_moisture_3_values.erase(soil_moisture_3_values.end());
              break;
            case 4:
              soil_moisture_4_values.insert(soil_moisture_4_values.begin(),
                                            sm_value);
              if (soil_moisture_4_values.size() >
                  (unsigned int)console_max_x / 2)
                soil_moisture_4_values.erase(soil_moisture_4_values.end());
              break;
            }
          }
        }
      } catch (...) {
        if (colored) { wattron(log_window, COLOR_PAIR(TEXT_ERROR_PAIR)); }
        wprintw(log_window, "Malformed Input.");
        if (colored) { wattroff(log_window, COLOR_PAIR(TEXT_ERROR_PAIR)); }
        // Ignore malformed input
      }

      wrefresh(log_window);
      draw_chart(light_sensor_win, light_sensor_values, colored, "Light Sensor 1");
      draw_chart(soil_moisture_sensor_1_win, soil_moisture_1_values, colored, "Soil Moisture Sensor 1");
      draw_chart(soil_moisture_sensor_2_win, soil_moisture_2_values, colored, "Soil Moisture Sensor 2");
      draw_chart(soil_moisture_sensor_3_win, soil_moisture_3_values, colored, "Soil Moisture Sensor 3");
      draw_chart(soil_moisture_sensor_4_win, soil_moisture_4_values, colored, "Soil Moisture Sensor 4");
    }

    refresh();

  }
  // Cleanup
  endwin();
  close(sockfd);
  return 0;
}

