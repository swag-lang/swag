namespace std.reflection
{
    using swag

    public
    {
        #[swag.constexpr]
        {
            // Get information about a specific method in a given struct
            // Returns null if the method does not exist
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

            // Helpers functions to know if a struct has a special function
            func hasMethod(type: typeinfo, name: string)->bool => getMethod(type, name) != null
            func hasDrop(type: typeinfo)->bool => hasMethod(type, "opDrop")
            func hasPostMove(type: typeinfo)->bool => hasMethod(type, "opPostMove")
            func hasPostCopy(type: typeinfo)->bool => hasMethod(type, "opPostCopy")

            // Returns true if the interface 'itf' is implemented in the given structure type
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

            // Returns the enum value name corresponding (exactly) to the value pointed by 'valueAddr'
            func getEnumName(type: typeinfo, valueAddr: const* void)->string
            {
                if type.kind != TypeInfoKind.Enum
                    return null
                typeEnum := cast(const *TypeInfoEnum) type

                // Get the user value, as an 'u64' or a string
                var val1: u64
                var val1s: string
                switch typeEnum.rawType.sizeof
                {
                case 1:  val1 = cast(u64) deref cast(const *u8) valueAddr
                case 2:  val1 = cast(u64) deref cast(const *u16) valueAddr
                case 4:  val1 = cast(u64) deref cast(const *u32) valueAddr
                case 8:  val1 = cast(u64) deref cast(const *u64) valueAddr
                case 16: val1s = deref cast(const *string) valueAddr
                default: return null
                }

                visit it: typeEnum.values
                {
                    // Get the enum value, as an 'u64' or a string
                    var val2: u64
                    var val2s: string
                    switch typeEnum.rawType.sizeof
                    {
                    case 1:  val2 = cast(u64) deref cast(const *u8) it.value
                    case 2:  val2 = cast(u64) deref cast(const *u16) it.value
                    case 4:  val2 = cast(u64) deref cast(const *u32) it.value
                    case 8:  val2 = cast(u64) deref cast(const *u64) it.value
                    case 16: val2s = deref cast(const *string) it.value
                    }

                    if val1 == val2 && val1s == val2s
                        return it.name
                }

                return null
            }

            // Returns the enum value name corresponding (exactly) to the value pointed by 'valueAddr'
            func getEnumFlagsName(type: typeinfo, valueAddr: const* void)->std.text.String
            {
                var result: std.text.String
                if type.kind != TypeInfoKind.Enum
                    return result
                typeEnum := cast(const *TypeInfoEnum) type

                // Get the user value, as an 'u64' or a string
                var val1: u64
                switch typeEnum.rawType.sizeof
                {
                case 1:  val1 = cast(u64) deref cast(const *u8) valueAddr
                case 2:  val1 = cast(u64) deref cast(const *u16) valueAddr
                case 4:  val1 = cast(u64) deref cast(const *u32) valueAddr
                case 8:  val1 = cast(u64) deref cast(const *u64) valueAddr
                default: return result
                }

                visit it: typeEnum.values
                {
                    // Get the enum value, as an 'u64' or a string
                    var val2: u64
                    switch typeEnum.rawType.sizeof
                    {
                    case 1:  val2 = cast(u64) deref cast(const *u8) it.value
                    case 2:  val2 = cast(u64) deref cast(const *u16) it.value
                    case 4:  val2 = cast(u64) deref cast(const *u32) it.value
                    case 8:  val2 = cast(u64) deref cast(const *u64) it.value
                    }

                    if val1 & val2
                    {
                        if !result.isEmpty() result += " | "
                        result += it.name
                    }
                }

                return result
            }
        }
    }
}