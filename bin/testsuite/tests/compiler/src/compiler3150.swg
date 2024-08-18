#global #if #os == Swag.TargetOs.Windows and #arch == Swag.TargetArch.X86_64

func toto(str: string, x: cvarargs)
{
    var valist: Swag.CVaList
    @cvastart(&valist)

    let ptr = dref cast(^^s64) &valist
    //@print(cast(s32),bit ptr[0], " ", cast(s32),bit ptr[1], "\n")
    @assert(cast(s32),bit ptr[0] == 111)
    @assert(cast(s32),bit ptr[1] == 222)
}

//#[Swag.PrintBc]
#test
{
    toto("oula", 111, 222)
}