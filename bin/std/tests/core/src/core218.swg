using Core

#test
{
    let str = "toto la fripouille toto la fripouille"
    @assert(Utf8.indexOf(str, "la") == 5)
    @assert(Utf8.lastIndexOf(str, "la") == 24)
    @assert(Utf8.lastIndexOf(str, "lla") == Swag.U64.Max)
}
