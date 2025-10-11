enum RGBRGB
{
    R = 10
    G
    B
}

var GLOBAL = 0

impl enum RGBRGB
{
    func isRed(me)->bool
    {
        GLOBAL += cast(s32) me
        return me == R
    }
}

#test
{
    let xyz = RGBRGB.R
    @assert(xyz.isRed())
    //@assert(RGBRGB.isRed())
    @assert(RGBRGB.R.isRed())
    @assert(GLOBAL == 20)
}
