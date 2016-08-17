#include <main.c>

void envcalcManual(int ch)
{
  uint32_t envlength = timeofstatechange[ch]-prevtimeofstatechange[ch];
  uint32_t breakpoint = (float(envParam) / 256) * envlength
  uint32_t decaytime = envlength - breakpoint;
  riseTime[ch] = envParam[ch] << 8; // bitshift division to get 256 steps in microseconds - by 2?
  riseTime[ch+2] = envParam[ch] << 8; // bitshift division to get 256 steps in microseconds - by 2?
  fallTime[ch] = envParam[ch] << 8; // bitshift division to get 256 steps in microseconds
  fallTime[ch+2] = envParam[ch] << 8; // bitshift division to get 256 steps in microseconds
  envNeedsCalc = FALSE;
}

void envcalcTTFF(int ch)
{
  uint32_t envlength = timeofstatechange[ch]-prevtimeofstatechange[ch];
  uint32_t breakpoint = (float(envParam) / 256) * envlength
  uint32_t decaytime = envlength - breakpoint;
  riseTime[ch] = timeTrue - prevTimeTrue; // bitshift division to get 256 steps in microseconds - by 2?
  fallTime[ch] = timeTrue - prevTimeTrue; // bitshift division to get 256 steps in microseconds
  riseTime[ch+2] = timeFalse - prevTimeFalse; // bitshift division to get 256 steps in microseconds - by 2?
  fallTime[ch+2] = timeFalse - prevTimeFalse; // bitshift division to get 256 steps in microseconds
  envNeedsCalc = FALSE;
}


void envcalcTFFT(int ch)
{
  uint32_t envlength = timeofstatechange[ch]-prevtimeofstatechange[ch];
  uint32_t breakpoint = (float(envParam) / 256) * envlength
  uint32_t decaytime = envlength - breakpoint;
  riseTime[ch] = timeTrue - prevTimeTrue; // bitshift division to get 256 steps in microseconds - by 2?
  fallTime[ch] = timeFalse - prevTimeFalse; // bitshift division to get 256 steps in microseconds
  riseTime[ch+2] = timeFalse - prevTimeFalse; // bitshift division to get 256 steps in microseconds - by 2?
  fallTime[ch+2] = timeTrue - prevTimeTrue; // bitshift division to get 256 steps in microseconds
  envNeedsCalc = FALSE;
}

void envcalcFtFtTfTf(int ch)
{
  uint32_t envlength = timeofstatechange[ch]-prevtimeofstatechange[ch];
  uint32_t breakpoint = (float(envParam) / 256) * envlength
  uint32_t decaytime = envlength - breakpoint;
  riseTime[ch] = timeFalse - timeTrue; // bitshift division to get 256 steps in microseconds - by 2?
  fallTime[ch] = timeFalse - timeTrue; // bitshift division to get 256 steps in microseconds
  riseTime[ch+2] = timeTrue - prevTimeFalse; // bitshift division to get 256 steps in microseconds - by 2?
  fallTime[ch+2] = timeTrue - prevTimeFalse; // bitshift division to get 256 steps in microseconds
  envNeedsCalc = FALSE;
}

void envcalcTfFtFtTf(int ch)
{
  uint32_t envlength = timeofstatechange[ch]-prevtimeofstatechange[ch];
  uint32_t breakpoint = (float(envParam) / 256) * envlength
  uint32_t decaytime = envlength - breakpoint;
  riseTime[ch] = timeTrue - prevtimeFalse; // bitshift division to get 256 steps in microseconds - by 2?
  fallTime[ch] = timeFalse - timeTrue; // bitshift division to get 256 steps in microseconds
  riseTime[ch+2] = timeFalse - TimeTrue; // bitshift division to get 256 steps in microseconds - by 2?
  fallTime[ch+2] = timeTrue - prevTimeFalse; // bitshift division to get 256 steps in microseconds
  envNeedsCalc = FALSE;
}

void envcalcFtTfTfFt(int ch)
{
  uint32_t envlength = timeofstatechange[ch]-prevtimeofstatechange[ch];
  uint32_t breakpoint = (float(envParam) / 256) * envlength
  uint32_t decaytime = envlength - breakpoint;
  riseTime[ch] = timeFalse - TimeTrue; // bitshift division to get 256 steps in microseconds - by 2?
  fallTime[ch] = timeTrue - prevtimeFalse; // bitshift division to get 256 steps in microseconds
  riseTime[ch+2] = timeTrue - prevtimeFalse; // bitshift division to get 256 steps in microseconds - by 2?
  fallTime[ch+2] = timeFalse - TimeTrue; // bitshift division to get 256 steps in microseconds
  envNeedsCalc = FALSE;
}

void ad_oneshot_step(int ch)
{
  analogWrite()
}

void adOneshotSustain(int ch)

void adLooping(int ch)
oneshot
sustain
loop
quadrature
wavetable
