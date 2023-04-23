func toto()->u64 => 0xFFFFFFFF_FFFFFFFF

func titi()->u32
{
    return cast,over(u32) toto()
}

//#[Swag.PrintBc]
#test
{
    x := titi()
}