struct A {}

impl A
{
    //mtd(op: string) opAssign(t: func(s32, s32)) {}
    mtd opAffect(t: func(s32, s32)) {}
}

#test
{
    var xxxx: A
    xxxx = func(a, b) {}
    //var y: func(s32, s32)
    //y = func(a, b) {}
}
