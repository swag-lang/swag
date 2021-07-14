#[Swag.AttrUsage(Swag.AttributeUsage.StructVariable)]
attr T();

struct TOTO
{
    #[T]
    {
        A: s32
        B: s32
    }
}
