#global testerror

#message(Swag.CompilerMsgMask.SemFunctions)
{
    itf := @compiler()
    msg := itf.getMessage()
    itf.compileString("impl I for A {\n mtd toto() {\nG += 1\n}\n}")
}
