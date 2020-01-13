using swag
#[attributeUsage(AttributeUsage.EnumValue)]
attr T();

enum TITI
{
    #[T]
    {
        A
        B   
    }
}
