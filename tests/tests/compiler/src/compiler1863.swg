enum RGBRGB
{
    R = 10
    G
    B
}

var GLOBAL = 0

impl enum RGBRGB
{
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
    @assert(GLOBAL == 20)
}