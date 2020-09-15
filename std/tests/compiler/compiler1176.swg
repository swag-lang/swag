using swag
enum TOTO : s64
{
    A = 666
    B = 100
}

func fromName(name: string)->TOTO
{
    let t = cast(const *TypeInfoEnum) @typeof(TOTO)
    loop t.values
    {
        if(t.values[@index].name == name)
        {
            ptr := cast(const *TOTO) t.values[@index].value
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