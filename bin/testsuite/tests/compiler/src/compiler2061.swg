struct AA
{
    x: s32
}

interface ITF
{
    func cb(self);
}

interface ITF1
{
    func cb(self);
}

impl ITF for AA
{
    func impl cb(using self)
    {
    }
}

impl ITF1 for AA
{
    func impl cb(using self)
    {
    }
}

func interfaceOf(structType, itfType: nn const *Swag.TypeInfoStruct)->const *void
{
    for i in structType.interfaces
    {
        if @stringcmp(structType.interfaces[i].name, itfType.fullname)
        {
            return structType.interfaces[i].value
        }
    }

    return null
}

#test
{
    var x: AA
    var tt = interfaceOf(#typeof(x), ITF1)
}