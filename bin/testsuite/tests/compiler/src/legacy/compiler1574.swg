using Swag

#[AttrUsage(AttributeUsage.Function | AttributeUsage.Struct | AttributeUsage.Enum)]
attr NC();

#[NC]
func toto()
{
}

#[NC]
struct Point
{
}

#[NC]
enum Color
{
}