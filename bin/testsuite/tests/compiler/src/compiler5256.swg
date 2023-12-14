#global skip
struct X {}
struct Y {}

impl X
{
    mtd opIndex()->Y
    {
        return {}
    }
}

impl Y
{
    mtd opIndex()->s32
    {
        return 555
    }
}

#[Swag.PrintBc]
#test
{
    var x: X
    let a = x[0][1]
    @print(a)
}