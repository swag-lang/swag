#global skip

struct A
{
    x: s32 = 555
}

var G = 0
impl A
{
    mtd opDrop()
    {
        G += 1
    }
}

#[Swag.PrintBc, Swag.Optim("", false)]
#test
{
    var x: [2] A
    count := 1
    count -= 1
    @init(&x[0], count)
}