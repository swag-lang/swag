namespace std.time
{
	public
	{
		func nowMs()->u32
		{
			ticks := nowTicks() * 1000
			ticks /= ticksFrequency()
			return acast ticks
		}
	}
}
