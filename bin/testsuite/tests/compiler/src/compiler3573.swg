//#global testerror

struct A {}

impl A
{
    #[Swag.Overload]
    mtd(op: string) opAssign(t: s32) {}
    #[Swag.Overload]
    mtd(op: string) opAssign(t: func(s32, s32)) {}
}

#test
{
    var xxxx: A
    xxxx += func(a, b) {}
}
