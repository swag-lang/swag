//#global testerror

#[Swag.Safety(.All, .All, true)]
#test
{
    var x = 127's8
    let y = 1'u32
    x <<= y
    //@print(cast(s64) x)
}
