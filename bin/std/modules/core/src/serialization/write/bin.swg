#global namespace Serialization.Write
#global public
using Swag

struct BinOptions
{
}

struct Bin
{
    options:    BinOptions
    output:     *ConcatBuffer
}

impl Bin
{
    mtd startWrite(buf: *ConcatBuffer)
    {
        output = buf
    }

    mtd beginRoot()
    {
    }

    mtd endRoot()
    {
    }

    mtd beginSequence(typeElem: typeinfo, data: const *void, countElem: uint)->bool
    {
        output.addNative(countElem)

        // Dynamic array of raw serializable types
        // Just write them in one batch
        if typeElem.kind == .Struct
        {
            structType := cast(const *TypeInfoStruct) typeElem
            if structType.fromGeneric == @typeof(Array)
            {
                valueType := structType.generics[0].pointedType
                (raw, sizeof) := isRawSerializable(valueType)

                if raw
                {
                    bufArr := cast(const *Array'u8) data
                    output.addBytes(@mkslice(bufArr.buffer, countElem * sizeof))
                    return true
                }
            }
        }
        else
        {
            (raw, sizeof) := isRawSerializable(typeElem)
            if raw
            {
                output.addBytes(@mkslice(cast(const *u8) data, countElem * sizeof))
                return true
            }
        }

        return false
    }

    mtd endSequence()
    {
    }

    mtd beginStruct(type: typeinfo, data: const *u8)->bool
    {
        return false
    }

    mtd endStruct()
    {
    }

    mtd beginElement(name: string, type: typeinfo)
    {
    }

    mtd endElement()
    {
    }

    mtd beginValue(type: typeinfo)
    {
    }

    mtd endValue()
    {
    }

    /////////////////////////////////
    // VALUES
    /////////////////////////////////

    mtd(NT) writeNative(value: NT)
    {
        #if NT == string
        {
            output.addNative(cast(u32) @countof(value))
            output.addBytes(value)
        }
        #else
        {
            output.addNative(value)
        }
    }
}