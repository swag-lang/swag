#global testerror "Err0093"

struct A {}
impl A
{
    mtd opIndex(u: u64)->s32
    {
        return 0
    }
}

#test
{
    var x: A
    let y  = x[0].y
}
