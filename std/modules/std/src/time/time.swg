public namespace std.time;

// Returns the current time expressed in milliseconds
func nowMilliseconds()->u32
{
    ticks := nowTicks() * 1000
    ticks /= ticksFrequency()
    return acast ticks
}

// Returns the current time expressed in microseconds
func nowMicroseconds()->u32
{
    ticks := nowTicks() * 1000000
    ticks /= ticksFrequency()
    return acast ticks
}
