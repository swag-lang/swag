//#global testerror

struct A
{
}

impl A
{
    mtd(op: string) opAssign(t: closure(s32, s32))
    {
    }
}

#test
{
    var xxxx: A
    xxxx += func(a, b)
    {
    }
}
