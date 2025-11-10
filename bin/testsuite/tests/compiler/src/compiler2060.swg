struct AA
{
    x:   s32
}

interface ITF
{
    func cb(me);
}

impl ITF for AA
{
    func impl cb(me) {}
}

func interfaceOf(structType, itfType: const *Swag.TypeInfoStruct)->const *void
{
    //for i in structType.interfaces
    return structType.interfaces[0].value
    /*{
        if @stringcmp(@dataof(structType.interfaces[i].name),
                   @dataof(itfType.name),
                   @countof(structType.interfaces[i].name),
                   @countof(itfType.name))
        {
            return structType.interfaces[i].value
        }
    }*/

    //return null
}

#test
{
    var x: AA
    var tt = interfaceOf(#typeof(x), ITF)
}
