#global testerror "Err0578"

func toString()->[..] u8
{
    return [0, 1, 2, 3]
}

#[Swag.Safety(.All, .BoundCheck, true)]
#test
{
    let x = toString()
    @assert(x[4] == 3)
}
