using Swag

struct BaseForm
{
    borderSize: f32
}

#[Swag.ConstExpr]
func getField(type: nn typeinfo, name: string)->const* TypeValue
{
    if type.kind == TypeInfoKind.Struct
    {
        let typeStruct = cast(const* TypeInfoStruct) type
        foreach it in typeStruct.fields
        {
            if it.name == name:
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