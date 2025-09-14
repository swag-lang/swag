using Core

//#[Swag.PrintBc]
#test
{
    let str1: String = "toto"
    let str2: String = str1
    @assert(str1 == "toto")
    @assert(str2 == "toto")
    @assert(str1.buffer != str2.buffer)
}
