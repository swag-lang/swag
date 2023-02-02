#global testerror

struct(T) Point
    #selectif
    {
        if T != bool
            @compilererror("toto", #location)
        return true
    }
{
}

var G: Point's32