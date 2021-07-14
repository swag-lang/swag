using Swag
#[AttrUsage(AttributeUsage.EnumValue)]
attr T();

enum TITI
{
    #[T]
    {
        A
        B
    }
}
