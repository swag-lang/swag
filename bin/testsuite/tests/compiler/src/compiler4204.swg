func toto()->u64 => 0xFFFFFFFF_FFFFFFFF

func titi()->u32
{
    return cast,safe(u32) toto()
}

//#[Swag.PrintBc]
#test
{
    x := titi()
}