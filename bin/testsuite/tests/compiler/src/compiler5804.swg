#global testerror "Err0428"
func toto() {}

struct A
{
    x = &toto
}
/*
#global skip

#[Swag.ConstExpr]
struct C
{
    a: s32
}

impl C
{
    #[Swag.Inline]
    func from(x: s32)->C
    {
        return {x}
    }
}

var t = C.from(52)
const t1 = C.from(52)

#test
{
    @assert(t.a == 52)
    @assert(t1.a == 52)
}*/