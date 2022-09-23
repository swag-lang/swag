using Core, Time

//#[Swag.PrintBc]
#test
{
    x := DateTime.now()
    stra := Format.toString("%{a}", x)
    strb := Format.toString("%{b}", x)
    strc := Format.toString("%{c}", x)
    strd := Format.toString("%{d}", x)
}