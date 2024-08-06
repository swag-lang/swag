#global testerror "Err0047"
//#[Swag.PrintBc]
#test
{
    const x: string = null
    var y: string
    y = "null"
    const z = x orelse y
}