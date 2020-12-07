#[swag.attributeUsage(swag.AttributeUsage.Field)]
attr T();

struct TOTO
{
    #[T]
    {
        x: s32 = 4 
        y: s32 = 10
    }
}

//#[swag.printbc]
#test
{
    var pt: TOTO
    @assert(pt.x == 4)
    @assert(pt.y == 10)
}