#global testerror "Err0028"

struct A
{
}

impl A
{
    #[Swag.Overload]
    mtd(op: string) opAssign(t: closure(s32, s32)) {}
    #[Swag.Overload]
    mtd(op: string) opAssign(t: func(f32, f32)) {}
}

#test
{
    var xxxx: A
    xxxx += func(a, b) { }
}