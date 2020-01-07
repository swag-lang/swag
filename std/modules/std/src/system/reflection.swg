namespace std.reflection
{
    using swag

    public 
    {
        #[swag.constexpr]
        func getMember(type: const* TypeInfo, name: string)->const* TypeInfoParam
        {
            if type.kind != TypeInfoKind.Struct
                return null
            typeStruct := cast(const* TypeInfoStruct) type
            visit it: typeStruct.methods
            {
                if it.namedParam == name
                    return it
            }

            return null
        }

        #[swag.constexpr]
        func hasMember(type: const* TypeInfo, name: string)->bool
        {
            return getMember(type, name) != null
        }
    }
}