using Swag

struct BaseForm
{
    borderSize: f32
}

#[Swag.ConstExpr]
func getField(type: typeinfo, name: string)->#null const *TypeValue
{
    if type.kind == TypeInfoKind.Struct
    {
        let typeStruct = cast(const *TypeInfoStruct) type
        foreach it in typeStruct.fields
        {
            if it.name == name do
                return it
        }
    }

    return null
}

#test
{
    let type = getField(#typeof(BaseForm), #nameof(BaseForm.borderSize))
    @assert(type != null)
}
