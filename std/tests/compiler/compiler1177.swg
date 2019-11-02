using swag
enum TOTO : s64
{
    A = 666
    B = 100
}

typealias X: TOTO

func fromName(name: string)->X
{
    let t = cast(const *TypeinfoEnum) @typeof(TOTO)
    loop t.values
    {
        param := t.values[index]
        if(param.namedParam == name)
        {
            ptr := cast(*TOTO) param.value
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