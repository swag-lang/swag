//#global testerror

#[Swag.Safety("", true)]
#test
{
    var x = 1's8
    let y = 100'u32
    x <<= y
}
