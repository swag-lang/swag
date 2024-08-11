#global testerror "Err0226"

struct A
{
}

impl A
{
    //mtd(op: string) opAssign(t: func(s32, s32)) {}
}

#test
{
    var xxxx: A
    xxxx += func(a, b) { }
}