
using Core.File

//#[Swag.PrintBc]
#test
{
    {
        var v1 = "c:/"
        var v2 = "toto"
        var res = Path.combine(v1, v2)
        @assert(res == "c:/toto")
        res = Path.combine(res.toString(), "titi")
        @assert(res == "c:/toto/titi")
    }
}