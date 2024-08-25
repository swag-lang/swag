using Core

func cc(sl: const [..] u8, val: s32)
{
    for @countof(sl)
    {
    }
}

//#[Swag.PrintBc]
#test
{
    Core.Debug.assert(true, "")
    cc("toto", 666)
    //    var x: String = "toto"
    //    var y: Array'u16 = x.toUnicode16()
}
