// Define your custom cards in this file

Card disk = {
  .x1 = 460,
  .y1 = 20,
  .x2 = 460 + 200,
  .y2 = 20 + 200,
  .minimized = false,
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

Card *cards[] = {&cpu, &mem, &disk};
