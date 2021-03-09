
using core.file

//#[swag.printbc]
#test
{
    {
        v1 := "c:/"
        v2 := "toto"
        res := path.combine(v1, v2)
        @assert(res == "c:/toto")
        res = path.combine(res.toString(), "titi")
        @assert(res == "c:/toto/titi")
    }
}