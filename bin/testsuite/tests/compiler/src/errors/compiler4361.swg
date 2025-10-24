#global testerror "toto"

struct(T) Point
        where
        {
            if T != bool do
                @compilererror("toto", #curlocation)
            return true
        }
{
}

var G: Point's32
