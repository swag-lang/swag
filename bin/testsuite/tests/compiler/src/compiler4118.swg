#global testerror

#[Swag.Safety("bool", true)]
#test
{
    x := true
    ptr := cast(*u8) &x
    dref ptr += 1
    @assert(x != true)
}