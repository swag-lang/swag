namespace std.reflection
{
    using swag

    public
    {
        #[swag.constexpr]
        {
            func getMethod(type: typeinfo, name: string)->const* TypeInfoParam
            {
                if type.kind != TypeInfoKind.Struct
                    return null
                typeStruct := cast(const* TypeInfoStruct) type
                visit it: typeStruct.methods
                {
                    if it.name == name
                        return it
                }

                return null
            }

            func hasMethod(type: typeinfo, name: string)->bool => getMethod(type, name) != null
            func hasDrop(type: typeinfo)->bool => hasMethod(type, "opDrop")
            func hasPostMove(type: typeinfo)->bool => hasMethod(type, "opPostMove")
            func hasPostCopy(type: typeinfo)->bool => hasMethod(type, "opPostCopy")

            func hasInterface(type: typeinfo, itf: typeinfo)->bool
            {
                if type.kind != TypeInfoKind.Struct
                    return false
                typeStruct := cast(const* TypeInfoStruct) type
                visit it: typeStruct.interfaces
                {
                    if it.name == itf.name
                        return true
                }

                return false
            }
        }
    }
}