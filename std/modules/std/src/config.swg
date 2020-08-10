#if std.WINDOWS
{
	#import "win32"
	#import "xinput"
}

namespace std
{
    // Setup
    #if #arch == "x64"
    {
        public const BIG_ENDIAN = false
    }
    #else
    {
        #assert(false, "invalid arch:" ~ #arch)
    }

    // Platform setup
    #if #os == "windows"
    {
        public const WINDOWS = true
    }
    #else
    {
        #assert(false, "invalid os:" ~ #os)
    }

    #if #cfg != "final"
    {
        public const ASSERT = true
    }
    #else
    {
        public const ASSERT = false
    }
}