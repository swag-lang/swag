#module "compiler1864"
#unittest error

enum RGBRGB
{
    R = 10
    G
    B
}

impl RGBRGB
{
    var GLOBAL = 0
    func isRed(self)->bool
    {
        GLOBAL += cast(s32) self
        return self == R
    }
}

#test
{
    @assert(RGBRGB.isRed())
}