typealias fct = func(s32, s32)->s32

struct A
{
    sig: fct
}

impl A
{
    mtd opCast()->fct
    {
        return sig
    }
}

func toto(x, y: s32) => x + y

#[Swag.Optim("", false)]
#test
{
    var v: A
    v.sig = &toto

    let r: fct = cast(fct) v
    let r1 = r(1, 2)
    @assert(r1 == 3)
}