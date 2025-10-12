func hasAttribute(value: typeinfo, attribute: typeinfo)->bool
{
    return attribute == attrB
}

var G = 0
func beginSection(fct: #null *void, kind: s32, skipFat = false) throw
{
    G = 1
}

attr attrA()
attr attrB()

func readNative(fct: #null *void)->u32 throw
{
    return 0
}

struct SerializationError
{
    err: string
}

//#[Swag.PrintBc]//, Swag.Optimize(.All, false)]
func beginStruct(fct: #null *void, structType: typeinfo, _structData: #null const *u8)->bool throw
{
    if hasAttribute(structType, attrA) or
       hasAttribute(structType, attrB)
    {
        beginSection(fct, 0)
    }
    else
    {
        // Read version
        let sig = readNative(fct)
        if sig != 0xFF do
            throw SerializationError{"stream is corrupted"}
    }

    return false
}

#test
{
    discard assume beginStruct(null, #typeof(SerializationError), null)
}
