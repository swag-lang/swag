struct(T) ST
{
    x:   T
}

struct(T) Arr
{
    x:   T
}

#[Swag.Macro]
func(T) toto(v: [..] T, stmt: #code void)
{
    func titi(val: *void)
    {
        var ptr = cast(*ST'T) val
        #inject(stmt)
    }

    var v1: Arr'(ST'T)
    titi(&v1.x)
}

#test
{
    var v: [2] s32
    toto(cast() v)
    {
    }
}
