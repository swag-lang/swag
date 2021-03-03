struct(T) ST
{
    x: T
}

struct(T) Arr
{
    x: T
}

#[swag.macro]
func(T) toto(v: [..] T, stmt: code)
{
    func titi(val: *void)
    {
        ptr := cast(*ST'T) val
        #mixin stmt
    }

    var v1: Arr'(ST'T)
    titi(&v1.x)
}

#test
{
    var v: [2] s32
    toto(v)
    {

    }
}