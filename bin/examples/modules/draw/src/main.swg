#global if #os == Swag.TargetOs.Windows
using Core, Win32, Ogl, Gdi32, Pixel, Gui, Math

const W = 1024
const H = 1024

namespace Res
{
    var image0:     Texture
    var image1:     Texture
    var image2:     Texture
    var typeface:   const *TypeFace
    var fontSmall:  *Font
    var font40:     *Font
    var font80:     *Font
    var fontFam:    FontFamily
}

var g_Painter:      Painter
var g_Renderer:     RenderOglWin32
var g_ModeAA:       bool = true
var g_PaintQuality: PaintQuality = Normal
var g_Pattern:      bool = true
var g_Page:         s32 = 0
var g_Wnd:          Surface
const MaxPages = 6

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

func displayPage0()
{
    pen := Pen.createSolid(Argb.Black, 20)

    if g_Pattern
        pen.dashStyle = .Dot

    //////////////////////////////////////
    g_Painter.begin()

    {
        g_Painter.resetTransform()
        g_Painter.translateTransform(50, 50)
        pen.brush.color = Argb.Black
        pen.beginCapStyle = .None
        pen.endCapStyle = .None
        g_Painter.drawLine(0, 0, 0, 150, pen)

        pen.beginCapStyle = .Square
        pen.endCapStyle = .Square
        g_Painter.drawLine(100, 0, 150, 150, pen)

        pen.beginCapStyle = .Round
        pen.endCapStyle = .Round
        g_Painter.drawLine(200, 0, 250, 150, pen)

        pen.beginCapStyle = .ArrowAnchor
        pen.endCapStyle = .ArrowAnchor
        pen.dashBeginCapStyle = .Round
        pen.dashEndCapStyle = .Round
        g_Painter.drawLine(300, 0, 350, 150, pen)

        pen.beginCapStyle = .SquareAnchor
        pen.endCapStyle = .SquareAnchor
        pen.dashBeginCapStyle = .Triangle
        pen.dashEndCapStyle = .Triangle
        pen.brush.color = Color.fromArgb(0x5f, Argb.Black)
        g_Painter.drawLine(400, 0, 450, 150, pen)
    }

    g_Painter.drawLine(500, 0, 550, 150, Argb.Brown, 2)

    pen.dashBeginCapStyle = .Round
    pen.dashEndCapStyle = .Round
    g_Painter.resetTransform()
    g_Painter.translateTransform(50, 250)
    pen.brush.color = Argb.Blue
    pen.joinStyle = .None
    g_Painter.drawRect(0, 0, 100, 100, pen)
    pen.joinStyle = .Bevel
    g_Painter.drawRect(150, 0, 100, 100, pen)
    pen.joinStyle = .Round
    g_Painter.drawRect(300, 0, 100, 100, pen)
    pen.joinStyle = .Miter
    g_Painter.drawRect(450, 0, 100, 100, pen)

    pen.brush.color = Argb.Red
    var path: LinePath
    path.lineTo(100, 0)
    path.lineTo(100, 50)
    path.lineTo(200, 50)
    pen.joinStyle = .Miter

    g_Painter.resetTransform()
    g_Painter.translateTransform(50, 400)
    g_Painter.drawPath(&path, pen)
    pen.joinStyle = .Bevel

    g_Painter.resetTransform()
    g_Painter.translateTransform(250, 400)
    g_Painter.drawPath(&path, pen)
    pen.joinStyle = .Round

    g_Painter.resetTransform()
    g_Painter.translateTransform(450, 400)
    g_Painter.drawPath(&path, pen)

    pen.brush.color = Argb.Green
    path.start(0, 0)
    path.lineTo(100, 0)
    path.lineTo(100, 50)
    path.lineTo(0, 50)
    pen.beginCapStyle = .Round
    pen.endCapStyle = .Round

    g_Painter.resetTransform()
    g_Painter.translateTransform(50, 500)
    g_Painter.drawPath(&path, pen)

    path.start(0, 0)
    path.bezierTo(100, 0, 50, -50)
    g_Painter.resetTransform()
    g_Painter.translateTransform(250, 500)
    g_Painter.drawPath(&path, pen)

    path.start(0, 0)
    path.lineTo(50, 0)
    path.bezierTo(100, 50, 100, 0, 50, 50)
    path.lineTo(200, 50)
    g_Painter.resetTransform()
    g_Painter.translateTransform(400, 500)
    g_Painter.drawPath(&path, pen)

    path.start(0, 0)
    pen.brush.color = Argb.Purple
    path.arcTo(0, 50, 50, 50, 0, 90)
    path.lineTo(100, 50)
    path.arcTo(150, 50, 50, 50, -90, 0)

    g_Painter.resetTransform()
    g_Painter.translateTransform(50, 600)
    g_Painter.drawPath(&path, pen)

    g_Painter.resetTransform()
    g_Painter.translateTransform(250, 600)
    pen = Pen.createSolid(Argb.Blue, 10)
    g_Painter.drawRoundRect(0, 0, 100, 100, 20, 20, pen)

    g_Painter.resetTransform()
    g_Painter.translateTransform(400, 600)
    pen.size = 4
    g_Painter.drawCircle(50, 50, 50, pen)

    g_Painter.resetTransform()
    g_Painter.translateTransform(500, 600)
    g_Painter.drawEllipse(50, 50, 25, 50, pen)

    brush := Brush.createSolid(Argb.Black)
    g_Painter.resetTransform()
    g_Painter.translateTransform(600, 600)
    g_Painter.fillCircle(50, 50, 50, brush)
    g_Painter.end()
    g_Renderer.draw(&g_Painter)

    //////////////////////////////////////
    g_Painter.begin()
    path.start(0, 50)
    path.lineTo(50, 50)
    path.lineTo(50, 20)
    path.lineTo(100, 75)
    path.lineTo(50, 130)
    path.lineTo(50, 100)
    path.lineTo(0, 100)
    path.close()
    brush.color = Argb.Red
    g_Painter.fillPath(&path, brush)
    path.polygon.offset(-10)
    brush.color = Argb.Blue
    g_Painter.fillPath(&path, brush)
    g_Painter.end()

    g_Renderer.resetTransform()
    g_Renderer.translateTransform(625, 50)
    g_Renderer.draw(&g_Painter)

    //////////////////////////////////////
    g_Painter.begin()
    brush = Brush.createTiled(Res.image0)
    g_Painter.fillRect(750, 50, 100, 150, brush)
    brush = Brush.createTexture(Res.image1)
    g_Painter.fillRoundRect(750, 250, 150, 150, 20, 20, brush)
    g_Painter.fillCircle(850, 450, 50, brush)
    g_Painter.end()
    g_Renderer.resetTransform()
    g_Renderer.draw(&g_Painter)

    //////////////////////////////////////
    g_Painter.begin()
    var lpl: LinePathList
    g_Painter.translateTransform(750, 550)
    p0 := lpl.newPath()
    p0.setRect(0, 0, 100, 100)
    p1 := lpl.newPath()
    p1.setCircle(50, 160, 50)
    g_Painter.fillPath(&lpl, Argb.Red)
    g_Painter.end()

    g_Renderer.draw(&g_Painter)
}

func displayPage1()
{
    var brush: Brush
    brush.color = Argb.Black

    g_Painter.begin()
    g_Painter.translateTransform(50, 50)

    cpt, cptLines := 0
    visit e: HatchStyle
    {
        brush.type   = .Hatch
        brush.hatch  = e
        brush.uvMode = .Fill
        g_Painter.fillRect(0, 0, 48, 48, brush)
        brush.uvMode = .Tile
        g_Painter.fillRect(0, 75, 48, 48, brush)

        g_Painter.translateTransform(75, 0)
        cpt += 1
        if cpt == 12
        {
            cptLines += 1
            cpt = 0
            g_Painter.resetTransform()
            g_Painter.translateTransform(50, 50 + 150.0 * cptLines)
        }
    }

    g_Painter.end()

    g_Renderer.resetTransform()
    g_Renderer.draw(&g_Painter)
}

func displayPage2()
{
    g_Painter.begin()

    g_Painter.drawTexture(200, 0, Res.image1)

    g_Painter.setInterpolationMode(InterpolationMode.Pixel)
    g_Painter.drawTexture(50, 0, 96, 96, Res.image0)
    g_Painter.setInterpolationMode(InterpolationMode.Linear)
    g_Painter.drawTexture(50, 110, 96, 96, Res.image0)

    g_Painter.rotateTransform(45, 100, 300)
    g_Painter.drawTexture(50, 250, 96, 96, Res.image0)

    g_Painter.resetTransform()
    pen := Pen.createHatch(HatchStyle.SolidDiamond, Argb.Black, 20)
    g_Painter.drawRoundRect(400, 700, 200, 200, 10, 10, pen)

    g_Painter.drawTexture(50, 500, 150, 150, @{228, 103, 206, 167}, Res.image1)
    g_Painter.drawTexture(220, 500, 150, 150, @{228, 103, 206, 167}, Res.image2)
    g_Painter.drawTexture(50, 700, @{228, 103, 206, 167}, Res.image1)

    g_Painter.end()

    g_Renderer.resetTransform()
    g_Renderer.draw(&g_Painter)
}

func displayPage3()
{
    g_Painter.begin()

    brush := Brush.createHatch(HatchStyle.SolidDiamond, Argb.Black)
    g_Painter.setClippingRect(0, 0, 100, 100)
    g_Painter.fillCircle(100, 100, 100, brush)
    g_Painter.drawCircle(100, 100, 100, Argb.Red, 4)
    g_Painter.resetClippingRect()

    g_Painter.startClippingRegion(ClippingMode.Set)
    g_Painter.fillRoundRect(150, 0, 100, 100, 20, 20, Argb.Green)
    g_Painter.setClippingRegionMode(ClippingMode.Clear)
    g_Painter.fillCircle(200, 50, 50, Argb.White)
    g_Painter.endClippingRegion()

    g_Painter.fillRect(0, 0, 1000, 1000, Argb.Red)
    g_Painter.resetClippingRegion()

    {
        g_Painter.drawCircle(500, 500, 400, Argb.Red, 1)
    }

    {
        g_Painter.drawRoundRect(500, 500, 200, 200, 10, 10, Color.fromArgb(0x51, Argb.Blue), 15)
    }

    {
        var lp1: LinePath
        lp1.start(800, 800)
        lp1.bezierTo(150, 150, 250, 50)
        g_Painter.drawPath(&lp1, Pen.createSolid(Color.fromArgb(0x51, Argb.Red), 50))
    }

    {
        var lp: LinePath
        lp.start(50, 800)
        lp.bezierTo(800, 50, 250, 50)
        g_Painter.drawPath(&lp, Pen.createSolid(Color.fromArgb(0x51, Argb.Black), 50))
    }

    {
        var lp1: LinePath
        lp1.start(400, 400)
        lp1.lineTo(500, 450)
        lp1.lineTo(400, 500)
        g_Painter.drawPath(&lp1, Pen.createSolid(Color.fromArgb(0x51, Argb.Blue), 50))
    }

    {
        g_Painter.resetTransform()
        g_Painter.translateTransform(400, 200)
        g_Painter.setBlendingMode(BlendingMode.Add)
        g_Painter.fillRect(0, 0, 150, 150, Argb.Blue)
        g_Painter.setBlendingMode(BlendingMode.Max)
        g_Painter.fillRect(50, 50, 150, 150, Argb.Red)
        g_Painter.setBlendingMode(BlendingMode.Alpha)
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
        g_Painter.resetTransform()
        g_Painter.translateTransform(700, 200)
        g_Painter.drawPathList(&lp1, pen, DrawPathListMode.Separate)

        g_Painter.translateTransform(0, 150)
        g_Painter.drawPathList(&lp1, pen, DrawPathListMode.Merge)
        g_Painter.translateTransform(150, 0)
        pen.brush = Brush.createTexture(Res.image1)
        g_Painter.drawPathList(&lp1, pen, DrawPathListMode.Merge)

        g_Painter.translateTransform(0, 150)
        pen = Pen.createSolid(Color.fromArgb(0x51, Argb.Green), 15)
        pen.dashStyle = .Dash
        g_Painter.drawPathList(&lp1, pen, DrawPathListMode.Merge)
    }

    g_Painter.end()
    g_Renderer.resetTransform()
    g_Renderer.draw(&g_Painter)
}

func displayPage4()
{
    g_Painter.begin()

    {
        var format: StringFormat
        var rect:   Rectangle{350, 50, 100, 100}
        format.horzAlignment = .Center
        format.vertAlignment = .Center
        format.lineGap = 1
        g_Painter.fillRect(rect.x, rect.y, rect.width, rect.height, Color.fromArgb(0x1F, Argb.Blue))
        g_Painter.drawString(rect, "on multiple lines\nthis is\na text\non multiple lines", Res.fontSmall, Argb.Black, format)
    }

    {
        var format: StringFormat
        var rect:   Rectangle{500, 50, 100, 100}
        format.flags |= .WordWrap
        format.underline = .Underline
        format.lineGap = 1
        g_Painter.fillRect(rect.x, rect.y, rect.width, rect.height, Color.fromArgb(0x1F, Argb.Blue))
        g_Painter.drawString(rect, "on multiple lines this is a text on multiple lines", Res.fontSmall, Argb.Black, format)
    }

    {
        format := StringFormat{flags: StringFormatFlags.WordWrap}
        format.horzAlignment = .Center
        rect := Rectangle{650, 50, 200, 700}
        g_Painter.fillRect(rect.x, rect.y, rect.width, rect.height, Argb.White)
        const TXT = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Sed non risus. Suspendisse lectus tortor, dignissim sit amet, adipiscing nec, ultricies sed, dolor. Cras elementum ultrices diam. Maecenas ligula massa, varius a, semper congue, euismod non, mi. Proin porttitor, orci nec nonummy molestie, enim est eleifend mi, non fermentum diam nisl sit amet erat. Duis semper. Duis arcu massa, scelerisque vitae, consequat in, pretium a, enim. Pellentesque congue. Ut in risus volutpat libero pharetra tempor. Cras vestibulum bibendum augue. Praesent egestas leo in pede. Praesent blandit odio eu enim. Pellentesque sed dui ut augue blandit sodales. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Aliquam nibh. Mauris ac mauris sed pede pellentesque fermentum. Maecenas adipiscing ante non diam sodales hendrerit."
        g_Painter.drawString(rect, TXT, Res.fontSmall, Argb.Black, format)
    }

    {
        g_Painter.drawString(100, 200, "this is a TEXT on a single line", Res.fontSmall, Argb.Black)
    }

    {
        fnt := Res.fontFam.getFont(FontFamilyStyle.Regular)
        g_Painter.fillCircle(100, 100, 5, Argb.Red)
        g_Painter.drawString(100, 100, "abcdefghijkl", fnt, Argb.Black)
    }

    const str = "abcdefghijklmnopqrstuvwxyz"

    {
        brush := Brush.createTexture(Res.image1)
        rect := g_Painter.measureString(100, 300, str, Res.font40)
        g_Painter.fillRect(rect.left, rect.top, rect.width, rect.height, Color.fromArgb(0x1F, Argb.Blue))
        g_Painter.fillRect(rect.left, rect.top, rect.width, rect.baseLine, Color.fromArgb(0x1F, Argb.Red))
        g_Painter.drawString(100, 300, str, Res.font40, brush, UnderlineStyle.Strikeout)
        g_Painter.drawString(100, 350, str, Res.font40, Argb.Black)
    }

    {
        var pathl: LinePathList
        g_Painter.translateTransform(50, 470)

        pen := Pen.createSolid(Argb.Red, 5)
        pen.joinStyle = .Round
        pen.plotOffset = 2

        assume Res.typeface.getStringOutline(&pathl, str, 70)
        g_Painter.drawPathList(&pathl, pen)
        g_Painter.fillPath(&pathl, Argb.Black)

        g_Painter.resetTransform()
    }

    {
        var p: LinePathList
        g_Painter.translateTransform(50, 570)
        assume Res.typeface.getStringOutline(&p, "A B C D", 70)
        p.flatten(g_PaintQuality)
        p.offset(4)
        p.clean()
        g_Painter.drawPathList(&p, Argb.Red, 2)

        g_Painter.resetTransform()
    }

    {
        var p: LinePathList
        g_Painter.translateTransform(50, 670)
        assume Res.typeface.getStringOutline(&p, "E F G H", 70)
        p.flatten(g_PaintQuality)
        p.offset(4, JoinStyle.Round)
        p.clean()
        g_Painter.fillPath(&p, Argb.Red)
        g_Painter.drawPathList(&p, Argb.Black, 2)

        g_Painter.resetTransform()
    }

    {
        var p: LinePath
        g_Painter.translateTransform(500, 570)
        p.start(0, 0)
        p.lineTo(100, 0)
        p.lineTo(100, 100)
        p.lineTo(0, 100)
        p.close()
        p.flatten(g_PaintQuality)
        g_Painter.drawPath(&p, Argb.Red, 2)
        p.polygon.offset(10, JoinStyle.Miter)
        g_Painter.drawPath(&p, Argb.Green, 2)
        p.polygon.offset(10, JoinStyle.Bevel)
        g_Painter.drawPath(&p, Argb.Green, 2)
        p.polygon.offset(10, JoinStyle.Round)
        g_Painter.drawPath(&p, Argb.Green, 4)

        g_Painter.resetTransform()
    }

    // Draw font page
    //g_Painter.drawTexture(100, 400, font80.atlases[0].texture)

    g_Painter.end()
    g_Renderer.resetTransform()
    g_Renderer.draw(&g_Painter)
}

func displayPage5()
{
    g_Painter.begin()

    {
        g_Painter.resetTransform()
        g_Painter.translateTransform(500, 300)

        var lp: LinePath
        lp.start(0, 100)
        lp.curveTo(50, 80)
        lp.curveTo(100, 20)
        lp.curveTo(150, 80)
        lp.curveTo(200, 100)
        g_Painter.drawPath(&lp, Pen.createSolid(Argb.Black, 2))
    }

    {
        g_Painter.resetTransform()
        g_Painter.translateTransform(500, 400)

        var lp: LinePath
        lp.start(20, 50)
        lp.curveTo(100, 10)
        lp.curveTo(200, 100)
        lp.curveTo(300, 50)
        lp.curveTo(400, 80)
        g_Painter.drawPath(&lp, Pen.createSolid(Argb.Black, 2))
    }

    {
        g_Painter.resetTransform()
        g_Painter.translateTransform(500, 500)

        var lp: LinePath
        lp.start(60, 60)
        lp.curveTo(150, 80)
        lp.curveTo(200, 40)
        lp.curveTo(180, 120)
        lp.curveTo(120, 100)
        lp.curveTo(80, 160)
        lp.curveTo(40, 140)
        lp.close()
        g_Painter.fillPath(&lp, Argb.Blue)
        g_Painter.drawPath(&lp, Pen.createSolid(Argb.Red, 4))
    }

    g_Painter.end()
    g_Renderer.resetTransform()
    g_Renderer.draw(&g_Painter)
}

func display()
{
    g_Renderer.begin()

    g_Painter.paintParams  = g_ModeAA ? .Antialiased : .Default
    g_Painter.paintQuality = g_PaintQuality

    g_Painter.begin()
    g_Painter.clear(Color.fromArgb(0xFFEEEEEE))
    g_Painter.end()
    g_Renderer.draw(&g_Painter)

    switch g_Page
    {
    case 0: displayPage0()
    case 1: displayPage1()
    case 2: displayPage2()
    case 3: displayPage3()
    case 4: displayPage4()
    case 5: displayPage5()
    }

    g_Painter.begin()
    g_Painter.translateTransform(680, 780)
    g_Painter.drawString(0, 00, "F1: AA on/off", Res.font40, Argb.Black)
    g_Painter.drawString(0, 40, "F2: Quality", Res.font40, Argb.Black)
    g_Painter.drawString(0, 80, "F3: Pattern", Res.font40, Argb.Black)
    g_Painter.drawString(0, 120, "F4: Change page", Res.font40, Argb.Black)
    g_Painter.end()
    g_Renderer.draw(&g_Painter)

    g_Renderer.end()
}

func messageLoop()
{
    var keyb: Input.Keyboard

    loop
    {
        if !Surface.messageLoop()
            return

        Memory.freeTemp()
        display()

        keyb.update()
        if keyb.isKeyJustPressed(Input.Key.F1)
            g_ModeAA = g_ModeAA ? false : true
        if keyb.isKeyJustPressed(Input.Key.F2)
            g_PaintQuality = cast(PaintQuality) ((cast(u32) g_PaintQuality + 1) % 2)
        if keyb.isKeyJustPressed(Input.Key.F3)
            g_Pattern = g_Pattern ? false : true
        if keyb.isKeyJustPressed(Input.Key.F4)
            g_Page = (g_Page + 1) % MaxPages
    }
}

func winProc(wnd: Surface, msg: SurfaceMessage)
{
    switch msg.kind
    {
    case Paint:
        display()
    case Resize:
        g_Renderer.resize(cast(s32) msg.param0, cast(s32) msg.param1)
    }
}

#main
{
    try
    {
        g_Wnd = Surface.create("Example", W, H, SurfaceFlags.OverlappedWindow, &winProc)
        g_Renderer.init(g_Wnd.hWnd, W, H)

        img0 := Image.load(imagePath("basn0g01.png"))
        img1 := Image.load(imagePath("rgba.bmp"))
        Res.image0 = g_Renderer.addImage(img0)
        Res.image1 = g_Renderer.addImage(img1)
        Res.image2 = g_Renderer.addImage(img1)
        img1.grayScale()
        img1.setAlpha(128)
        g_Renderer.updateTexture(Res.image2, img1.pixels)

        Res.typeface = TypeFace.create(imagePath("arial.ttf"))
        Res.fontSmall = Font.create(Res.typeface, cast(u32) ((12 * 96) / 72))
        Res.font40 = Font.create(Res.typeface, 40)
        Res.font80 = Font.create(Res.typeface, 80)

        Res.fontFam.regular = Font.create(imagePath("arial.ttf"), 30)
        Res.fontFam.bold = Font.create(imagePath("arialbd.ttf"), 30)
        Res.fontFam.italic = Font.create(imagePath("ariali.ttf"), 30)
        Res.fontFam.boldItalic = Font.create(imagePath("arialbi.ttf"), 30)

        g_Wnd.show()
        messageLoop()

        g_Renderer.drop()
    }
}