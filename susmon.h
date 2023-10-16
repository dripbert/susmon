typedef void (*Contents)();
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

void disk_info();
void cpu_info();
void mem_info();

