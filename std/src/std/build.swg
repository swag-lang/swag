// Platform setup
#if #arch == "win64"
{
	const WINDOWS = true
	const BIG_ENDIAN = false
}
#else
{
	#assert(false, "std invalid platform")
}

// Configuration setup
#if #config == "debug"
{
	const DEBUG = true
	const ASSERT = true
}
#elif #config == "release"
{
	const DEBUG = false
	const ASSERT = false
}
#else
{
	#assert(false, "std invalid configuration")
}