namespace std.time
{	
	public
	{
		func nowMilliseconds()->u32
		{
			ticks := nowTicks() * 1000
			ticks /= ticksFrequency()
			return acast ticks
		}
        
		func nowMicroseconds()->u32
		{
			ticks := nowTicks() * 1000000
			ticks /= ticksFrequency()
			return acast ticks
		}        
	}
}
