#global testerror "toto"

struct(T) Point
    #validif
    {
        if T != bool do
            @compilererror("toto", #location)
        return true
    }
{
}

var G: Point's32