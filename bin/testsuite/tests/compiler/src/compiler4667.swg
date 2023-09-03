#global testerror "Err0670"
//#[Swag.PrintBc]
#test
{
    const x: ^u8 = null
    var y: ^u8 = null
    y += 1
    const z = x orelse y
}