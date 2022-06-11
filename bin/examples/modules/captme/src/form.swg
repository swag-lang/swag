using Pixel, Core, Gui

interface IForm
{
    paintGizmo:     func(self, *Painter, f32, FormFlags)
    paint:          func(self, *Painter, f32, bool)
    mouse:          func(self, *EditView, Math.Point, *MouseEvent)->bool
    hitTest:        func(self, Math.Point, f32)->bool
    boundRect:      func(self)->Math.Rectangle
    moveForm:       func(self, s32, s32)
    moveBaseForm:   func(self, s32, s32, s32, s32)
    setBaseForm:    func(self, s32, s32, s32, s32)
    movePoint:      func(self, u32, s32, s32)
    edit:           func(self, *MouseEvent, Math.Rectangle, Math.Point, Math.Point, f32)->bool
    setup:          func(self, *Capture)
    isSame:         func(self, *BaseForm)->bool
    copy:           func(self, *BaseForm, *BaseForm)
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

    capture:        *Capture
    paintPos:       Math.Rectangle
    alpha:          u8 = 100
    shadowOffset:   Math.Vector2 = @{2, 2}
    borderSize:     f32 = 5
    borderColor:    Color = Argb.White
    backColor:      Color = Argb.Red
    dashStyle:      DashStyle = None
}

impl BaseForm
{
    func adaptSize(size: f32, minSize, maxSize: f32, z: f32, quickMode: bool, dstMin: f32 = 4, dstMax: f32 = 16)->f32
    {
        if !quickMode
            return size * z
        return Math.map(size * z, minSize, maxSize, dstMin, dstMax)
    }
}