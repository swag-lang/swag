func hasAttribute(value: typeinfo, attribute: typeinfo)->bool
{
    return attribute == attrB
}

var G = 0
func beginSection(fct: *void, kind: s32, skipFat = false) throw
{
    G = 1
}

attr attrA();
attr attrB();

func readNative(fct: *void)->u32 throw
{
    return 0;
}

struct SerializationError
{
    err: string
}

//#[Swag.PrintBc]//, Swag.Optimize("", false)]
func beginStruct(fct: *void, structType: typeinfo, _structData: const *u8)->bool throw
{
    if  hasAttribute(structType, attrA) or
        hasAttribute(structType, attrB)
    {
        beginSection(fct, 0)
    }
    else
    {
        // Read version
        let sig = readNative(fct)
        if sig != 0xFF:
            throw SerializationError{"stream is corrupted"}
    }

    return false
}

#test
{
    discard assume beginStruct(null, #typeof(SerializationError), null)
}