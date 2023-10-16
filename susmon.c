#include "raylib.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include "susmon.h"

#ifdef __FreeBSD__
#include <sys/sysctl.h>
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


#define cpu_freq_graph_len 20
float cpu_freq_graph[cpu_freq_graph_len] = {0.0f};

void disk_info() {
  DrawRectangle(disk.x1, disk.y1, disk.x2 - disk.x1, disk.y2 - disk.y1, DARKGRAY);
  DrawText(TextFormat("Disk: %.1fGig/%.1f Gig", sus_disk_total(), sus_disk_total()), disk.x1 + 10, disk.y1 + 30, TSIZE, LIGHTGRAY);
}
void cpu_info() {
  DrawRectangle(cpu.x1, cpu.y1, cpu.x2 - cpu.x1, cpu.y2 - cpu.y1, DARKGRAY);
  DrawText(TextFormat("CPUs: %d", sysconf(_SC_NPROCESSORS_ONLN)), cpu.x1 + 10, cpu.y1 + 30, TSIZE, LIGHTGRAY);
  DrawText(TextFormat("CPU perc: %d%%", sus_cpu_perc()), cpu.x1 + 10, cpu.y1 + 46, TSIZE, LIGHTGRAY);
  DrawText(TextFormat("CPU freq: %.1f Ghz", sus_cpu_freq()), cpu.x1 + 10, cpu.y1 + 62, TSIZE, LIGHTGRAY);

  int h;
  for (int i = 0; i < cpu_freq_graph_len; ++i) {
    h = cpu_freq_graph[i] * 10;
    DrawRectangle(cpu.x1 + (10 * i), cpu.y2 - h, 5, h, RED);
  }
}
void mem_info() {
  DrawRectangle(mem.x1, mem.y1, mem.x2 - mem.x1, mem.y2 - mem.y1, DARKGRAY);
  DrawText(TextFormat("Mem: %.1fGig/%.1f Gig", sus_mem_used(), sus_mem_phys()), mem.x1 + 10, mem.y1 + 30, TSIZE, LIGHTGRAY);
}

int main(void)
{
  int cards_len = sizeof(cards)/sizeof(Card*);
  InitWindow(WIDTH, HEIGHT, "susmon");

  disk.contents = disk_info;

  Timer graph_refresh = {.start_time = GetTime(), .duration = 5};
  Vector2 mouse = { -100.0f, -100.0f };

  while (!WindowShouldClose()) {
    mouse = GetMousePosition();

    for (int i = 0; i < cards_len; ++i) {
      Card *c = cards[i];
      if (c->moving) {
        c->x1 = mouse.x;
        c->y1 = mouse.y;
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
        c->moving = true;
        break;
      }
    }

    if (GetTime() - graph_refresh.start_time > graph_refresh.duration) {
      cpu_freq_graph[cpu_freq_graph_len - 1] = sus_cpu_freq();

      for (int i = 0; i < cpu_freq_graph_len - 1; ++i) {
        cpu_freq_graph[i] = cpu_freq_graph[i + 1];
        graph_refresh.start_time = GetTime();
      }
    }

    BeginDrawing();
    
    ClearBackground(BLACK);

    for (int i = 0; i < cards_len; ++i){
      Card c = *cards[i];
      DrawRectangle(c.x1, c.y1 - 20, c.x2 - c.x1, 20, ORANGE);
      DrawRectangle(c.x2 - 20, c.y1 - 20, 20, 20, RED);
      DrawText(c.title, c.x1, c.y1 - 20, HSIZE, BLACK);
      if(!c.minimized){
        c.contents();
      }
    }

    // DrawLine(0, 240, WIDTH, 240, WHITE);

    WaitTime(0.1);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
