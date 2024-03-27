#global testerror "Err0019"

struct A
{
}

impl A
{
    #[Swag.Overload]
    mtd(op: string) opAssign(t: func(s32, s32)) {}
    #[Swag.Overload]
    mtd(op: string) opAssign(t: func(f32, f32)) {}
}

#test
{
    var xxxx: A
    xxxx += func(a, b) { }
}
