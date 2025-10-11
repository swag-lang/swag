interface IForm
{
    mtd paintGizmo(painter: *Painter, z: f32, flags: FormFlags);
    mtd paint(painter: *Painter, cxt: FormPaintContext);
    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool;
    mtd isInside(pt: Math.Point)->bool;
    mtd hitTest(pt: Math.Point, z: f32)->bool;
    mtd boundRect()->Math.Rectangle;
    mtd moveForm(newPosX, newPosY: s32);
    mtd moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32);
    mtd scaleBaseForm(scale: f32);
    mtd setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32);
    mtd movePoint(pt: u32, newPosX, newPosY: s32);
    mtd edit(evt: *MouseEvent, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool;
    mtd setup(capt: #null *Capture);
    mtd isSame(ptr: *BaseForm)->bool;
    mtd copy(dst, src: *BaseForm);
    mtd mustClipToBound()->bool { return false; }
}

struct FormPaintContext
{
    z:             f32
    quickMode:     bool
    hitTest:       bool
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
    x, y:                s32 = 0
    w, h:                s32 = 32

    alpha:               u8 = 100
    shadowOffsetX:       f32 = 2
    shadowOffsetY:       f32 = 2
    borderSize:          f32 = 5
    borderColor:         Color = Argb.White
    backColor:           Color = Argb.Red
    dashStyle:           DashStyle = None
    quickModeOpaque:     bool
    bezier:              bool
    paintShadow:         bool = true
    subKind:             FormSubKind = None
    joinStyle            = JoinStyle.Round
    scale:               f32 = 0
    rotate:              f32 = 0

    #[Serialization.NoSerialize]
    {
        capture:      *Capture
        paintPos:     Math.Rectangle
        freeze:       bool
    }
}

impl BaseForm
{
    func adaptSize(size: f32, minSize, maxSize: f32, cxt: FormPaintContext, dstMin: f32 = 4, dstMax: f32 = 8)->f32
    {
        if !cxt.quickMode do
            return size * cxt.z
        return Math.map(size * cxt.z, minSize, maxSize, dstMin, dstMax)
    }

    func drawHitTest(itf: IForm, pt: Math.Point, z: f32)->bool
    {
        var painter: Painter{fake: true}
        painter.begin()

        var cxt: FormPaintContext
        cxt.z       = z
        cxt.hitTest = true
        itf.paint(&painter, cxt)

        painter.end()

        let pos = Math.Vector2{pt.x, pt.y}
        foreach cmd in painter.commandBuffer
        {
            if cmd.id != .DrawTriangles do
                continue
            for var i = 0; i < cmd.drawTriangles.count; i += 3
            {
                let tri = cmd.drawTriangles.start + i
                let xx0 = painter.vertexBuffer[tri].pos
                let xx1 = painter.vertexBuffer[tri + 1].pos
                let xx2 = painter.vertexBuffer[tri + 2].pos
                if Math.Geometry.isInTriangle(pos, xx0, xx1, xx2) do
                    return true
            }
        }

        return false
    }
}
