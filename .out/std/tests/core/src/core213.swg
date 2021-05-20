
using core

#[Swag.mixin]
func mk(name: string, type: typeinfo)
{
    #ast
    {
        var str: String = Format.toString("var %: % = 444", name, type.name)
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