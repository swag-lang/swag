using Core, Core.Time

//#[Swag.PrintBc]
#test
{
    let x    = DateTime.now()
    let stra = Format.toString("%{a}", x)
    let strb = Format.toString("%{b}", x)
    let strc = Format.toString("%{c}", x)
    let strd = Format.toString("%{d}", x)
}
