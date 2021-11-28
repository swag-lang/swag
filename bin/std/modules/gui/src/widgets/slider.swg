#global public
using Core, Pixel

struct Slider
{
    using wnd:      Wnd
    value1:         f32 = 0.25
    value2:         f32 = 0.75
    isRange:        bool
    sigChanged:     func(*Slider)

    isMoving:       bool
    isPressed1:     bool
    isPressed2:     bool
    isHot1:         bool
    isHot2:         bool
    correcMoving:   f32
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
        var colorMark1:    Color = ?
        var colorMark2:    Color = ?

        if isDisabled
        {
            colorBkLeft   = colors.slider_DisabledBk
            colorBkMiddle = colors.slider_DisabledBk
            colorBkRight  = colors.slider_DisabledBk
            colorMark1    = colors.slider_DisabledMark
            colorMark2    = colors.slider_DisabledMark
        }
        else
        {
            colorBkLeft   = colors.slider_BkLeft
            colorBkMiddle = colors.slider_BkMiddle
            colorBkRight  = colors.slider_BkRight

            if isPressed1
                colorMark1 = colors.slider_PressedMark
            else if isHot1
                colorMark1 = colors.slider_HotMark
            else
                colorMark1 = colors.slider_Mark

            if isPressed2
                colorMark2 = colors.slider_PressedMark
            else if isHot2
                colorMark2 = colors.slider_HotMark
            else
                colorMark2 = colors.slider_Mark

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
        theme.drawSubRect(bc.painter, posBox1, &rects.slider_Mark, colorMark1)
        if isRange
            theme.drawSubRect(bc.painter, posBox2, &rects.slider_Mark, colorMark2)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MouseLeave:
            isHot1, isHot2 = false
            invalidate()

        case MouseMove:
            if !isMoving
            {
                hitTest(evt.pos)
                break
            }

            clientRect := getOffsetedClientRect()
            diff := (evt.pos.x - correcMoving) - clientRect.x
            f    := diff / clientRect.width
            if isPressed1
            {
                value1 = Math.clamp(f, 0, 1)
            }
            else if isPressed2
            {
                value2 = Math.clamp(f, 0, 1)
            }

            invalidate()

        case MousePressed:
            if evt.button == .Left
            {
                if isHot1
                {
                    isPressed1, isMoving = true
                    posBox := getPosBox1()
                    correcMoving = evt.pos.x - posBox.horzCenter()
                }
                else if isHot2
                {
                    isPressed2, isMoving = true
                    posBox := getPosBox2()
                    correcMoving = evt.pos.x - posBox.horzCenter()
                }

                if isMoving
                {
                    captureMouse()
                    invalidate()
                }
            }

        case MouseReleased:
            if isMoving and evt.button == .Left
            {
                releaseMouse()
                isPressed1, isPressed2, isMoving = false
                hitTest(evt.pos)
                invalidate()
            }
        }
    }
}

private impl Slider
{
    mtd hitTest(pos: Math.Point)
    {
        if isHot1 or isHot2
        {
            invalidate()
            isHot1, isHot2 = false
        }

        rect := getPosBox1()
        if rect.contains(pos)
        {
            isHot1 = true
            invalidate()
        }
        else if isRange
        {
            rect = getPosBox2()
            if rect.contains(pos)
            {
                isHot2 = true
                invalidate()
            }
        }
    }

    mtd getOffsetedClientRect()->Math.Rectangle
    {
        var result: retval = getClientRect()
        result.x += (result.height * 0.5)
        result.width -= result.height
        return result
    }

    mtd getPosBox1()->Math.Rectangle
    {
        var result: retval = getOffsetedClientRect()
        result.x += result.width * value1
        result.x -= (result.height * 0.5)

        result.y = result.vertCenter() - (result.height * 0.5)
        result.width = result.height
        return result
    }

    mtd getPosBox2()->Math.Rectangle
    {
        var result: retval = getOffsetedClientRect()
        result.x += result.width * value2
        result.x -= (result.height * 0.5)

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