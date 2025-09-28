#global public

enum ThemeIcons24
{
    WndMinimize
    WndMaximize
    WndClose
    Search
    RightPopupArrow
    BottomPopupArrow
    SortArrowDown
    SortArrowUp
    SortArrowRight
    Picker
    Play
    Pause
    Stop
    SendToBack
    SendBackward
    BringForward
    BringToFront
    Plus
    More
    Undo
    Redo
    Copy
    Paste
    Cut
    Expanded
    Collapsed
    RemoveAll
    Remove
    Options
    About
    ZoomIn
    ZoomOut
    Save
    SaveAs
    Export
    Download
    OpenFile
    Update
    Duplicate
    Quit
    Link
    Sort
    Edit
    Filter
    Minimize
}

enum ThemeIcons64
{
    Question
    Warning
    Critical
    Information
    HourGlass
}

impl Theme
{
    mtd getIcon24(idx: ThemeIcons24, size = 24'f32)->Icon
    {
        var result: retval
        result.imageList = .res.icons24
        result.index     = cast(s32) idx
        result.sizeX     = size
        result.sizeY     = size
        return result
    }

    mtd getIcon64(idx: ThemeIcons64, size = 64'f32)->Icon
    {
        var result: retval
        result.imageList = .res.icons64
        result.index     = cast(s32) idx
        result.sizeX     = size
        result.sizeY     = size
        return result
    }
}
