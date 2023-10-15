int sus_cpu_perc()
{
  return -1;
}

float sus_cpu_freq()
{
  int freq = -1;
  #ifdef __FreeBSD__
  size_t size = sizeof(freq);
  if (sysctlbyname("dev.cpu.0.freq", &freq, &size, NULL, 0) < 0) {
    fprintf(stderr, "ERROR: Failed to get cpu frequency!\n");
    return -1;
  }
  #endif

  return (float)freq/1000;
}

float sus_mem_phys()
{
  long bytes = -1;
  #ifdef __FreeBSD__
  size_t size = sizeof(bytes);
  if (sysctlbyname("hw.physmem", &bytes, &size, NULL, 0) < 0) {
    fprintf(stderr, "ERROR: Failed to get physical memory!\n");
    return -1;
  }
  #endif

  return (float)bytes/1.0e9;
}
float sus_mem_used()
{
  long bytes = -1;
  #ifdef __FreeBSD__
  size_t size = sizeof(bytes);
  if (sysctlbyname("hw.usermem", &bytes, &size, NULL, 0) < 0) {
    fprintf(stderr, "ERROR: Failed to get user memory!\n");
    return -1;
  }
  #endif

  return (sus_mem_phys() - ((float)bytes)/1.0e9);
}

int sus_disk_total ()
{
  return - 1;
}
