struct(T) ST
{
    x: T
}

struct(T) Arr
{
    x: T
}

#[Swag.macro]
public func(T) toto2648(v: [..] T, stmt: code)
{
    /*func titi(val: *void)
    {
        ptr := cast(*ST'T) val
        #mixin stmt
    }*/

    var v1: Arr'(ST'T)
    //titi(&v1.x)
}

#test
{
    var v: [2] s32
    toto2648(v)
    {

    }
}