#[Swag.attributeUsage(Swag.AttributeUsage.StructVariable)]
attr T();

struct TOTO
{
    #[T]
    {
        x: s32 = 4
        y: s32 = 10
    }
}

//#[Swag.printbc]
#test
{
    var pt: TOTO
    @assert(pt.x == 4)
    @assert(pt.y == 10)
}