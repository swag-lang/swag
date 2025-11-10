//#[Swag.PrintBc]
#[Swag.Safety(.All, .Overflow, false)]
func toto(buffer: *bool)
{
    var total: u64
    let c      = #ref buffer
    total = c ? 1 : 0
    @assert(c == false)
    c = true
}

#test
{
    var b = false
    toto(&b)
    @assert(b == true)
}
