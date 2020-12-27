/*#import "core"
using core

#[swag.mixin]
func mk(name: string, type: typeinfo)
{
    #ast
    {
        var str: String = strconv.format("var %: %", name, type.name)
        return str.toString()
    }
}

mk("toto", s32)*/