
using Core.File

//#[Swag.printbc]
#test
{
    {
        v1 := "c:/"
        v2 := "toto"
        res := Path.combine(v1, v2)
        @assert(res == "c:/toto")
        res = Path.combine(res.toString(), "titi")
        @assert(res == "c:/toto/titi")
    }
}