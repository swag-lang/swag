using Pixel, Core, Gui

interface IForm
{
    paintGizmo:     func(self, *Painter, f32, FormFlags)
    paint:          func(self, *Painter, f32)
    mouse:          func(self, *EditView, Math.Point, *MouseEvent)->bool
    hitTest:        func(self, Math.Point, f32)->bool
    boundRect:      func(self)->Math.Rectangle
    moveForm:       func(self, s32, s32)
    moveBaseForm:   func(self, s32, s32, s32, s32)
    setBaseForm:    func(self, s32, s32, s32, s32)
    movePoint:      func(self, u32, s32, s32)
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
}
