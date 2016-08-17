/* all logic functions are defined in this file.  They set global values for the state, and track timing
also in global variables of when the state last changed, as well as time between low states and high states.
*/
#include <main.c>

void becameTrue(int ch)
{
  out[ch] = TRUE;  // set output high (mainloop should set output?)
  digitalHigh(LogicOutPin[ch]);
  prevTimeChanged[ch] = timeChanged[ch]; // store former time
  prevTimeTrue[ch] = timeTrue[ch]; // store former time
  timeChanged[ch] = micros();  // should check for overflow condition somewhere??
  timeTrue[ch] = timeChanged[ch]; // set to current time
  stateChanged[ch] = TRUE;  //any functions that rely on change of state should set this flag to false when completed.
}

void becameFalse(int ch)
{
  out[ch] = FALSE;
  digitalLow(LogicOutPin[ch]);
  prevTimeChanged[ch] = timeChanged[ch]; // store former time
  prevTimeFalse[ch] = timeFalse[ch]; // store former time
  timeChanged[ch] = micros();  // should check for overflow condition somewhere??
  timeFalse[ch] = timeChanged[ch]; // set to current time
  stateChanged[ch] = TRUE;  //any functions that rely on change of state should set this flag to false when completed.
}

void threeinAND(int ch)
{
  if (logicIn1 && logicIn2 && logicIn3) // logical AND
  {
    if (out[ch] == FALSE)
    {
      becameTrue(ch);
    }
  }
  else
  {
    if (out[ch] == TRUE)
    {
      becameFalse(ch);
    }
  }
}


void threeinOR(int ch)
{
  if (logicIn1 || logicIn2 || logicIn3) // logical OR
  {
    if (out[ch] == FALSE)
    {
      becameTrue(ch);
    }
  }
  else
  {
    if (out[ch] == TRUE)
    {
      becameFalse(ch);
    }
  }
}

// inhibit here means "prevent changing state"
void twoinORinhibit(int ch)
{
  if (!logicIn3)
  {
    threeinOR(ch);
  }
}

void majority(int ch)
{
  if ((logicIn1 || logicIn2) && (logicIn2 || logicIn3)) // majority
  {
    if (out[ch] == FALSE)
    {
      becameTrue(ch);
    }
  }
  else
  {
    if (out[ch] == TRUE)
    {
      becameFalse(ch);
    }
  }
}


void twoofthree(int ch)
{
  if ((logicIn1 && logicIn2 && !logicIn3) ||
      (logicIn2 && logicIn3 && !logicIn1) ||
      (logicIn1 && logicIn3 && !logicIn2))  // 2 of 3
  {
    if (out[ch] == FALSE)
    {
      becameTrue(ch);
    }
  }
  else
  {
    if (out[ch] == TRUE)
    {
      becameFalse(ch);
    }
  }
}


void isodd(int ch)
{
  if (((logicIn1 && !logicIn2 && !logicIn3) ||
      (logicIn2 && !logicIn3 && !logicIn1) ||
      (logicIn3 && !logicIn1 && !logicIn2)) ||
      (logicIn1 && logicIn2 && logicIn3)) // odd number true
  {
    if (out[ch] == FALSE)
    {
      becameTrue(ch);
    }
  }
  else
  {
    if (out[ch] == TRUE)
    {
      becameFalse(ch);
    }
  }
}


void threeinXOR(int ch)
{
  if ((logicIn1 && !logicIn2 && !logicIn3) ||
      (logicIn2 && !logicIn1 && !logicIn3) ||
      (logicIn3 && !logicIn1 && !logicIn2))
  {
    if (out[ch] == FALSE)
    {
      becameTrue(ch);
    }
  }
  else
  {
    if (out[ch] == TRUE)
    {
      becameFalse(ch);
    }
  }
}

void twoinXOR_inhibit(int ch)
{
  if (!logicIn3)
  {
    threeinXOR(ch);
  }
}

/* SR latch behavior:

gated - Gate turns output low while latched, continues to operate in background
inhibited - state is maintained, but can be neither set or reset

*/

void SRlatch_gated(int ch)
{
  if (logicIn1 == TRUE && out[ch] == FALSE)
  {
    if (logicIn3 == FALSE)
    {
    becameTrue(ch);
    isLatched[ch] = TRUE;
    }
    else
    {

    }
  }
  else if (out[ch] == TRUE && isLatched[ch] == TRUE)
  {
    becameFalse(ch);
    isLatched[ch] = FALSE;
  }
  else
  {
    if ()
  }
}

void SRlatch_inhibit(int ch)
{
  if (!logicIn3)
  {
    if (logicIn1 = TRUE)
    {
      if (out[ch] == FALSE)
      {
      becameTrue(ch);
      isLatched[ch] = TRUE;
      }
    }
    else
    {
      if (out[ch] == TRUE)
      {
        becameFalse(ch);
        isLatched[ch] = FALSE;
      }
    }
  }
}

void SRlatch_background(int ch)
{
  if (logicIn1 = TRUE)
  {
    if (out[ch] == FALSE)
    {
      becameTrue(ch);
    }
  }
  else
  {
    if (out[ch] == TRUE)
    {
      if(logicIn3 == TRUE)  // if inhibit we don't want to unlatch... but what about timeFalse/timeChanged?
      {
        becameTrue(ch);
      }
      else
      {
        becameFalse(ch);
      }
    }
  }
}


void flipflop(int ch)
{
  out[ch] = !out[ch];
  stateChanged[ch] = TRUE;  //any functions that rely on change of state should set this flag to false when completed.
  prevTimeChanged[ch] = timeChanged[ch]; // store former time
  timeChanged[ch] = micros();  // should check for overflow condition somewhere??

  if (out[ch] == TRUE)
  {
    prevTimeTrue[ch] = timeTrue[ch]; // store former time
    timeTrue[ch] = timeChanged[ch]
  }
  else
  {
    prevTimeFalse[ch] = timeFalse[ch]; // store former time
    timeFalse[ch] = timeChanged[ch]
  }
}

void flipflopinhibit(int ch)
{
    if (!logicIn3)
    {
      flipflop(ch);
    }
}


SR ping
+/- trig/ping


gated latch srg
inhibited latch srt
double set latch ssr
double reset latch srr


how should latch behave?  transparently operating behind (gated) or inhibit resets and prevents latching while high
latch timing, plus env trigger
ping
end.env

ping 2 env
RISING/FALLING
