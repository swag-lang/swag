//#global skip

struct(T) A
{
}

impl A
{
    mtd(op: string) opAssign(t: T)
    {
    }
}

#test
{
    var xxxx: A'closure(s32)
    xxxx += func(a)
    {
    }
}
