#global #if #os == Swag.TargetOs.Windows
using Core, Pixel, Gui, Core.Math

const W = 1024
const H = 1024

namespace Res
{
    var image0:      Texture
    var image1:      Texture
    var image2:      Texture
    var typeface:    *TypeFace
    var fontDefault: *Font
    var font20:      *Font
    var font40:      *Font
    var fontFam:     FontFamily
}

var g_ModeAA:       bool = true
var g_PaintQuality: PaintQuality = Normal
var g_Pattern:      bool = true
var g_Page:         s32 = 5
var g_Angle:        f32 = 0
const MaxPages      = 8

func dataPath()->String
{
    var dataPath = Path.getDirectoryName(#curlocation.fileName)
    dataPath = assume Path.getDirectoryName(dataPath)
    dataPath = assume Path.getDirectoryName(dataPath)
    dataPath = assume Path.getDirectoryName(dataPath)
    dataPath = assume Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "std/tests/pixel/datas")
}

func imagePath(name: string)->String
{
    return Path.combine(dataPath(), name)
}

func displayPage0(painter: *Pixel.Painter)
{
    var pen = Pen.createSolid(Argb.Black, 20)

    if g_Pattern do
        pen.dashStyle = .Dot

    let trs = painter.getTransform()

    //////////////////////////////////////
    {
        painter.setTransform(trs)
        painter.translateTransform(50, 50)
        pen.brush.color = Argb.Black
        pen.begCapStyle = .None
        pen.endCapStyle = .None
        painter.drawLine(0, 0, 0, 150, pen)

        pen.begCapStyle = .Square
        pen.endCapStyle = .Square
        painter.drawLine(100, 0, 150, 150, pen)

        pen.begCapStyle = .Round
        pen.endCapStyle = .Round
        painter.drawLine(200, 0, 250, 150, pen)

        pen.begCapStyle     = .ArrowAnchor
        pen.endCapStyle     = .ArrowAnchor
        pen.dashBegCapStyle = .Round
        pen.dashEndCapStyle = .Round
        painter.drawLine(300, 0, 350, 150, pen)

        pen.begCapStyle     = .SquareAnchor
        pen.endCapStyle     = .SquareAnchor
        pen.dashBegCapStyle = .Triangle
        pen.dashEndCapStyle = .Triangle
        pen.brush.color     = Color.fromArgb(0x5f, Argb.Black)
        painter.drawLine(400, 0, 450, 150, pen)
    }

    painter.drawLine(500, 0, 550, 150, Argb.Brown, 2)

    pen.dashBegCapStyle = .Round
    pen.dashEndCapStyle = .Round
    painter.setTransform(trs)
    painter.translateTransform(50, 250)
    pen.brush.color = Argb.Blue
    pen.joinStyle   = .None
    painter.drawRect(0, 0, 100, 100, pen)
    pen.joinStyle = .Bevel
    painter.drawRect(150, 0, 100, 100, pen)
    pen.joinStyle = .Round
    painter.drawRect(300, 0, 100, 100, pen)
    pen.joinStyle = .Miter
    painter.drawRect(450, 0, 100, 100, pen)

    pen.brush.color = Argb.Red
    var path: LinePath
    path.lineTo(100, 0)
    path.lineTo(100, 50)
    path.lineTo(200, 50)
    pen.joinStyle = .Miter

    painter.setTransform(trs)
    painter.translateTransform(50, 400)
    painter.drawPath(&path, pen)
    pen.joinStyle = .Bevel

    painter.setTransform(trs)
    painter.translateTransform(250, 400)
    painter.drawPath(&path, pen)
    pen.joinStyle = .Round

    painter.setTransform(trs)
    painter.translateTransform(450, 400)
    painter.drawPath(&path, pen)

    pen.brush.color = Argb.Green
    path.start(0, 0)
    path.lineTo(100, 0)
    path.lineTo(100, 50)
    path.lineTo(0, 50)
    pen.begCapStyle = .Round
    pen.endCapStyle = .Round

    painter.setTransform(trs)
    painter.translateTransform(50, 500)
    painter.drawPath(&path, pen)

    path.start(0, 0)
    path.bezierTo(100, 0, 50, -50)
    painter.setTransform(trs)
    painter.translateTransform(250, 500)
    painter.drawPath(&path, pen)

    path.start(0, 0)
    path.lineTo(50, 0)
    path.bezierTo(100, 50, 100, 0, 50, 50)
    path.lineTo(200, 50)
    painter.setTransform(trs)
    painter.translateTransform(400, 500)
    painter.drawPath(&path, pen)

    path.start(0, 0)
    pen.brush.color = Argb.Purple
    path.arcTo(0, 50, 50, 50, 0'deg, 90'deg)
    path.lineTo(100, 50)
    path.arcTo(150, 50, 50, 50, -90'deg, 0'deg)

    painter.setTransform(trs)
    painter.translateTransform(50, 600)
    painter.drawPath(&path, pen)

    painter.setTransform(trs)
    painter.translateTransform(250, 600)
    pen = Pen.createSolid(Argb.Blue, 10)
    painter.drawRoundRect(0, 0, 100, 100, 20, 20, pen)

    painter.setTransform(trs)
    painter.translateTransform(400, 600)
    pen.size = 4
    painter.drawCircle(50, 50, 50, pen)

    painter.setTransform(trs)
    painter.translateTransform(500, 600)
    painter.drawEllipse(50, 50, 25, 50, pen)

    var brush = Brush.createSolid(Argb.Black)
    painter.setTransform(trs)
    painter.translateTransform(600, 600)
    painter.fillCircle(50, 50, 50, brush)

    //////////////////////////////////////
    painter.resetState()
    painter.setParams(g_ModeAA ? .Antialiased : .Default)
    painter.setQuality(g_PaintQuality)

    //////////////////////////////////////
    path.start(0, 50)
    path.lineTo(50, 50)
    path.lineTo(50, 20)
    path.lineTo(100, 75)
    path.lineTo(50, 130)
    path.lineTo(50, 100)
    path.lineTo(0, 100)
    path.close()
    brush.color = Argb.Red
    painter.setTransform(trs)
    painter.translateTransform(10, 750)
    painter.fillPath(&path, brush)
    path.polygon.offset(-10)
    brush.color = Argb.Blue
    painter.fillPath(&path, brush)

    //////////////////////////////////////
    painter.setTransform(trs)
    brush = Brush.createTiled(Res.image0)
    painter.fillRect(750, 50, 100, 150, brush)
    brush = Brush.createTexture(Res.image1)
    painter.fillRoundRect(750, 250, 150, 150, 20, 20, brush)
    painter.fillCircle(850, 450, 50, brush)

    //////////////////////////////////////
    var lpl: LinePathList
    painter.setTransform(trs)
    painter.translateTransform(750, 550)
    let p0 = lpl.newPath()
    p0.setCircle(50, 160, 50)
    let p1 = lpl.newPath()
    p1.setRect(0, 0, 100, 100)
    painter.fillPath(&lpl, Argb.Red)
}

func displayPage1(painter: *Pixel.Painter)
{
    var brush: Brush
    brush.color = Argb.Black

    let trs = painter.getTransform()
    painter.translateTransform(50, 50)
    painter.setAntialiased(false)

    var cpt, cptLines = 0
    foreach e in HatchStyle
    {
        brush.type   = .Hatch
        brush.hatch  = e
        brush.uvMode = .Fill
        painter.fillRect(0, 0, 48, 48, brush)
        brush.uvMode = .Tile
        painter.fillRect(0, 75, 48, 48, brush)

        painter.translateTransform(75, 0)
        cpt += 1
        if cpt == 12
        {
            cptLines += 1
            cpt = 0
            painter.setTransform(trs)
            painter.translateTransform(50, 50 + 150.0 * cptLines)
        }
    }
}

func displayPage2(painter: *Pixel.Painter)
{
    let trs = painter.getTransform()

    painter.drawTexture(200, 0, Res.image1)

    painter.setInterpolationMode(.Pixel)
    painter.drawTexture(50, 0, 96, 96, Res.image0)
    painter.setInterpolationMode(.Linear)
    painter.drawTexture(50, 110, 96, 96, Res.image0)

    painter.rotateTransform(45.0'deg, 100, 300)
    painter.drawTexture(50, 250, 96, 96, Res.image0)

    painter.setTransform(trs)
    let pen = Pen.createHatch(.SolidDiamond, Argb.Black, 20)
    painter.drawRoundRect(400, 700, 200, 200, 10, 10, pen)

    painter.drawTexture(50, 500, 150, 150, {228, 103, 206, 167}, Res.image1)
    painter.drawTexture(220, 500, 150, 150, {228, 103, 206, 167}, Res.image2)
    painter.drawTexture(50, 700, {228, 103, 206, 167}, Res.image1)
}

func displayPage3(painter: *Pixel.Painter)
{
    let trs = painter.getTransform()

    let brush = Brush.createHatch(.SolidDiamond, Argb.Black)
    painter.pushClippingRect({0, 0, 100, 100})
    painter.fillCircle(100, 100, 100, brush)
    painter.drawCircle(100, 100, 100, Argb.Red, 4)
    painter.popClippingRect()

    painter.startClippingRegion(.Set)
    painter.fillRoundRect(150, 0, 100, 100, 20, 20, Argb.Green)
    painter.setClippingRegionMode(.Clear)
    painter.fillCircle(200, 50, 50, Argb.White)
    painter.endClippingRegion()

    painter.fillRect(0, 0, 1000, 1000, Argb.Red)
    painter.resetClippingRegion()

    {
        painter.drawCircle(500, 500, 400, Argb.Red, 1)
    }

    {
        painter.drawRoundRect(500, 500, 200, 200, 10, 10, Color.fromArgb(0x51, Argb.Blue), 15)
    }

    {
        var lp1: LinePath
        lp1.start(800, 800)
        lp1.bezierTo(150, 150, 250, 50)
        painter.drawPath(&lp1, Pen.createSolid(Color.fromArgb(0x51, Argb.Red), 50))
    }

    {
        var lp: LinePath
        lp.start(50, 800)
        lp.bezierTo(800, 50, 250, 50)
        painter.drawPath(&lp, Pen.createSolid(Color.fromArgb(0x51, Argb.Black), 50))
    }

    {
        var lp1: LinePath
        lp1.start(400, 400)
        lp1.lineTo(500, 450)
        lp1.lineTo(400, 500)
        painter.drawPath(&lp1, Pen.createSolid(Color.fromArgb(0x51, Argb.Blue), 50))
    }

    {
        painter.setTransform(trs)
        painter.translateTransform(400, 200)
        painter.setBlendingMode(BlendingMode.Add)
        painter.fillRect(0, 0, 150, 150, Argb.Blue)
        painter.setBlendingMode(BlendingMode.Max)
        painter.fillRect(50, 50, 150, 150, Argb.Red)
        painter.setBlendingMode(BlendingMode.Alpha)
    }

    {
        var lp1: LinePathList
        var lp   = lp1.newPath()
        lp.start(50, 0)
        lp.lineTo(50, 100)
        lp = lp1.newPath()
        lp.start(0, 50)
        lp.lineTo(100, 50)

        var pen = Pen.createSolid(Color.fromArgb(0x51, Argb.Green), 50)
        painter.setTransform(trs)
        painter.translateTransform(700, 200)
        painter.drawPath(&lp1, pen, .Separate)

        painter.translateTransform(0, 150)
        painter.drawPath(&lp1, pen, .Merge)
        painter.translateTransform(150, 0)
        pen.brush = Brush.createTexture(Res.image1)
        painter.drawPath(&lp1, pen, .Merge)

        painter.translateTransform(0, 150)
        pen           = Pen.createSolid(Color.fromArgb(0x51, Argb.Green), 15)
        pen.dashStyle = .Dash
        painter.drawPath(&lp1, pen, .Merge)
    }

    {
        painter.setTransform(trs)
        painter.drawArc(250, 600, 80, 0, 1.5, Argb.Green, 18)
    }
}

func displayPage4(painter: *Pixel.Painter)
{
    let trs = painter.getTransform()

    {
        let rect: Rectangle{350, 50, 100, 100}
        painter.fillRect(rect, Color.fromArgb(0x1F, Argb.Blue))

        var rs: RichString = "on multiple lines\nthis is\na text\non multiple lines"
        var sf: RichStringFormat
        sf.horzAlignment = .Center
        sf.vertAlignment = .Center
        sf.lineGap       = 1
        sf.font.regular  = Res.fontDefault
        sf.palette[0]    = Argb.Blue
        painter.drawRichString(rect, &rs, sf)
    }

    {
        let rect: Rectangle{500, 50, 100, 100}
        painter.fillRect(rect, Color.fromArgb(0x1F, Argb.Blue))

        var rs: RichString = "<u>on multiple lines this is a text on multiple lines<u>"
        var sf: RichStringFormat
        sf.flags |= .WordWrap
        sf.lineGap      = 1
        sf.font.regular = Res.fontDefault
        sf.palette[0]   = Argb.Black
        painter.drawRichString(rect, &rs, sf)
    }

    {
        let rect = Rectangle{650, 50, 200, 700}
        painter.fillRect(rect.x, rect.y, rect.width, rect.height, Argb.White)

        var sf = RichStringFormat{flags: StringFormatFlags.WordWrap}
        sf.horzAlignment = .Center
        sf.font.regular  = Res.fontDefault
        sf.palette[0]    = Argb.Black

        const TXT = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus. Suspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor. Cras elementum ultrices diam. Maecenas ligula massa, varius a, semper congue, euismod non, mi. Proin porttitor, orci nec nonummy molestie, enim est eleifend mi, non fermentum diam nisl sit amet erat. Duis semper. Duis arcu massa, scelerisque vitae, consequat in, pretium a, enim. Pellentesque congue. Ut in risus volutpat libero pharetra tempor. Cras vestibulum bibendum augue. Praesent egestas leo in pede. Praesent blandit odio eu enim. Pellentesque sed dui ut augue blandit sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Aliquam nibh. Mauris ac mauris sed pede pellentesque fermentum. Maecenas adipiscing ante non diam sodales hendrerit."
        var rs:   RichString = TXT
        painter.drawRichString(rect, &rs, sf)
    }

    {
        let rect = Rectangle{100, 225, 200, 700}
        var sf   = RichStringFormat{flags: StringFormatFlags.WordWrap}
        sf.horzAlignment = .Center
        sf.font.regular  = Res.fontDefault
        sf.palette[0]    = Color.fromArgb(0x1FFF0000)
        var rs: RichString = "<u>TOTO<u>"
        painter.drawRichString(rect, &rs, sf)
    }

    {
        painter.drawString(100, 200, "this is a TEXT on a single line", Res.fontDefault, Color.fromArgb(0x5FFF0000), UnderlineStyle.Underline)
    }

    {
        let fnt = assume Res.fontFam.getFont(FontFamilyStyle.Regular)
        painter.fillCircle(100, 100, 5, Argb.Red)
        painter.drawString(100, 100, "abcdefghijkl", fnt, Argb.Black)
    }

    const str = "abcdefghijklmnopqrstuvwxyz"

    {
        let brush   = Brush.createTexture(Res.image1)
        let measure = Painter.measureString(str, Res.font40)
        let off     = Res.font40.height - measure.height
        painter.fillRect(100, 300 + off, measure.width, measure.height, Color.fromArgb(0x1F, Argb.Blue))
        painter.fillRect(100, 300 + off, measure.width, measure.ascent, Color.fromArgb(0x1F, Argb.Red))
        painter.drawString(100, 300, str, Res.font40, brush, UnderlineStyle.Strikeout)
        painter.drawString(100, 350, str, Res.font40, Argb.Black)
    }

    {
        var pathl: LinePathList
        painter.translateTransform(50, 470)

        var pen = Pen.createSolid(Argb.Red, 5)
        pen.joinStyle  = .Round
        pen.plotOffset = 2

        assume Res.typeface.getStringOutline(&pathl, str, 70)
        painter.drawPath(&pathl, pen)
        painter.fillPath(&pathl, Argb.Black)

        painter.setTransform(trs)
    }

    {
        var p: LinePathList
        painter.translateTransform(50, 570)
        assume Res.typeface.getStringOutline(&p, "A B C D", 80)
        p.flatten(g_PaintQuality)
        p.offset(4, JoinStyle.Round)
        p.clean()
        painter.fillPath(&p, Argb.Blue)
        painter.drawPath(&p, Argb.Red, 2)

        painter.setTransform(trs)
    }

    {
        var p: LinePathList
        painter.translateTransform(50, 670)
        assume Res.typeface.getStringOutline(&p, "D F G H", 70)
        p.flatten(g_PaintQuality)
        p.offset(4, JoinStyle.Round)
        p.clean()
        painter.fillPath(&p, Argb.Red)
        painter.drawPath(&p, Argb.Black, 2)

        painter.setTransform(trs)
    }

    {
        var p: LinePath
        painter.translateTransform(500, 570)
        p.start(0, 0)
        p.lineTo(100, 0)
        p.lineTo(100, 100)
        p.lineTo(0, 100)
        p.close()
        p.flatten(g_PaintQuality)
        painter.drawPath(&p, Argb.Red, 2)
        p.polygon.offset(10, JoinStyle.Miter)
        painter.drawPath(&p, Argb.Green, 2)
        p.polygon.offset(10, JoinStyle.Bevel)
        painter.drawPath(&p, Argb.Green, 2)
        p.polygon.offset(10, JoinStyle.Round)
        painter.drawPath(&p, Argb.Green, 4)

        painter.setTransform(trs)
    }
}

func displayPage5(painter: *Pixel.Painter)
{
    let trs = painter.getTransform()

    {
        painter.setTransform(trs)
        painter.translateTransform(100, 400)

        var lp: [?] Math.Vector2 = [{0, 0}, {40, 0}, {40, 40}]
        painter.fillPolygon(lp, Brush.createSolid(Argb.Red))
    }

    {
        painter.setTransform(trs)
        painter.translateTransform(500, 300)

        var lp: LinePath
        lp.start(0, 100)
        lp.curveTo(50, 80)
        lp.curveTo(100, 20)
        lp.curveTo(150, 80)
        lp.curveTo(200, 100)
        painter.drawPath(&lp, Pen.createSolid(Argb.Black, 2))
    }

    {
        painter.setTransform(trs)
        painter.translateTransform(500, 400)

        var lp: LinePath
        lp.start(20, 50)
        lp.curveTo(100, 10)
        lp.curveTo(200, 100)
        lp.curveTo(300, 50)
        lp.curveTo(400, 80)
        painter.drawPath(&lp, Pen.createSolid(Argb.Black, 2))
    }

    {
        painter.setTransform(trs)
        painter.translateTransform(500, 500)

        var lp: LinePath
        lp.start(60, 60)
        lp.curveTo(150, 80)
        lp.curveTo(200, 40)
        lp.curveTo(180, 120)
        lp.curveTo(120, 100)
        lp.curveTo(80, 160)
        lp.curveTo(40, 140)
        lp.close()
        painter.fillPath(&lp, Argb.Blue)
        painter.drawPath(&lp, Pen.createSolid(Argb.Red, 4))
    }

    {
        painter.setTransform(trs)
        painter.translateTransform(0, 0)

        var lp: LinePath
        lp.start(0, 0)
        lp.bezierTo(500, 0, 250, 300)
        var pen = Pen.createSolid(Argb.Red, 10)
        pen.begCapStyle  = .ArrowAnchor
        pen.endCapStyle  = .ArrowAnchor
        pen.capEndScaleX = 5
        pen.capEndScaleY = 2.5
        pen.capBegScaleX = 5
        pen.capBegScaleY = 2.5
        painter.drawPath(&lp, pen)

        let bv = Math.Curve.getBezierBoundingBox({0, 0}, {500, 0}, {250, 300})
        painter.drawRect(bv, Pen.createSolid(Argb.Blue, 5))
    }

    {
        var pen = Pen.createSolid(Argb.Red, 20)
        painter.setTransform(trs)
        painter.translateTransform(50, 50)
        pen.brush.color = Argb.Black
        pen.begCapStyle = .ArrowAnchor
        pen.endCapStyle = .ArrowAnchor
        painter.drawLine(200, 0, 250, 150, pen)
    }

    {
        var lp: LinePath
        lp.start(0, 0)
        lp.bezierTo(100, 100, 0, 100)
        var pen = Pen.createSolid(Argb.Red, 10)
        pen.begCapStyle  = .ArrowAnchor
        pen.endCapStyle  = .ArrowAnchor
        pen.capEndScaleX = 5
        pen.capEndScaleY = 2.5
        pen.capBegScaleX = 5
        pen.capBegScaleY = 2.5
        painter.drawPath(&lp, pen)

        let bv = Math.Curve.getBezierBoundingBox({0, 0}, {100, 100}, {0, 100})
        painter.drawRect(bv, Pen.createSolid(Argb.Blue, 5))
    }
}

func displayPage6(painter: *Pixel.Painter)
{
    var str:    RichString
    var format: RichStringFormat

    format.font       = Res.fontFam
    format.palette[0] = Argb.Black
    format.palette[1] = Argb.Red
    format.palette[2] = Argb.Green

    str = "<u>normal</u> <i>italic<b>bold</b></i> another <p1>one</p> <p2><u>two</u></p>"
    painter.drawRichString({10, 10, 100, 100}, &str, format)

    str = "this is\na\ntext"
    painter.translateTransform(0, 100)
    painter.fillRect({10, 10, 100, 100}, Argb.Gray)
    painter.drawRichString({10, 10, 100, 100}, &str, format)

    format.horzAlignment = .Center
    painter.fillRect({110, 10, 100, 100}, Argb.Gray)
    painter.drawRichString({110, 10, 100, 100}, &str, format)

    str                  = "Lorem ipsum dolor sit amet, <b>consectetur</b> adipiscing elit. Sed non risus.\n<p1>Suspendisse</p> lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, <u>dolor</u>."
    format.horzAlignment = .Center
    format.flags         = .WordWrap
    painter.fillRect({10, 200, 200, 200}, Argb.Gray)
    painter.drawRichString({10, 200, 200, 100}, &str, format)

    format.horzAlignment = .Justify
    format.flags         = .WordWrap
    painter.fillRect({220, 200, 200, 200}, Argb.Gray)
    painter.drawRichString({220, 200, 200, 100}, &str, format, true)
}

func displayPage7(renderer: *Pixel.RenderOgl, painter: *Pixel.Painter)
{
    let tgt = renderer.getRenderTarget(256, 256)
    painter.drawRoundRect(200, 20, 100, 100, 10, 10, Color.fromArgb(80, Argb.Red), 15)
    painter.drawRoundRect(230, 50, 100, 100, 10, 10, Color.fromArgb(80, Argb.Green), 15)
    painter.bindRenderTarget(tgt, true)
    painter.clear(Argb.White)
    //painter.setBlendingMode(.Copy)
    painter.drawRoundRect(20, 20, 100, 100, 10, 10, Color.fromArgb(80, Argb.Red), 15)
    painter.drawRoundRect(50, 50, 100, 100, 10, 10, Color.fromArgb(80, Argb.Green), 15)
    painter.unbindRenderTarget()
    painter.drawTexture(0, 0, tgt.colorBuffer)

    {
        var p: LinePath
        p.start(200, 200)
        p.lineTo(300, 200)
        p.lineTo(250, 250)
        p.lineTo(250, 150)
        p.close()
        painter.fillPolygon(&p, Argb.Red)
        painter.drawPath(&p, Argb.Blue, 2)
    }

    {
        var p: LinePath
        p.start(400, 300)
        p.lineTo(600, 100)
        p.lineTo(600, 400)
        p.lineTo(100, 400)
        p.close()
        painter.fillPolygon(&p, Argb.Red)
        painter.drawPath(&p, Argb.Blue, 2)
    }

    renderer.releaseRenderTarget(tgt)
}

struct MyView
{
    using wnd: Wnd
}

impl IWnd for MyView
{
    mtd impl onPaintEvent(event: *PaintEvent)
    {
        let painter = event.bc.painter
        painter.setParams(g_ModeAA ? .Antialiased : .Default)
        painter.setQuality(g_PaintQuality)
        painter.pushState()
        painter.fillRect(.getClientRect(), Color.fromArgb(0xFFEEEEEE))

        switch g_Page
        {
        case 0:
            displayPage0(painter)
        case 1:
            displayPage1(painter)
        case 2:
            displayPage2(painter)
        case 3:
            displayPage3(painter)
        case 4:
            displayPage4(painter)
        case 5:
            displayPage5(painter)
        case 6:
            displayPage6(painter)
        case 7:
            displayPage7(event.bc.renderer, painter)
        }

        painter.popState()
        painter.translateTransform(10, 920)
        painter.drawString(0, 0, "F1: AA on/off F2: Quality F3: Pattern F4: Change page", Res.font20, Argb.Black)

        if Env.hasArg("swag.test")
        {
            Threading.Thread.sleep(100'ms)
            g_Page = (g_Page + 1) % MaxPages
            if g_Page == 0 do
                Env.exit(0)
            .invalidate()
        }
    }

    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind != .KeyPressed do
            return
        evt.accepted = true

        switch evt.key
        {
        case F1:
            g_ModeAA = g_ModeAA ? false : true
            .invalidate()

        case F2:
            g_PaintQuality = cast(PaintQuality) ((cast(u32) g_PaintQuality + 1) % 2)
            .invalidate()

        case F3:
            g_Pattern = g_Pattern ? false : true
            .invalidate()

        case F4:
            if evt.modifiers & .Alt do
                .surface.destroy()
            else
            {
                g_Page = (g_Page + 1) % MaxPages
                .invalidate()
            }
        }
    }
}

#main
{
    var app: Gui.Application

    let view    = Wnd.create'MyView()
    let surface = try app.createSurface(100, 100, W, H, view: view)

    assume
    {
        let rdr  = surface.app.getRenderer()
        let img0 = Image.load(imagePath("basn0g01.png"))
        var img1 = Image.load(imagePath("test.gif"))
        Res.image0 = rdr.addImage(img0)
        Res.image1 = rdr.addImage(img1)
        Res.image2 = rdr.addImage(img1)
        img1.grayScale()
        img1.setAlpha(128)
        rdr.updateTexture(Res.image2, img1.pixels.buffer)

        Res.typeface    = TypeFace.load(imagePath("arial.ttf"))
        Res.fontDefault = Font.create(Res.typeface, cast(u32) ((12 * 96) / 72))
        Res.font20      = Font.create(Res.typeface, 20)
        Res.font40      = Font.create(Res.typeface, 40)

        Res.fontFam.regular    = Font.create(imagePath("arial.ttf"), 20)
        Res.fontFam.bold       = Font.create(imagePath("arialbd.ttf"), 20)
        Res.fontFam.italic     = Font.create(imagePath("ariali.ttf"), 20)
        Res.fontFam.boldItalic = Font.create(imagePath("arialbi.ttf"), 20)
    }

    surface.show()

    if Env.hasArg("swag.test") do
        g_Page = 0

    app.run()
}
