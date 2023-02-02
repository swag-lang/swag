#global testerror

struct(T) Point
    #selectif
    {
        if T != bool
            @compilererror("s32 is bad !", @location(T))
        return true
    }
{
}

var G: Point's32