//#global testerror

#[Swag.Safety(.All, .All, true)]
#test
{
    var x = 1's8
    let y = 100'u32
    x <<= y
    //@print(cast(s64) x)
}
