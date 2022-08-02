#global if #os == Swag.TargetOs.Windows and #arch == Swag.TargetArch.X86_64

func toto(str: string, x: cvarargs)
{
    var valist: Swag.CVaList
    @cvastart(&valist)

    ptr := dref cast(^^s64) &valist
    //@print(bitcast(s32) ptr[0], " ", bitcast(s32) ptr[1], "\n")
    @assert(bitcast(s32) ptr[0] == 111)
    @assert(bitcast(s32) ptr[1] == 222)
}

//#[Swag.PrintBc]
#test
{
    toto("oula", 111, 222)
}