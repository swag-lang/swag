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
    #run
    {
        @target().backendC.writeSourceCode = true
        @target().backendC.writeByteCodeInstruction = true
    }    
}
#else
{
	#assert(false, "std invalid configuration")
}
