#if swag.WINDOWS
{
	#import "win32"
	#import "xinput"
}

namespace std
{
    #if #cfg != "final"
    {
        const ASSERT = true
    }
    #else
    {
        const ASSERT = false
    }
}