
#ifdef __FreeBSD__
int sus_cpu_perc()
{
  int perc = -1;

  static long t[CPUSTATES];
  long p[CPUSTATES], u, c;

  size_t size = sizeof(t);
  memcpy(p, t, sizeof(p));

  if (sysctlbyname("kern.cp_time", &t, &size, NULL, 0) < 0) {
    fprintf(stderr, "ERROR: Failed to get cpu frequency!\n");
    return -1;
  }
  u = (t[CP_USER] + t[CP_NICE] + t[CP_SYS]) -
      (p[CP_USER] + p[CP_NICE] + p[CP_SYS]); 
  c = (t[CP_USER] + t[CP_NICE] + t[CP_SYS] + t[CP_INTR] + t[CP_IDLE]) -
      (p[CP_USER] + p[CP_NICE] + p[CP_SYS] + p[CP_INTR] + p[CP_IDLE]);

  perc = 100*u/c;
  return perc;
}

float sus_cpu_temp()
{
  int temp = -1;
  size_t size = sizeof(temp);
  if (sysctlbyname("dev.cpu.0.temperature", &temp, &size, NULL, 0) < 0) {
    fprintf(stderr, "ERROR: Failed to get cpu temperature!\n");
    return -1;
  }

  return (float)temp/100;
}
float sus_cpu_freq()
{
  int freq = -1;
  size_t size = sizeof(freq);
  if (sysctlbyname("dev.cpu.0.freq", &freq, &size, NULL, 0) < 0) {
    fprintf(stderr, "ERROR: Failed to get cpu frequency!\n");
    return -1;
  }

  return (float)freq/1000;
}

float sus_mem_phys()
{
  long bytes = -1;
  size_t size = sizeof(bytes);
  if (sysctlbyname("hw.physmem", &bytes, &size, NULL, 0) < 0) {
    fprintf(stderr, "ERROR: Failed to get physical memory!\n");
    return -1;
  }

  return (float)bytes/1.0e9;
}
float sus_mem_used()
{
  long bytes = -1;
  size_t size = sizeof(bytes);
  if (sysctlbyname("hw.usermem", &bytes, &size, NULL, 0) < 0) {
    fprintf(stderr, "ERROR: Failed to get user memory!\n");
    return -1;
  }

  return (sus_mem_phys() - ((float)bytes)/1.0e9);
}

int sus_disk_total ()
{
  return 0;
}
#endif
