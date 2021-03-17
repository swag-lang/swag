const TARGET = "debug"

#if TARGET == "debug"
{
	const DEBUG = true
}
#elif TARGET == "release"
{
	const DEBUG = false
}
#else
{
	//#assert(false, "invalid target")
}

#if DEBUG
{
	func toto()
	{
	}
}
#else
{
	func toto()
	{
		@assert(false)
	}
}

#test
{
	toto()
}