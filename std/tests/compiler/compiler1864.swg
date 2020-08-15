#module compiler1864
#unittest error

public enum RGBRGB
{
    R = 10
    G
    B
}

impl RGBRGB
{
    var GLOBAL = 0
    public func isRed(self)->bool
    {
        GLOBAL += cast(s32) self
        return self == R
    }
}

#test
{
    @assert(RGBRGB.isRed())
}