#global testerror "Err0561"

struct A {}

impl A
{
    mtd(op: string) opAssign(t: s32) {}
}

#test
{
    var xxxx: A
    xxxx += func(a, b) {}
}
