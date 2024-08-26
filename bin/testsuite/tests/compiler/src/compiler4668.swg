#global testerror "Err0045"
//#[Swag.PrintBc]
#test
{
    const x: string = null
    var y: string
    y = "null"
    const z = x orelse y
}