using Swag
enum TOTO : s64
{
    A = 666
    B = 100
}

alias X = TOTO

func fromName(name: string)->X
{
    var t = cast(const *TypeInfoEnum) @typeof(TOTO)
    loop t.values
    {
        var param = t.values[@index]
        if(param.name == name)
        {
            var ptr = cast(const *TOTO) param.value
            return dref ptr
        }
    }

    return TOTO.A
}

#test
{
    @assert(cast(s64) fromName("A") == 666)
    @assert(cast(s64) fromName("B") == 100)
}