#import "core"
using core

#[swag.mixin]
func mk(name: string, type: typeinfo)
{
    #ast
    {
        var str: String = format.toString("var %: % = 444", name, type.name)
        return str.toString()
    }
}

mk("toto", s32)
#placeholder toto

#test
{
    @assert(toto == 444)
    @assert(@typeof(toto) == s32)
}