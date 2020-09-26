struct AA
{
    x: s32
}

interface ITF
{
    cb: func(self)
}

interface ITF1
{
    cb: func(self)
}

impl ITF for AA
{
    func cb(using self)
    {
    }
}

impl ITF1 for AA
{
    func cb(using self)
    {
    }
}

func interfaceOf(structType, itfType: const *swag.TypeInfoStruct)->const *void
{
    loop i: structType.interfaces
    {
        if @strcmp(@dataof(structType.interfaces[i].name),
                   @dataof(itfType.name),
                   @countof(structType.interfaces[i].name),
                   @countof(itfType.name))
        {
            return structType.interfaces[i].value
        }
    }

    return null
}

#test
{
    var x: AA
    tt := interfaceOf(@typeof(x), ITF1)
}