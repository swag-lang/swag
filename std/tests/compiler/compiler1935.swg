struct SALUTE
{
    s, t: f64
}

func cc(sl: [..] u8, val: u32, titi: string, st: SALUTE)
{
    @print(st.s)
    x := @countof(sl)
    loop val
    {
    }
}

//#[swag.printbc]
#test
{
    var st: SALUTE
    cc("toto", 55, "titi", st)
//    var x: String = "toto"
//    var y: Array'u16 = x.toUnicode16()
}