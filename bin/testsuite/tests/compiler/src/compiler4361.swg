#global testerror "toto"

struct(T) Point
    #validif
    {
        if T != bool
            @compilererror("toto", #location)
        return true
    }
{
}

var G: Point's32