#global public

enum SurfaceMessageKind
{
    None
    Resize
    Paint
}

struct SurfaceMessage
{
    kind:   SurfaceMessageKind = .None
    param0: u64
    param1: u64
}

#[Swag.EnumFlags]
enum SurfaceFlags
{
    Caption
    SysMenu
    MinimizeBox
    MaximizeBox

    DlgFrame
    SizeFrame

    OverlappedWindow
    PopupWindow
    PaletteWindow
}