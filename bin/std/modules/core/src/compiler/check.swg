#global namespace Compiler
#global export

#[Swag.Compiler]
func(T) checkCanCompareToSame()->bool
{
    if Reflection.isStruct(T) and !@defined(T.opEquals) do
        @compilererror("type '" ++ @typeof(T).fullname ++ "' does not implement 'opEquals'", @location(T))
    return true
}