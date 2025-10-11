struct AA
{
    x:   s32
}

interface ITF
{
    func cb(me);
}

interface ITF1
{
    func cb(me);
}

impl ITF for AA
{
    func impl cb(me) {}
}

impl ITF1 for AA
{
    func impl cb(me) {}
}

func interfaceOf(structType, itfType: const *Swag.TypeInfoStruct)->#null const *void
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
