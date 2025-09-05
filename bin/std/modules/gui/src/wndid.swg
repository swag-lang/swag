#global public

alias WndId = string

#[Swag.Mixin]
func newCmdId(val: string)
{
    #ast "const " ++ val ++ " = \"" ++ "Core.Path.getFileName(#file)" ++ val ++ "\""
}