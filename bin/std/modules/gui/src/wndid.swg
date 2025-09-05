#global public

alias WndId = string

#[Swag.Mixin]
func newCmdId(val: string)
{
    #ast "const " ++ val ++ " = \"" ++ #scopename ++ "." ++ val ++ "\""
}
