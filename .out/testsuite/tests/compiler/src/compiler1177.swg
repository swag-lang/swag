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
        param := t.values[@index]
        if(param.name == name)
        {
            ptr := cast(const *TOTO) param.value
            return ptr[0]
        }
    }

    return TOTO.A
}

#test
{
    @assert(cast(s64) fromName("A") == 666)
    @assert(cast(s64) fromName("B") == 100)
}