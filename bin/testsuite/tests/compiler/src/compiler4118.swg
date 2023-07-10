#global testerror

#[Swag.Safety("bool", true)]
#test
{
    let x = true
    var ptr = cast(*u8) &x
    dref ptr += 1
    @assert(x != true)
}