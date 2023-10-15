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

#define WIDTH  1000
#define HEIGHT 500
#define HSIZE  20
#define TSIZE  16

typedef void (*Contents)();

typedef struct Timer {
  double start_time;
  double duration;
} Timer;

typedef struct Card {
  int x1;
  int y1;
  int x2;
  int y2;
  bool minimized;
  bool moving;
  Contents contents;
  char *title;
} Card;

Card disk = {
  .x1 = 460,
  .y1 = 20,
  .x2 = 460 + 200,
  .y2 = 20 + 200,
  .minimized = true,
  .moving = false,
  .contents = disk_info,
  .title = "DISK",
};
Card cpu = {
  .x1 = 20,
  .y1 = 20,
  .x2 = 20 + 200,
  .y2 = 20 + 200,
  .minimized = false,
  .moving = false,
  .contents = cpu_info,
  .title = "CPU",
};
Card mem = {
  .x1 = 240,
  .y1 = 20,
  .x2 = 240 + 200,
  .y2 = 20 + 200,
  .minimized = false,
  .moving = false,
  .contents = mem_info,
  .title = "MEMORY",
};

const int cpu_freq_graph_len = 20; 
float cpu_freq_graph[cpu_freq_graph_len] = {0};

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
  Card *cards[] = {&cpu, &mem, &disk};
  int cards_len = 3;
  InitWindow(WIDTH, HEIGHT, "susmon");

  disk.contents = disk_info;

  Timer graph_refresh = {.start_time = GetTime(), .duration = 5};


  Vector2 mouse = { -100.0f, -100.0f };

  while (!WindowShouldClose()) {
    mouse = GetMousePosition();

    if (cpu.moving) {
      cpu.x1 = mouse.x;
      cpu.y1 = mouse.y;
      cpu.x2 = cpu.x1 + 200;
      cpu.y2 = cpu.y1 + 200;
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        cpu.moving = false;
    }
    else if (mem.moving) {
      mem.x1 = mouse.x;
      mem.y1 = mouse.y;
      mem.x2 = mem.x1 + 200;
      mem.y2 = mem.y1 + 200;
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        mem.moving = false;
    }
    else if (disk.moving) {
      disk.x1 = mouse.x;
      disk.y1 = mouse.y;
      disk.x2 = disk.x1 + 200;
      disk.y2 = disk.y1 + 200;
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        disk.moving = false;
    }

    if (mouse.x >= disk.x2 - 20 && mouse.x <= disk.x2 && mouse.y >= disk.y1 - 20 && mouse.y <= disk.y1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      disk.minimized = !disk.minimized;
    else if (mouse.x >= mem.x2 - 20 && mouse.x <= mem.x2 && mouse.y >= mem.y1 - 20 && mouse.y <= mem.y1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      mem.minimized = !mem.minimized;
    else if (mouse.x >= cpu.x2 - 20 && mouse.x <= cpu.x2 && mouse.y >= cpu.y1 - 20 && mouse.y <= cpu.y1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      cpu.minimized = !cpu.minimized;

    else if (mouse.x >= cpu.x1 && mouse.x <= cpu.x2 && mouse.y >= cpu.y1 - 20 && mouse.y <= cpu.y1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      cpu.moving = true;
    else if (mouse.x >= mem.x1 && mouse.x <= mem.x2 && mouse.y >= mem.y1 - 20 && mouse.y <= mem.y1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      mem.moving = true;
    else if (mouse.x >= disk.x1 && mouse.x <= disk.x2 && mouse.y >= disk.y1 - 20 && mouse.y <= disk.y1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      disk.moving = true;

    if (GetTime() - graph_refresh.start_time > graph_refresh.duration) {
      cpu_freq_graph[cpu_freq_graph_len - 1] = sus_cpu_freq();

      for (int i = 0; i < cpu_freq_graph_len - 1; ++i) {
        cpu_freq_graph[i] = cpu_freq_graph[i + 1];
        graph_refresh.start_time = GetTime();
      }
    }

    BeginDrawing();
    
    ClearBackground(BLACK);


    // CPU INFO
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
