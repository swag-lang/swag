using swag

enum AttributeUsage
{
    All
}

attr attributeUsage(usage: AttributeUsage) -> "func";

#[attributeUsage(AttributeUsage.All)]
func MyAttribute()
{

}

#test
{
    ptr := cast(const *TypeInfoFunc) @typeof(&MyAttribute)
    visit it: ptr.attributes
    {
        //@print(it.item0)
    }
}