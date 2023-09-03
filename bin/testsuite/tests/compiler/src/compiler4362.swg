#global testerror "s32 is bad !"

struct(T) Point
    #validif
    {
        if T != bool
            @compilererror("s32 is bad !", @location(T))
        return true
    }
{
}

var G: Point's32