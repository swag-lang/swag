struct Array
{
    buf: s32 = 666
}

impl Array
{
    mtd opDrop()
    {
        @assert(buf == 666)
    }
}

func GetFontData()->s32 throw
{
    throw "toto"
    return 0
}

//#[Swag.PrintBc]
func getDataFromHFONT()->Array throw
{
    var result: retval
    var outFontBufferLen = GetFontData()
    return result
}

//#[Swag.Optim("", false)]
//#[Swag.PrintBc]
func createFromHFONT() throw
{
    var buf = getDataFromHFONT()
}

#test
{
    catch createFromHFONT()
    @assert(cast(string) @err() == "toto")
}