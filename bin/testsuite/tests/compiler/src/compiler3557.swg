#global testerror "Err0192"

#message(Swag.CompilerMsgMask.SemFunctions)
{
    let itf = @compiler
    var msg = itf.getMessage()
    itf.compileString("impl I for A {\n mtd toto() {\nG += 1\n}\n}")
}
