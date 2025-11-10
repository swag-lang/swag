struct A
{
    x:   s32
}

impl A
{
    mtd opDrop()
    {
        @assert(me.x == 0)
        me.x = 1
    }
}

#[Swag.Mixin]
func toto()->A
{
    {
        return A{}
    }
}

//#[Swag.PrintBc]
#test
{
    var x  = toto()
    var x1 = toto()
}
