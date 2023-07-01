interface IForm
{
    mtd paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    mtd paint(app: *Application, painter: *Painter, z: f32, quickMode: bool)
    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    mtd isInside(pt: Math.Point)->bool
    mtd hitTest(pt: Math.Point, z: f32)->bool
    mtd boundRect()->Math.Rectangle
    mtd moveForm(newPosX, newPosY: s32)
    mtd moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    mtd setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    mtd movePoint(pt: u32, newPosX, newPosY: s32)
    mtd edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    mtd setup(capt: *Capture)
    mtd isSame(ptr: *BaseForm)->bool
    mtd copy(dst, src: *BaseForm)
}

#[Swag.EnumFlags]
enum FormFlags
{
    Zero
    Hot
    Selected
}

enum FormSubKind
{
    None
    Highlight
    PixSel
}

struct BaseForm
{
    x, y: s32 = 0
    w, h: s32 = 32

    alpha:              u8 = 100
    shadowOffset:       Math.Vector2 = {2, 2}
    borderSize:         f32 = 5
    borderColor:        Color = Argb.White
    backColor:          Color = Argb.Red
    dashStyle:          DashStyle = None
    quickModeOpaque:    bool
    paintShadow         = true
    subKind:            FormSubKind = None
    joinStyle           = JoinStyle.Round

    #[Serialization.NoSerialize]
    {
        capture:    *Capture
        paintPos:   Math.Rectangle
    }
}

impl BaseForm
{
    func adaptSize(size: f32, minSize, maxSize: f32, z: f32, quickMode: bool, dstMin: f32 = 2, dstMax: f32 = 6)->f32
    {
        if !quickMode
            return size * z
        return Math.map(size * z, minSize, maxSize, dstMin, dstMax)
    }
}