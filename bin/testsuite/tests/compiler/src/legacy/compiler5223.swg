struct DateTime
{
    x:   s32 = 55
}

func toto() throw {}

func parse(str: string, checkDate = true)->{ value: DateTime, eat: u32 } throw
{
    var result: retval
    toto()
    toto()
    return result
}

#test
{
    let x = catch parse("toto")
}
