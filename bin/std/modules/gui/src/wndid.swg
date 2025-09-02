#global public

alias WndId = string

#[Swag.Mixin]
func newCmdId(val: string)
{
    #ast "namespace CmdId { const " ++ val ++ " = \"" ++ val ++ "\" }"
}