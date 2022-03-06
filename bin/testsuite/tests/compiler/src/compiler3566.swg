#global testerror

struct A
{
}

impl A
{
    //mtd(op: string) opAssign(t: func(s32, s32)) {}
    mtd opAffect(t: func(s32, s32)->s32) {}
}

#test
{
    var xxxx: A
    xxxx = @(a, b)->s32 { return 0; }
    //var y: func(s32, s32)
    //y = @(a, b) {}
}