using Core

func cc(sl: const [..] u8, val: s32)
{
    loop @countof(sl)
    {
    }
}

//#[Swag.PrintBc]
#test
{
    Core.Debug.assert(true, "")
    cc("toto", 666)
}
