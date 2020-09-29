namespace std.time;

// Returns the current time expressed in milliseconds
public func nowMilliseconds()->u32
{
    ticks := nowTicks() * 1000
    ticks /= ticksFrequency()
    return acast ticks
}

// Returns the current time expressed in microseconds
public func nowMicroseconds()->u32
{
    ticks := nowTicks() * 1000000
    ticks /= ticksFrequency()
    return acast ticks
}
