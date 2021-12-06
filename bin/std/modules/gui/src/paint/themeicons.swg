#global public

enum ThemeIcons24
{
    WndMinimize
    WndMaximize
    WndClose
    Search
    RightPopupArrow
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
    mtd getIcon24(idx: ThemeIcons24, size = 24'f32)->Icon
    {
        var result: retval
        result.imageList = &res.icons24
        result.index = cast(s32) idx
        result.size = size
        return result
    }

    mtd getIcon64(idx: ThemeIcons64, size = 64'f32)->Icon
    {
        var result: retval
        result.imageList = &res.icons64
        result.index = cast(s32) idx
        result.size = size
        return result
    }
}