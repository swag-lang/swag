#[Swag.Inline]
func toto()->{ v: s32 }
{
    var result: retval
    result.v = 10
    return result
}

func titi()->s32
{
    return toto().v
}

#test
{
    let x = titi()
    @assert(x == 10)
}
