#compiler(swag.CompilerMessageKind.PassAllDone)
{
    msg := @msg()

    @print(cast(s64) msg.kind)
    @print(" ")
    @print(msg.moduleName)
}