#global if #os == Swag.TargetOs.Windows
using Core, Pixel, Gui, Math

const W = 1024
const H = 1024

namespace Res
{
    var image0:         Texture
    var image1:         Texture
    var image2:         Texture
    var typeface:       const *TypeFace
    var fontDefault:    *Font
    var font40:         *Font
    var font80:         *Font
    var fontFam:        FontFamily
}

var g_ModeAA:       bool = true
var g_PaintQuality: PaintQuality = Normal
var g_Pattern:      bool = true
var g_Page:         s32 = 6
const MaxPages = 7

func dataPath()->String
{
    dataPath := Path.getDirectoryName(#location.fileName)
    dataPath = Path.getDirectoryName(dataPath)
    dataPath = Path.getDirectoryName(dataPath)
    dataPath = Path.getDirectoryName(dataPath)
    dataPath = Path.getDirectoryName(dataPath)
    return Path.combine(dataPath, "std/tests/pixel/datas")
}

func imagePath(name: string)->String
{
    return Path.combine(dataPath(), name)
}

func displayPage0(painter: *Pixel.Painter)
{
    pen := Pen.createSolid(Argb.Black, 20)

    if g_Pattern
        pen.dashStyle = .Dot

    //////////////////////////////////////
    {
        painter.resetTransform()
        painter.translateTransform(50, 50)
        pen.brush.color = Argb.Black
        pen.beginCapStyle = .None
        pen.endCapStyle = .None
        painter.drawLine(0, 0, 0, 150, pen)

        pen.beginCapStyle = .Square
        pen.endCapStyle = .Square
        painter.drawLine(100, 0, 150, 150, pen)

        pen.beginCapStyle = .Round
        pen.endCapStyle = .Round
        painter.drawLine(200, 0, 250, 150, pen)

        pen.beginCapStyle = .ArrowAnchor
        pen.endCapStyle = .ArrowAnchor
        pen.dashBeginCapStyle = .Round
        pen.dashEndCapStyle = .Round
        painter.drawLine(300, 0, 350, 150, pen)

        pen.beginCapStyle = .SquareAnchor
        pen.endCapStyle = .SquareAnchor
        pen.dashBeginCapStyle = .Triangle
        pen.dashEndCapStyle = .Triangle
        pen.brush.color = Color.fromArgb(0x5f, Argb.Black)
        painter.drawLine(400, 0, 450, 150, pen)
    }

    painter.drawLine(500, 0, 550, 150, Argb.Brown, 2)

    pen.dashBeginCapStyle = .Round
    pen.dashEndCapStyle = .Round
    painter.resetTransform()
    painter.translateTransform(50, 250)
    pen.brush.color = Argb.Blue
    pen.joinStyle = .None
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

    painter.resetTransform()
    painter.translateTransform(50, 400)
    painter.drawPath(&path, pen)
    pen.joinStyle = .Bevel

    painter.resetTransform()
    painter.translateTransform(250, 400)
    painter.drawPath(&path, pen)
    pen.joinStyle = .Round

    painter.resetTransform()
    painter.translateTransform(450, 400)
    painter.drawPath(&path, pen)

    pen.brush.color = Argb.Green
    path.start(0, 0)
    path.lineTo(100, 0)
    path.lineTo(100, 50)
    path.lineTo(0, 50)
    pen.beginCapStyle = .Round
    pen.endCapStyle = .Round

    painter.resetTransform()
    painter.translateTransform(50, 500)
    painter.drawPath(&path, pen)

    path.start(0, 0)
    path.bezierTo(100, 0, 50, -50)
    painter.resetTransform()
    painter.translateTransform(250, 500)
    painter.drawPath(&path, pen)

    path.start(0, 0)
    path.lineTo(50, 0)
    path.bezierTo(100, 50, 100, 0, 50, 50)
    path.lineTo(200, 50)
    painter.resetTransform()
    painter.translateTransform(400, 500)
    painter.drawPath(&path, pen)

    path.start(0, 0)
    pen.brush.color = Argb.Purple
    path.arcTo(0, 50, 50, 50, 0, 90)
    path.lineTo(100, 50)
    path.arcTo(150, 50, 50, 50, -90, 0)

    painter.resetTransform()
    painter.translateTransform(50, 600)
    painter.drawPath(&path, pen)

    painter.resetTransform()
    painter.translateTransform(250, 600)
    pen = Pen.createSolid(Argb.Blue, 10)
    painter.drawRoundRect(0, 0, 100, 100, 20, 20, pen)

    painter.resetTransform()
    painter.translateTransform(400, 600)
    pen.size = 4
    painter.drawCircle(50, 50, 50, pen)

    painter.resetTransform()
    painter.translateTransform(500, 600)
    painter.drawEllipse(50, 50, 25, 50, pen)

    brush := Brush.createSolid(Argb.Black)
    painter.resetTransform()
    painter.translateTransform(600, 600)
    painter.fillCircle(50, 50, 50, brush)

    //////////////////////////////////////
    painter.resetState()
    path.start(0, 50)
    path.lineTo(50, 50)
    path.lineTo(50, 20)
    path.lineTo(100, 75)
    path.lineTo(50, 130)
    path.lineTo(50, 100)
    path.lineTo(0, 100)
    path.close()
    brush.color = Argb.Red
    painter.resetTransform()
    painter.translateTransform(10, 750)
    painter.fillPath(&path, brush)
    path.polygon.offset(-10)
    brush.color = Argb.Blue
    painter.fillPath(&path, brush)

    //////////////////////////////////////
    painter.resetTransform()
    brush = Brush.createTiled(Res.image0)
    painter.fillRect(750, 50, 100, 150, brush)
    brush = Brush.createTexture(Res.image1)
    painter.fillRoundRect(750, 250, 150, 150, 20, 20, brush)
    painter.fillCircle(850, 450, 50, brush)

    //////////////////////////////////////
    var lpl: LinePathList
    painter.resetTransform()
    painter.translateTransform(750, 550)
    p0 := lpl.newPath()
    p0.setRect(0, 0, 100, 100)
    p1 := lpl.newPath()
    p1.setCircle(50, 160, 50)
    painter.fillPath(&lpl, Argb.Red)
}

func displayPage1(painter: *Pixel.Painter)
{
    var brush: Brush
    brush.color = Argb.Black

    painter.translateTransform(50, 50)

    cpt, cptLines := 0
    visit e: HatchStyle
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
            painter.resetTransform()
            painter.translateTransform(50, 50 + 150.0 * cptLines)
        }
    }
}

func displayPage2(painter: *Pixel.Painter)
{
    painter.drawTexture(200, 0, Res.image1)

    painter.setInterpolationMode(InterpolationMode.Pixel)
    painter.drawTexture(50, 0, 96, 96, Res.image0)
    painter.setInterpolationMode(InterpolationMode.Linear)
    painter.drawTexture(50, 110, 96, 96, Res.image0)

    painter.rotateTransform(45, 100, 300)
    painter.drawTexture(50, 250, 96, 96, Res.image0)

    painter.resetTransform()
    pen := Pen.createHatch(HatchStyle.SolidDiamond, Argb.Black, 20)
    painter.drawRoundRect(400, 700, 200, 200, 10, 10, pen)

    painter.drawTexture(50, 500, 150, 150, @{228, 103, 206, 167}, Res.image1)
    painter.drawTexture(220, 500, 150, 150, @{228, 103, 206, 167}, Res.image2)
    painter.drawTexture(50, 700, @{228, 103, 206, 167}, Res.image1)
}

func displayPage3(painter: *Pixel.Painter)
{
    brush := Brush.createHatch(HatchStyle.SolidDiamond, Argb.Black)
    painter.setClippingRect(0, 0, 100, 100)
    painter.fillCircle(100, 100, 100, brush)
    painter.drawCircle(100, 100, 100, Argb.Red, 4)
    painter.resetClippingRect()

    painter.startClippingRegion(ClippingMode.Set)
    painter.fillRoundRect(150, 0, 100, 100, 20, 20, Argb.Green)
    painter.setClippingRegionMode(ClippingMode.Clear)
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
        painter.resetTransform()
        painter.translateTransform(400, 200)
        painter.setBlendingMode(BlendingMode.Add)
        painter.fillRect(0, 0, 150, 150, Argb.Blue)
        painter.setBlendingMode(BlendingMode.Max)
        painter.fillRect(50, 50, 150, 150, Argb.Red)
        painter.setBlendingMode(BlendingMode.Alpha)
    }

    {
        var lp1: LinePathList
        lp := lp1.newPath()
        lp.start(50, 0)
        lp.lineTo(50, 100)
        lp = lp1.newPath()
        lp.start(0, 50)
        lp.lineTo(100, 50)

        pen := Pen.createSolid(Color.fromArgb(0x51, Argb.Green), 50)
        painter.resetTransform()
        painter.translateTransform(700, 200)
        painter.drawPathList(&lp1, pen, DrawPathListMode.Separate)

        painter.translateTransform(0, 150)
        painter.drawPathList(&lp1, pen, DrawPathListMode.Merge)
        painter.translateTransform(150, 0)
        pen.brush = Brush.createTexture(Res.image1)
        painter.drawPathList(&lp1, pen, DrawPathListMode.Merge)

        painter.translateTransform(0, 150)
        pen = Pen.createSolid(Color.fromArgb(0x51, Argb.Green), 15)
        pen.dashStyle = .Dash
        painter.drawPathList(&lp1, pen, DrawPathListMode.Merge)
    }
}

func displayPage4(painter: *Pixel.Painter)
{
    {
        var format: StringFormat
        var rect:   Rectangle{350, 50, 100, 100}
        format.horzAlignment = .Center
        format.vertAlignment = .Center
        format.lineGap = 1
        painter.fillRect(rect.x, rect.y, rect.width, rect.height, Color.fromArgb(0x1F, Argb.Blue))
        painter.drawString(rect, "on multiple lines\nthis is\na text\non multiple lines", Res.fontDefault, Argb.Black, format)
    }

    {
        var format: StringFormat
        var rect:   Rectangle{500, 50, 100, 100}
        format.flags |= .WordWrap
        format.underline = .Underline
        format.lineGap = 1
        painter.fillRect(rect.x, rect.y, rect.width, rect.height, Color.fromArgb(0x1F, Argb.Blue))
        painter.drawString(rect, "on multiple lines this is a text on multiple lines", Res.fontDefault, Argb.Black, format)
    }

    {
        format := StringFormat{flags: StringFormatFlags.WordWrap}
        format.horzAlignment = .Center
        rect := Rectangle{650, 50, 200, 700}
        painter.fillRect(rect.x, rect.y, rect.width, rect.height, Argb.White)
        const TXT = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus. Suspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor. Cras elementum ultrices diam. Maecenas ligula massa, varius a, semper congue, euismod non, mi. Proin porttitor, orci nec nonummy molestie, enim est eleifend mi, non fermentum diam nisl sit amet erat. Duis semper. Duis arcu massa, scelerisque vitae, consequat in, pretium a, enim. Pellentesque congue. Ut in risus volutpat libero pharetra tempor. Cras vestibulum bibendum augue. Praesent egestas leo in pede. Praesent blandit odio eu enim. Pellentesque sed dui ut augue blandit sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Aliquam nibh. Mauris ac mauris sed pede pellentesque fermentum. Maecenas adipiscing ante non diam sodales hendrerit."
        painter.drawString(rect, TXT, Res.fontDefault, Argb.Black, format)
    }

    {
        painter.drawString(100, 200, "this is a TEXT on a single line", Res.fontDefault, Argb.Black)
    }

    {
        fnt := Res.fontFam.getFont(FontFamilyStyle.Regular)
        painter.fillCircle(100, 100, 5, Argb.Red)
        painter.drawString(100, 100, "abcdefghijkl", fnt, Argb.Black)
    }

    const str = "abcdefghijklmnopqrstuvwxyz"

    {
        brush := Brush.createTexture(Res.image1)
        rect := painter.measureString(100, 300, str, Res.font40)
        painter.fillRect(rect.left, rect.top, rect.width, rect.height, Color.fromArgb(0x1F, Argb.Blue))
        painter.fillRect(rect.left, rect.top, rect.width, rect.baseLine, Color.fromArgb(0x1F, Argb.Red))
        painter.drawString(100, 300, str, Res.font40, brush, UnderlineStyle.Strikeout)
        painter.drawString(100, 350, str, Res.font40, Argb.Black)
    }

    {
        var pathl: LinePathList
        painter.translateTransform(50, 470)

        pen := Pen.createSolid(Argb.Red, 5)
        pen.joinStyle = .Round
        pen.plotOffset = 2

        assume Res.typeface.getStringOutline(&pathl, str, 70)
        painter.drawPathList(&pathl, pen)
        painter.fillPath(&pathl, Argb.Black)

        painter.resetTransform()
    }

    {
        var p: LinePathList
        painter.translateTransform(50, 570)
        assume Res.typeface.getStringOutline(&p, "A B C D", 70)
        p.flatten(g_PaintQuality)
        p.offset(4)
        p.clean()
        painter.drawPathList(&p, Argb.Red, 2)

        painter.resetTransform()
    }

    {
        var p: LinePathList
        painter.translateTransform(50, 670)
        assume Res.typeface.getStringOutline(&p, "E F G H", 70)
        p.flatten(g_PaintQuality)
        p.offset(4, JoinStyle.Round)
        p.clean()
        painter.fillPath(&p, Argb.Red)
        painter.drawPathList(&p, Argb.Black, 2)

        painter.resetTransform()
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

        painter.resetTransform()
    }

    // Draw font page
    //painter.drawTexture(100, 400, font80.atlases[0].texture)
}

func displayPage5(painter: *Pixel.Painter)
{
    {
        painter.resetTransform()
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
        painter.resetTransform()
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
        painter.resetTransform()
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
}

func displayPage6(painter: *Pixel.Painter)
{
    var str: RichString

    str.set("<u>normal<u><i>italic<b>bold<b><i> another <p1>one<p1> <p2><u>two<u><p2>")
    str.format.font = Res.fontFam
    str.format.palette[0] = Argb.Black
    str.format.palette[1] = Argb.Red
    str.format.palette[2] = Argb.Green
    painter.drawRichString(@{10, 10, 100, 100}, &str)
}

func onPaintEvent(wnd: *Wnd, event: *PaintEvent)->bool
{
    painter := event.bc.painter
    painter.paintParams = g_ModeAA ? .Antialiased : .Default
    painter.paintQuality = g_PaintQuality

    painter.clear(Color.fromArgb(0xFFEEEEEE))

    switch g_Page
    {
    case 0: displayPage0(painter)
    case 1: displayPage1(painter)
    case 2: displayPage2(painter)
    case 3: displayPage3(painter)
    case 4: displayPage4(painter)
    case 5: displayPage5(painter)
    case 6: displayPage6(painter)
    }

    painter.resetState()
    painter.translateTransform(680, 780)
    painter.drawString(0, 00, "F1: AA on/off", Res.font40, Argb.Black)
    painter.drawString(0, 40, "F2: Quality", Res.font40, Argb.Black)
    painter.drawString(0, 80, "F3: Pattern", Res.font40, Argb.Black)
    painter.drawString(0, 120, "F4: Change page", Res.font40, Argb.Black)

    return true
}

func onKeyEvent(wnd: *Wnd, evt: *KeyEvent)->bool
{
    if evt.kind != .KeyPressed
        return false

    switch evt.key
    {
    case F1:
        g_ModeAA = g_ModeAA ? false : true
        wnd.surface.invalidate()
        return true

    case F2:
        g_PaintQuality = cast(PaintQuality) ((cast(u32) g_PaintQuality + 1) % 2)
        wnd.surface.invalidate()
        return true

    case F3:
        g_Pattern = g_Pattern ? false : true
        wnd.surface.invalidate()
        return true

    case F4:
        if evt.modifiers & .Alt
            wnd.destroy()
        else
        {
            g_Page = (g_Page + 1) % MaxPages
            wnd.surface.invalidate()
        }
        return true
    }

    return false
}

func setup(wnd: *Wnd) throw
{
    try
    {
        rdr := wnd.surface.app.getRenderer()
        img0 := Image.load(imagePath("basn0g01.png"))
        img1 := Image.load(imagePath("rgba.bmp"))
        Res.image0 = rdr.addImage(img0)
        Res.image1 = rdr.addImage(img1)
        Res.image2 = rdr.addImage(img1)
        img1.grayScale()
        img1.setAlpha(128)
        rdr.updateTexture(Res.image2, img1.pixels)

        Res.typeface = TypeFace.load(imagePath("arial.ttf"))
        Res.fontDefault = Font.create(Res.typeface, cast(u32) ((12 * 96) / 72))
        Res.font40 = Font.create(Res.typeface, 40)
        Res.font80 = Font.create(Res.typeface, 80)

        Res.fontFam.regular = Font.create(imagePath("arial.ttf"), 30)
        Res.fontFam.bold = Font.create(imagePath("arialbd.ttf"), 30)
        Res.fontFam.italic = Font.create(imagePath("ariali.ttf"), 30)
        Res.fontFam.boldItalic = Font.create(imagePath("arialbi.ttf"), 30)
    }
}

#main
{
    var app: Gui.Application
    surface := try app.createSurface(100, 100, W, H)
    surface.wnd.hookOnKeyEvent = &onKeyEvent
    surface.wnd.view.hookOnPaintEvent = &onPaintEvent
    try setup(&surface.wnd)
    surface.show()
    app.run()
}