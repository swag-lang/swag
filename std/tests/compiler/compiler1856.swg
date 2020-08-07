#compiler(swag.CompilerMsgKindMask.PassAllDone)
{
    msg := @msg()

    /*@print(cast(s64) msg.kind)
    @print(" ")
    @print(msg.moduleName)*/

    @assert(msg.moduleName == "tests.compiler")
}