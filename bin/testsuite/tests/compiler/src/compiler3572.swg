#global testerror

struct A
{
}

impl A
{
    mtd(op: string) opAssign(t: s32) {}
}

#test
{
    var xxxx: A
    xxxx += @(a, b) { }
}