#global testerror "Err0115"

struct A
{
}

impl A
{
    mtd(op: string) opAssign(t: func(s32, s32)) {}
    mtd(op: string) opAssign(t: func(f32, f32)) {}
}

#test
{
    var xxxx: A
    xxxx += func(a, b) { }
}
