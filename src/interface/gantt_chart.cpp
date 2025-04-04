#include <ncurses.h>
#include <string>
#include <vector>

void draw_gantt_chart(WINDOW *win, const std::vector<std::string>& processes, int start_time, int end_time) {
    int x = 1; // Starting position for time labels

    // Draw the time labels (at the top of the chart)
    for (int t = start_time; t <= end_time; ++t) {
        mvwprintw(win, 0, x, "%3d", t); // Print time (formatted to take 3 characters)
        x += 6; // Move to the next column for the next time unit (space for process blocks)
    }

    // Draw the process names on the left side of the chart
    int y = 1; // Starting position for process names
    for (const auto& process : processes) {
        mvwprintw(win, y, 0, "%s", process.c_str()); // Print process name
        y++; // Move to the next row for the next process
    }

    wrefresh(win); // Refresh the window to show the updates
}

int main() {
    initscr();              // Initialize ncurses
    noecho();               // Don't show typed characters
    cbreak();               // Disable line buffering (allow immediate input)
    curs_set(0);            // Hide the cursor

    int height = 10, width = 40, start_y = 0, start_x = 0;
    WINDOW *gantt_win = newwin(height, width, start_y, start_x); // Create a window for the Gantt chart

    // Test data (example with 3 processes)
    std::vector<std::string> processes = {"P1", "P2", "P3"};
    int start_time = 0, end_time = 10;

    // Draw the Gantt chart
    draw_gantt_chart(gantt_win, processes, start_time, end_time);

    getch(); // Wait for user input before closing the window

    endwin(); // Clean up ncurses
    return 0;
}
