#include "raylib.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef __FreeBSD__
#include <sys/sysctl.h>
#endif

#include "./components.c"

#define WIDTH  1000
#define HEIGHT 500
#define HSIZE  20
#define TSIZE  16

typedef struct Timer {
  double start_time;
  double duration;
} Timer;

typedef struct Canvas {
  int x1;
  int y1;
  int x2;
  int y2;
} Canvas;

int main(void)
{
  InitWindow(WIDTH, HEIGHT, "susmon");

  Canvas cpu  = { .x1 = 20,  .y1 = 20, .x2 = 20 + 200,  .y2 = 20 + 200 };
  Canvas mem  = { .x1 = 240, .y1 = 20, .x2 = 240 + 200, .y2 = 20 + 200 };
  Canvas disk = { .x1 = 460, .y1 = 20, .x2 = 460 + 200, .y2 = 20 + 200 };

  Timer graph_refresh = {.start_time = GetTime(), .duration = 5};

  const int cpu_freq_graph_len = 20; 
  float cpu_freq_graph[cpu_freq_graph_len] = {0};

  Vector2 mouse = { -100.0f, -100.0f };

  enum Moving {Cpu, Mem, Disk, None}; 
  enum Moving mov = None;
  
  while (!WindowShouldClose()) {
    mouse = GetMousePosition();

    switch (mov) {
    case Cpu:
      cpu.x1 = mouse.x;
      cpu.y1 = mouse.y;
      cpu.x2 = cpu.x1 + 200;
      cpu.y2 = cpu.y1 + 200;
      break;
    case Mem:
      mem.x1 = mouse.x;
      mem.y1 = mouse.y;
      mem.x2 = mem.x1 + 200;
      mem.y2 = mem.y1 + 200;
      break;
    case Disk:
      disk.x1 = mouse.x;
      disk.y1 = mouse.y;
      disk.x2 = disk.x1 + 200;
      disk.y2 = disk.y1 + 200;
      break;
    }

    if (mouse.x >= cpu.x1 && mouse.x <= cpu.x2 && mouse.y >= cpu.y1 && mouse.y <= cpu.y2 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      mov = Cpu;
    else if (mouse.x >= mem.x1 && mouse.x <= mem.x2 && mouse.y >= mem.y1 && mouse.y <= mem.y2 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      mov = Mem;
    else if (mouse.x >= disk.x1 && mouse.x <= disk.x2 && mouse.y >= disk.y1 && mouse.y <= disk.y2 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
      mov = Disk;
    else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
      mov = None;
     
    BeginDrawing();
    
    ClearBackground(BLACK);

    DrawRectangle(cpu.x1, cpu.y1, cpu.x2 - cpu.x1, cpu.y2 - cpu.y1, DARKGRAY);
    DrawRectangle(mem.x1, mem.y1, mem.x2 - mem.x1, mem.y2 - mem.y1, DARKGRAY);
    DrawRectangle(disk.x1, disk.y1, disk.x2 - disk.x1, disk.y2 - disk.y1, DARKGRAY);

    // CPU INFO
    DrawRectangle(cpu.x1, cpu.y1, cpu.x2 - cpu.x1, 20, BLUE);
    DrawText("CPU:", cpu.x1, cpu.y1, HSIZE, BLACK);
    DrawText(TextFormat("CPUs: %d", sysconf(_SC_NPROCESSORS_ONLN)), cpu.x1 + 10, cpu.y1 + 30, TSIZE, LIGHTGRAY);
    DrawText(TextFormat("CPU perc: %d%%", sus_cpu_perc()), cpu.x1 + 10, cpu.y1 + 46, TSIZE, LIGHTGRAY);
    DrawText(TextFormat("CPU freq: %.1f Ghz", sus_cpu_freq()), cpu.x1 + 10, cpu.y1 + 62, TSIZE, LIGHTGRAY);

    // MEMORY INFO
    DrawRectangle(mem.x1, mem.y1, mem.x2 - mem.x1, 20, GREEN);
    DrawText("MEMORY:", mem.x1, mem.y1, HSIZE, BLACK);
    DrawText(TextFormat("Mem: %.1fGig/%.1f Gig", sus_mem_used(), sus_mem_phys()), mem.x1 + 10, mem.y1 + 30, TSIZE, LIGHTGRAY);

    // DISK INFO
    DrawRectangle(disk.x1, disk.y1, disk.x2 - disk.x1, 20, YELLOW);
    DrawText("DISK:", disk.x1, disk.y1, HSIZE, BLACK);
    DrawText(TextFormat("Disk: %.1fGig/%.1f Gig", sus_disk_total(), sus_disk_total()), disk.x1 + 10, disk.y1 + 30, TSIZE, LIGHTGRAY);


    if (GetTime() - graph_refresh.start_time > graph_refresh.duration) {
      cpu_freq_graph[cpu_freq_graph_len - 1] = sus_cpu_freq();

      for (int j = 0; j < cpu_freq_graph_len - 1; ++j) {
        cpu_freq_graph[j] = cpu_freq_graph[j + 1];
        graph_refresh.start_time = GetTime();
      }
    }

    int h;
    for (int i = 0; i < cpu_freq_graph_len; ++i) {
      h = cpu_freq_graph[i] * 10;
      DrawRectangle(cpu.x1 + (10 * i), cpu.y2 - h, 5, h, RED);
    }

    // DrawLine(0, 240, WIDTH, 240, WHITE);

    WaitTime(0.1);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
