namespace std.reflection
{
    using swag

    public
    {
        #[swag.constexpr]
        func getMember(type: typeinfo, name: string)->const* TypeInfoParam
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

        #[swag.constexpr]
        func hasMember(type: typeinfo, name: string)->bool
        {
            return getMember(type, name) != null
        }
    }
}