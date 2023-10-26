#include "raylib.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include "susmon.h"
#include "config.h"

#ifdef __FreeBSD__
#include <sys/sysctl.h>
#include <devstat.h>
#endif

#include "./components.c"
#include "./cards.h"

#define WIDTH  1000
#define HEIGHT 500
#define HSIZE  20
#define TSIZE  16

typedef struct Timer {
  double start_time;
  double duration;
} Timer;

#define CPU_GRAPH_LEN 20
float cpu_perc_graph[CPU_GRAPH_LEN] = {0.0f};
uint8_t cpu_perc;

void draw_card_text(const char *text, int x, int y, int offset, int m) {
  DrawTextEx(font_small, text, (Vector2){x, y + (offset * m)}, (float)font_small.baseSize, 2, RAYWHITE);
}

void disk_info() {
  DrawRectangle(disk.x1, disk.y1, disk.x2 - disk.x1, disk.y2 - disk.y1, DARKGRAY);
  draw_card_text(TextFormat("Disk: %.1fGig/%.1f Gig", 0.0f, sus_disk_total()), disk.x1 + 10, disk.y1 + 10, 16, 0);
}
void cpu_info() {
  DrawRectangle(cpu.x1, cpu.y1, cpu.x2 - cpu.x1, cpu.y2 - cpu.y1, DARKGRAY);
  draw_card_text(TextFormat("CPUs: %d", sysconf(_SC_NPROCESSORS_ONLN)), cpu.x1 + 10, cpu.y1 + 10, 16, 0);
  draw_card_text(TextFormat("CPU perc: %d%%", cpu_perc), cpu.x1 + 10, cpu.y1 + 10, 16, 1);
  draw_card_text(TextFormat("CPU freq: %.1f Ghz", sus_cpu_freq()), cpu.x1 + 10, cpu.y1 + 10, 16, 2);
  draw_card_text(TextFormat("CPU temp: %.1f°C", sus_cpu_temp()), cpu.x1 + 10, cpu.y1 + 10, 16, 3);
  DrawLine(cpu.x1, cpu.y2 - 105, cpu.x2, cpu.y2 - 105, LIGHTGRAY);

  int h;
  for (int i = 0; i < CPU_GRAPH_LEN; ++i) {
    h = cpu_perc_graph[i] + 5;
    DrawRectangle(cpu.x1 + (10 * i), cpu.y2 - h, 5, h, RED);
  }
}
void mem_info() {
  DrawRectangle(mem.x1, mem.y1, mem.x2 - mem.x1, mem.y2 - mem.y1, DARKGRAY);
  draw_card_text(TextFormat("Mem: %.1fGig/%.1f Gig", sus_mem_used(), sus_mem_phys()), mem.x1 + 10, mem.y1 + 10, 16, 0);
}

int main(void)
{
  int cards_len = sizeof(cards)/sizeof(Card*);
  InitWindow(WIDTH, HEIGHT, "susmon");

  Timer graph_refresh = {.start_time = GetTime(), .duration = 5};
  Timer cpu_perc_refresh = {.start_time = GetTime(), .duration = 1};
  Vector2 mouse = { -100.0f, -100.0f };
  Font font_big = LoadFontEx(font_title, 20, 0, 256);
  font_small = LoadFontEx(font_cont, 16, 0, 256);

  int relative_mouse_position_x;
  int relative_mouse_position_y;

  while (!WindowShouldClose()) {
    clock_t start_time = clock();
    if (GetTime() - cpu_perc_refresh.start_time > cpu_perc_refresh.duration) {
      cpu_perc = sus_cpu_perc();
      cpu_perc_refresh.start_time = GetTime();
    }
    if (GetTime() - graph_refresh.start_time > graph_refresh.duration) {
      cpu_perc_graph[CPU_GRAPH_LEN - 1] = cpu_perc;

      for (int i = 0; i < CPU_GRAPH_LEN - 1; ++i) {
        cpu_perc_graph[i] = cpu_perc_graph[i + 1];
        graph_refresh.start_time = GetTime();
      }
    }

    mouse = GetMousePosition();

    for (int i = 0; i < cards_len; ++i) {
      Card *c = cards[i];
      if (c->moving) {
        c->x1 = mouse.x - relative_mouse_position_x;
        c->y1 = mouse.y - relative_mouse_position_y;
        c->x2 = c->x1 + 200;
        c->y2 = c->y1 + 200;
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
          c->moving = false;
        break;
      }
      if (mouse.x >= c->x2 - 20 && mouse.x <= c->x2 && mouse.y >= c->y1 - 20 && mouse.y <= c->y1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        c->minimized = !c->minimized;
        break;
      }
      if (mouse.x >= c->x1 && mouse.x <= c->x2 && mouse.y >= c->y1 - 20 && mouse.y <= c->y1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        relative_mouse_position_x = mouse.x - c->x1;
        relative_mouse_position_y = mouse.y - c->y1;
        c->moving = true;
        break;
      }
    }


    BeginDrawing();
    
    ClearBackground(BLACK);

    for (int i = 0; i < cards_len; ++i){
      Card c = *cards[i];
      DrawRectangle(c.x1 - 2, c.y1 - 20, c.x2 - c.x1 + 4, 20, ORANGE);
      if (c.minimized)
        DrawText("+", c.x2 - 20, c.y1 - 20, HSIZE, RED);
      else
        DrawText("-", c.x2 - 20, c.y1 - 20, HSIZE, RED);
      DrawTextEx(font_big, c.title, (Vector2){c.x1, c.y1 - 20}, (float)font_big.baseSize, 2, BLACK);
      if(!c.minimized){
        DrawRectangleLinesEx((Rectangle){c.x1 - 2, c.y1, c.x2 - c.x1 + 4, c.y2 - c.y1 + 2}, 2, ORANGE);
        c.contents();
      }
    }

    //DrawTextEx(djvb16, "hello", (Vector2){cpu.x1 + 10, cpu.y1 + 10}, (float)djvb16.baseSize, 2, LIGHTGRAY);

    clock_t end_time = clock();
    if (end_time - start_time < 0.03)
      WaitTime(0.03 - (end_time - start_time));
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
