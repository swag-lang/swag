//#global testerror

struct A
{
}

impl A
{
    mtd(op: string) opAssign(t: closure(s32, s32)) {}
    //mtd(op: string) opAssign(t: func(f32, f32)) {}
}

#test
{
    var xxxx: A
    xxxx += func(a, b) { }
}
