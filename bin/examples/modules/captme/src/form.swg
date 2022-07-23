interface IForm
{
    paintGizmo:     mtd(*Painter, f32, FormFlags)
    paint:          mtd(*Painter, f32, bool)
    mouse:          mtd(*EditView, Math.Point, *MouseEvent)->bool
    hitTest:        mtd(Math.Point, f32)->bool
    boundRect:      mtd(bool)->Math.Rectangle
    moveForm:       mtd(s32, s32)
    moveBaseForm:   mtd(s32, s32, s32, s32)
    setBaseForm:    mtd(s32, s32, s32, s32)
    movePoint:      mtd(u32, s32, s32)
    edit:           mtd(*MouseEvent, Math.Rectangle, Math.Point, Math.Point, f32)->bool
    setup:          mtd(*Capture)
    isSame:         mtd(*BaseForm)->bool
    copy:           mtd(*BaseForm, *BaseForm)
}

#[Swag.EnumFlags]
enum FormFlags
{
    Zero
    Hot
    Selected
}

struct BaseForm
{
    x, y: s32 = 0
    w, h: s32 = 32

    alpha:          u8 = 100
    shadowOffset:   Math.Vector2 = @{2, 2}
    borderSize:     f32 = 5
    borderColor:    Color = Argb.White
    backColor:      Color = Argb.Red
    dashStyle:      DashStyle = None

    #[Serialization.NoSerialize]
    {
        capture:    *Capture
        paintPos:   Math.Rectangle
    }
}

impl BaseForm
{
    func adaptSize(size: f32, minSize, maxSize: f32, z: f32, quickMode: bool, dstMin: f32 = 2, dstMax: f32 = 16)->f32
    {
        if !quickMode
            return size * z
        return Math.map(size * z, minSize, maxSize, dstMin, dstMax)
    }
}