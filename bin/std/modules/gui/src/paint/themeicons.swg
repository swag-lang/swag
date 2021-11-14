#global public

enum ThemeIcons24
{
    WndMinimize
    WndMaximize
    WndClose
}

enum ThemeIcons64
{
    Question
    Warning
    Critical
    Information
}

impl Theme
{
    mtd getIcon24(idx: ThemeIcons24)->Icon
    {
        var result: retval
        result.imageList = &res.icons24
        result.index = cast(s32) idx
        result.size = 24
        return result
    }

    mtd getIcon64(idx: ThemeIcons64)->Icon
    {
        var result: retval
        result.imageList = &res.icons64
        result.index = cast(s32) idx
        result.size = 64
        return result
    }
}