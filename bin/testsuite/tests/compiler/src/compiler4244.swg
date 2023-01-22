struct A
{
    x: s32
}

impl A
{
    mtd opDrop()
    {
        @assert(x == 0)
        x = 1
    }
}

#[Swag.Mixin]
func toto()->A
{
    return A{}
}

//#[Swag.PrintBc]
#test
{
    x := toto()
    x1 := toto()
}