struct ActionUndo {}

#[Swag.Mixin]
func newCmdId(val: string)
{
    #ast "const " ++ val ++ " = \"" ++ val ++ "\""
}

impl ActionUndo
{
    newCmdId("Undo")
}

interface IActionUI
{
    mtd accept(x: string);
}

impl IActionUI for ActionUndo
{
    mtd impl accept(x: string)
    {
        switch x
        {
        case ActionUndo.Undo:
            break
        }
    }
}
