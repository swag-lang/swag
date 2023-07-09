using Swag

struct BaseForm
{
    borderSize: f32
}

#[Swag.ConstExpr]
func getField(type: typeinfo, name: string)->const* TypeValue
{
    if type.kind == TypeInfoKind.Struct
    {
        var typeStruct = cast(const* TypeInfoStruct) type
        visit it: typeStruct.fields
        {
            if it.name == name
                return it
        }
    }

    return null
}

#test
{
    var type = getField(@typeof(BaseForm), @nameof(BaseForm.borderSize))
    @assert(type != null)
}