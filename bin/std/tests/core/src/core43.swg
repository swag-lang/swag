using Core

//#[Swag.PrintBc]
#test
{
    var str1: String = "toto"
    var str2: String =,move str1
    @assert(str1.isNull())
    @assert(str2 == "toto")
}
