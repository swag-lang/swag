using Swag
enum TOTO : s64
{
    A = 666
    B = 100
}

typealias X = TOTO

func fromName(name: string)->X
{
    let t = cast(const *TypeInfoEnum) @typeof(TOTO)
    loop t.values
    {
        let param = t.values[#index]
        if(param.name == name)
        {
            let ptr = cast(const *TOTO) param.value
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