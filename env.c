#include <main.c>

void envcalc(int ch)
{
  uint32_t envlength = timeofstatechange[ch]-prevtimeofstatechange[ch];
  uint32_t breakpoint = (float(envParam) / 1024) * envlength
  uint32_t decaytime = envlength - breakpoint;
  riseTime[ch] = breakpoint << 8; // bitshift division to get 256 steps in microseconds
  fallTime[ch] = decaytime << 8; // bitshift division to get 256 steps in microseconds
  envNeedsCalc = FALSE;
}
