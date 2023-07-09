using Core, Time

//#[Swag.PrintBc]
#test
{
    var x = DateTime.now()
    var stra = Format.toString("%{a}", x)
    var strb = Format.toString("%{b}", x)
    var strc = Format.toString("%{c}", x)
    var strd = Format.toString("%{d}", x)
}