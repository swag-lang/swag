using swag
#[attributeUsage(AttributeUsage.Field)]
attr T();

struct TOTO
{
    #[T]
    {
        A: s32
        B: s32   
    }
}
