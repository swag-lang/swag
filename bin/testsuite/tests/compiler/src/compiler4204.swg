func toto()->u64 => 0xFFFFFFFF_FFFFFFFF

func titi()->u32
{
    return cast(u32) #over toto()
}

//#[Swag.PrintBc]
#test
{
    var x = titi()
}