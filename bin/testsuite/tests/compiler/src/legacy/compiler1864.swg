#global testerror "Err0324"

enum RGBRGB
{
    R = 10
    G
    B
}

impl RGBRGB
{
    var GLOBAL = 0
    func isRed(me)->bool
    {
        GLOBAL += cast(s32) me
        return me == R
    }
}

#test
{
    @assert(RGBRGB.isRed())
}