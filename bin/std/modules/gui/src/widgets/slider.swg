#global public
using Core, Pixel

#[Swag.EnumFlags]
enum SliderFlags
{
    None
    Range
    HilightBar
}

struct Slider
{
    using wnd: Wnd

    value1:         f32
    value2:         f32
    min:            f32
    max:            f32 = 1.0
    steps:          f32
    sliderFlags     = SliderFlags.HilightBar
    sigChanged:     SigArray'closure(*Self)

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

        if bc.isDisabled
        {
            colorBkLeft   = colors.slider_DisabledBk
            colorBkMiddle = colors.slider_DisabledBk
            colorBkRight  = colors.slider_DisabledBk
            colorMark1    = colors.slider_DisabledMark
            colorMark2    = colors.slider_DisabledMark
        }
        else
        {
            colorBkRight  = colors.slider_BkRight
            if !(sliderFlags & .Range) and (sliderFlags & .HilightBar)
                colorBkLeft = colors.slider_HilightBk
            else
                colorBkLeft = colors.slider_BkLeft
            if (sliderFlags & .Range) and (sliderFlags & .HilightBar)
                colorBkMiddle = colors.slider_HilightBk
            else
                colorBkMiddle = colors.slider_BkMiddle

            if isPressed1
                colorMark1 = colors.slider_PressedMark
            elif isHot1
                colorMark1 = colors.slider_HotMark
            else
                colorMark1 = colors.slider_Mark

            if isPressed2
                colorMark2 = colors.slider_PressedMark
            elif isHot2
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
        valueRight := sliderFlags & .Range ? value2 : value1
        right := clientRect
        right.x += (clientRect.width * valueRight)
        right.width -= (clientRect.width * valueRight)
        theme.drawSubRect(bc.painter, right, &rects.slider_BkRight, colorBkRight)

        // Bar middle
        if sliderFlags & .Range
        {
            middle := clientRect
            middle.x = left.right()
            middle.setRight(right.x)
            theme.drawSubRect(bc.painter, middle, &rects.slider_BkMiddle, colorBkMiddle)
        }

        // Display steps
        if steps > 0
        {
            for x := 0'f32; x < 1 + steps; x += steps
            {
                posStep := clientRect
                posStep.x = clientRect.x + (clientRect.width * x) - clientRect.height * 0.5
                posStep.width = posStep.height

                var colorStep: Color = ?
                if isHot1 or isHot2 or isMoving
                    colorStep = colors.slider_HotStepBk
                elif x <= value1 and (sliderFlags & .HilightBar)
                    colorStep = colors.slider_HilightStepBk
                elif x >= value1 and x <= value2 and (sliderFlags & .HilightBar) and (sliderFlags & .Range)
                    colorStep = colors.slider_HilightStepBk
                else
                    colorStep = colors.slider_StepBk

                theme.drawSubRect(bc.painter, posStep, &rects.slider_Step, colorStep)
            }
        }

        // Box value1
        theme.drawSubRect(bc.painter, posBox1, &rects.slider_Mark, colorMark1)
        if (sliderFlags & .Range)
            theme.drawSubRect(bc.painter, posBox2, &rects.slider_Mark, colorMark2)
    }

    mtd onMouseEvent(evt: *MouseEvent)
    {
        pos := surfaceToLocal(evt.surfacePos)
        switch evt.kind
        {
        case MouseLeave:
            isHot1, isHot2 = false
            invalidate()

        case MouseMove:
            if !isMoving
            {
                hitTest(evt.surfacePos)
                break
            }

            clientRect := getOffsetedClientRect()
            diff := (pos.x - correcMoving) - clientRect.x
            f    := diff / clientRect.width
            if isPressed1
                setValue1(f)
            elif isPressed2
                setValue2(f)
            invalidate()

        case MousePressed:
            if evt.button == .Left
            {
                isMoving = true
                if isHot1
                {
                    isPressed1 = true
                    posBox := getPosBox1()
                    correcMoving = pos.x - posBox.horzCenter()
                }
                elif isHot2
                {
                    isPressed2 = true
                    posBox := getPosBox2()
                    correcMoving = pos.x - posBox.horzCenter()
                }
                elif !(sliderFlags & .Range)
                {
                    correcMoving = 0
                    isPressed1 = true
                    clientRect := getOffsetedClientRect()
                    diff := pos.x - clientRect.x
                    setValue1(diff / clientRect.width)
                }
                else
                {
                    correcMoving = 0
                    clientRect := getOffsetedClientRect()
                    diff := pos.x - clientRect.x
                    f := diff / clientRect.width
                    m := (value1 + value2) * 0.5
                    if f <= m
                    {
                        isPressed1 = true
                        setValue1(f)
                    }
                    else
                    {
                        isPressed2 = true
                        setValue2(f)
                    }
                }

                captureMouse()
                invalidate()
            }

        case MouseReleased:
            if isMoving and evt.button == .Left
            {
                releaseMouse()
                isPressed1, isPressed2, isMoving = false
                hitTest(evt.surfacePos)
                invalidate()
            }
        }
    }
}

private impl Slider
{
    mtd clamp(f: f32)->f32
    {
        if steps > 0
        {
            r := cast(s32) ((f / steps) + 0.5)
            return r * steps
        }

        return f
    }

    mtd setValue1(f: f32)
    {
        fc := clamp(Math.clamp(f, 0, 1))
        if sliderFlags & .Range
            fc = Math.min(fc, value2)
        if fc == value1
            return
        value1 = fc
        sigChanged.call(self)
        invalidate()
    }

    mtd setValue2(f: f32)
    {
        fc := clamp(Math.clamp(f, 0, 1))
        if sliderFlags & .Range
            fc = Math.max(fc, value1)
        if fc == value2
            return
        value2 = fc
        sigChanged.call(self)
        invalidate()
    }

    mtd hitTest(surfacePos: Math.Point)
    {
        pos := surfaceToLocal(surfacePos)
        if isHot1 or isHot2
        {
            invalidate()
            isHot1, isHot2 = false
        }

        if sliderFlags & .Range
        {
            rect := getPosBox2()
            if rect.contains(pos)
            {
                isHot2 = true
                invalidate()
                return
            }
        }

        rect := getPosBox1()
        if rect.contains(pos)
        {
            isHot1 = true
            invalidate()
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
    mtdc getValue()->f32        => getValueLeft()
    mtdc getValueLeft()->f32    => min + (max - min) * value1
    mtdc getValueRight()->f32   => min + (max - min) * value2
    mtd  setValue(f: f32)       { setValueLeft(f); }
    mtd  setValueLeft(f: f32)   { setValue1((f - min) / (max - min)); }
    mtd  setValueRight(f: f32)  { setValue2((f - min) / (max - min)); }

    func setMinMax(self, min, max: f32, steps: f32 = 0)
    {
        Debug.assert(min < max)
        Debug.assert(steps < (max - min))
        self.min = min
        self.max = max
        self.steps = steps
    }

    // Set left and right values, for a range
    mtd setRangeValues(left, right: f32)
    {
        Debug.assert(left < right)
        Debug.assert(cast(bool) (sliderFlags & .Range))
        diff := max - min
        value1 = (left - min) / diff
        value2 = (right - min) / diff
        setValue1(value1)
        setValue2(value2)
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle, id: string = null, range: bool = false)->*Slider
    {
        Debug.assert(parent != null)
        pos := position
        if pos.height == 0
            pos.height = parent.getThemeMetrics().slider_Height
        if pos.width == 0
            pos.width  = parent.getThemeMetrics().slider_Width
        res := Wnd.create'Slider(parent, null, pos, id)
        if range
            res.sliderFlags |= .Range
        return res
    }
}