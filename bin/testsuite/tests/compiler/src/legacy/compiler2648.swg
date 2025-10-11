struct(T) ST
{
    x:   T
}

struct(T) Arr
{
    x:   T
}

#[Swag.Macro]
public func(T) toto2648(v: [..] T, stmt: #code void)
{
    /*func titi(val: *void)
    {
        var ptr = cast(*ST'T) val
        #inject(stmt)
    }*/

    var v1: Arr'(ST'T)
    //titi(&v1.x)
}

#test
{
    var v: [2] s32
    toto2648(cast() v)
    {
    }
}
