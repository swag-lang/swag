#global public

alias WndId = string

#[Swag.Mixin]
func newCommandId(val: #code void)
{
    #ast "const " ++ #nameof(val) ++ " = \"" ++ #nameof(val) ++ "\""
}