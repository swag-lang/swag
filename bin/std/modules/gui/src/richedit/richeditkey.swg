using Core

impl RichEdit
{
    mtd charPressed(key: Input.Key, ch: rune, mdf: Input.KeyModifiers)->bool
    {
        cmd := getMappingCommand(key, mdf)
        if cmd != .None
            return true
        insertRune(ch)
        return true
    }

    mtd keyPressed(key: Input.Key, mdf: Input.KeyModifiers)->bool
    {
        cmd := getMappingCommand(key, mdf)
        if cmd == .None
            return true
        cmdExecute(cmd)
        ensureCaretIsVisible()
        return true
    }
}