#if std.WINDOWS
{
	#import "win32"
	#import "xinput"
}

namespace std
{
    //////////////// TO REMOVE ////////////////
    public func toto()
    {
        @print("toto")
    }
    public func toto1(x: s64)
    {
        @print(x)
    }
    public func toto2(x, y, z, w: s64)
    {
        @print(x)
        @print(y)
        @print(z)
        @print(w)
    }
    public func toto3(a, b, c, d, e, f: s8)
    {
        @print(cast(s64) a)
        @print(cast(s64) b)
        @print(cast(s64) c)
        @print(cast(s64) d)
        @print(cast(s64) e)
        @print(cast(s64) f)
    }
    public func toto4()->string
    {
        return "toto"
    }

    //////////////// TO REMOVE ////////////////

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