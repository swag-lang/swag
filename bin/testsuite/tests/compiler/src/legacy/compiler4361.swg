#global testerror "toto"

struct(T) Point
        where
        {
            if T != bool:
                @compilererror("toto", #curlocation)
            return true
        }
{
}

var G: Point's32
