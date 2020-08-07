#compiler(swag.CompilerMessageKind.SemanticFunc)
{
    msg := cast(const *swag.CompilerMessageSemantic) @msg()
    @print(msg.name)
    @print("\n")
}