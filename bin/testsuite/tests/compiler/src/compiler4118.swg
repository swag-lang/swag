#global testerror "Saf0003"

#[Swag.Safety("bool", true)]
#test
{
    var x = true
    var ptr = cast(*u8) &x
    dref ptr += 1
    @assert(x != true)
}