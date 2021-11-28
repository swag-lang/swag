#global public
using Core, Pixel

struct Slider
{
    using button:   Button
    value1:         f32 = 0.25
    value2:         f32 = 0.75
    isRange:        bool
    sigChanged:     func(*Slider)
}

impl IWnd for Slider
{
    mtd onPaintEvent(evt: *PaintEvent)
    {
        bc     := evt.bc
        theme   := getTheme()
        metrics := getThemeMetrics()
        colors  := getThemeColors()
        rects   := getThemeRects()

        var colorBkLeft:   Color = ?
        var colorBkMiddle: Color = ?
        var colorBkRight:  Color = ?
        var colorMark:     Color = ?

        switch
        {
        case isDisabled:
            colorBkLeft   = colors.slider_DisabledBk
            colorBkMiddle = colors.slider_DisabledBk
            colorBkRight  = colors.slider_DisabledBk
            colorMark     = colors.slider_DisabledMark
        case isPressed:
            colorBkLeft   = colors.slider_HotBkLeft
            colorBkMiddle = colors.slider_HotBkMiddle
            colorBkRight  = colors.slider_HotBkRight
            colorMark     = colors.slider_PressedMark
        case isHot:
            colorBkLeft   = colors.slider_HotBkLeft
            colorBkMiddle = colors.slider_HotBkMiddle
            colorBkRight  = colors.slider_HotBkRight
            colorMark     = colors.slider_HotMark
        default:
            colorBkLeft   = colors.slider_BkLeft
            colorBkMiddle = colors.slider_BkMiddle
            colorBkRight  = colors.slider_BkRight
            colorMark     = colors.slider_Mark
        }

        // Pos box
        posBox1 := getPosBox1()
        posBox2 := getPosBox2()

        clientRect := getClientRect()
        clientRect.x += (posBox1.width * 0.5)
        clientRect.width -= posBox1.width

        // Bar left
        left := clientRect
        left.width = (clientRect.width * value1)
        theme.drawSubRect(bc.painter, left, &rects.slider_BkLeft, colorBkLeft)

        // Bar right
        valueRight := isRange ? value2 : value1
        right := clientRect
        right.x += (clientRect.width * valueRight)
        right.width -= (clientRect.width * valueRight)
        theme.drawSubRect(bc.painter, right, &rects.slider_BkRight, colorBkRight)

        // Bar middle
        if isRange
        {
            middle := clientRect
            middle.x = left.right()
            middle.setRight(right.x)
            theme.drawSubRect(bc.painter, middle, &rects.slider_BkMiddle, colorBkMiddle)
        }

        // Box value1
        theme.drawSubRect(bc.painter, posBox1, &rects.slider_Mark, colorMark)
        if isRange
        theme.drawSubRect(bc.painter, posBox2, &rects.slider_Mark, colorMark)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        if processMouseEvent(evt)
        {
            if sigChanged
                sigChanged(self)
        }
    }
}

private impl Slider
{
    mtd getPosBox1()->Math.Rectangle
    {
        var result: retval
        clientRect := getClientRect()
        result = clientRect
        result.x += (result.height * 0.5)
        result.width -= result.height
        result.x += result.width * value1
        result.x -= (clientRect.height * 0.5)

        result.y = result.vertCenter() - (result.height * 0.5)
        result.width = result.height
        return result
    }

    mtd getPosBox2()->Math.Rectangle
    {
        var result: retval
        clientRect := getClientRect()
        result = clientRect
        result.x += (result.height * 0.5)
        result.width -= result.height
        result.x += result.width * value2
        result.x -= (clientRect.height * 0.5)

        result.y = result.vertCenter() - (result.height * 0.5)
        result.width = result.height
        return result
    }
}

impl Slider
{
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null)->*Slider
    {
        pos := position
        if pos.height == 0 pos.height = parent.getThemeMetrics().slider_Height
        if pos.width == 0  pos.width  = parent.getThemeMetrics().slider_Width
        res := Wnd.create'Slider(parent, "", pos, id)
        evt := CreateEvent{}
        res.sendEvent(&evt)
        return res
    }
}