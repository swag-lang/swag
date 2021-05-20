
using Core

//#[Swag.printbc]
#test
{
    var str1: String = "toto"
    var str2: String =,move str1
    @assert(str1.isNull())
    @assert(str2 == "toto")

    /*@print(cast(s64) cast(u64) str2.buffer)
    @print("\n")
    @print(cast(s64) cast(u64) &str2.size) */
}