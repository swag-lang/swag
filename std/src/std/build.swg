// Platform setup
#if #platform == "win64"
{
	const WINDOWS = true
	const BIG_ENDIAN = false
}
#else
{
	#assert(false, "std invalid platform")
}

// Configuration setup
#if #configuration == "debug"
{
	const DEBUG = true
	const ASSERT = true
}
#elif #configuration == "release"
{
	const DEBUG = false
	const ASSERT = false
}
#else
{
	#assert(false, "std invalid configuration")
}