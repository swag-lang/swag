#import "core"
using core

#test
{
    str := "toto la fripouille toto la fripouille"
    @assert(utf8.indexOf(str, "la") == 5)
    @assert(utf8.lastIndexOf(str, "la") == 24)
    @assert(utf8.lastIndexOf(str, "lla") == swag.MaxUINT)
}