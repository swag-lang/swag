//#global testerror

#[Swag.Safety(.All, "", true)]
#test
{
    var x = 127's16
    let y = 1'u32
    x >>= y
    //@print(cast(s64) x)
}
