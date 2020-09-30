enum RGBRGB
{
    R = 10
    G
    B
}

impl enum RGBRGB
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
    var xyz = RGBRGB.R
    @assert(xyz.isRed())
    //@assert(RGBRGB.isRed())
    @assert(RGBRGB.R.isRed())
    @assert(RGBRGB.GLOBAL == 20)
}