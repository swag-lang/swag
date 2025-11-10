func toto()->u64 => 0xFFFFFFFF_FFFFFFFF

func titi()->u32
{
    return cast #wrap (u32) toto()
}

//#[Swag.PrintBc]
#test
{
    var x = titi()
}
