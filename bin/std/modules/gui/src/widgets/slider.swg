#global public
using Core, Pixel

#[Swag.EnumFlags]
enum SliderFlags
{
    Zero       = 0
    Range
    HilightBar
    Vertical
    Integer
    ShowSteps          // Display steps if defined
}

struct Slider
{
    using wnd:          Wnd

    value1:             f32
    value2:             f32
    min:                f32
    max:                f32 = 1.0
    steps:              f32
    sliderFlags         = SliderFlags.HilightBar
    sigStartChange:     SigArray'func||(*Slider)
    sigChanged:         SigArray'func||(*Slider)
    sigEndChange:       SigArray'func||(*Slider)

    isMoving:           bool
    isPressed1:         bool
    isPressed2:         bool
    isHot1:             bool
    isHot2:             bool
    hasValue1:          bool
    hasValue2:          bool
    correcMoving:       f32

    aniMark1:           BlendColor
    aniMark2:           BlendColor
    aniStep1:           BlendColor
    aniStep2:           BlendColor
}

impl IWnd for Slider
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let theme   = .getTheme()
        let metrics = .getThemeMetrics()
        let colors  = .getThemeColors()
        let rects   = .getThemeRects()

        var colorBkLeft:   Color = undefined
        var colorBkMiddle: Color = undefined
        var colorBkRight:  Color = undefined
        var colorMark1:    Color = undefined
        var colorMark2:    Color = undefined

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
            colorBkRight = colors.slider_BkRight
            if !.sliderFlags.has(.Range) and .sliderFlags.has(.HilightBar) do
                colorBkLeft = colors.slider_HilightBk
            else do
                colorBkLeft = colors.slider_BkLeft
            if .sliderFlags.has(.Range) and .sliderFlags.has(.HilightBar) do
                colorBkMiddle = colors.slider_HilightBk
            else do
                colorBkMiddle = colors.slider_BkMiddle

            if .isPressed1 do
                colorMark1 = colors.slider_PressedMark
            elif .isHot1 do
                colorMark1 = colors.slider_HotMark
            else do
                colorMark1 = colors.slider_Mark

            if .isPressed2 do
                colorMark2 = colors.slider_PressedMark
            elif .isHot2 do
                colorMark2 = colors.slider_HotMark
            else do
                colorMark2 = colors.slider_Mark
        }

        // Pos box
        let posBox1 = .getPosBox1()
        let posBox2 = .getPosBox2()

        var clientRect = .getClientRect()

        let isVertical = .sliderFlags.has(.Vertical)
        if isVertical
        {
            clientRect.y += (posBox1.height * 0.5)
            clientRect.height -= posBox1.height
        }
        else
        {
            clientRect.x += (posBox1.width * 0.5)
            clientRect.width -= posBox1.width
        }

        // Bar left
        var left = clientRect
        if isVertical
        {
            left.height = (clientRect.height * .value1)
            theme.drawSubRect(bc.painter, left, &rects.slider_BkLeftVert, colorBkLeft)
        }
        else
        {
            left.width = (clientRect.width * .value1)
            theme.drawSubRect(bc.painter, left, &rects.slider_BkLeft, colorBkLeft)
        }

        // Bar right
        let valueRight = .sliderFlags.has(.Range) ? .value2 : .value1
        var right      = clientRect
        if isVertical
        {
            right.y += (clientRect.height * valueRight)
            right.height -= (clientRect.height * valueRight)
            theme.drawSubRect(bc.painter, right, &rects.slider_BkRightVert, colorBkRight)
        }
        else
        {
            right.x += (clientRect.width * valueRight)
            right.width -= (clientRect.width * valueRight)
            theme.drawSubRect(bc.painter, right, &rects.slider_BkRight, colorBkRight)
        }

        // Bar middle
        if .sliderFlags.has(.Range)
        {
            var middle = clientRect
            if isVertical
            {
                middle.y = left.bottom()
                middle.setBottom(right.y)
                theme.drawSubRect(bc.painter, middle, &rects.slider_BkMiddleVert, colorBkMiddle)
            }
            else
            {
                middle.x = left.right()
                middle.setRight(right.x)
                theme.drawSubRect(bc.painter, middle, &rects.slider_BkMiddle, colorBkMiddle)
            }
        }

        // Display steps
        if .steps > 0 and .sliderFlags.has(.ShowSteps)
        {
            if .isHot1 or .isHot2 or .isMoving
            {
                .aniStep1.apply(me, colors.slider_HotStepBk)
                .aniStep2.apply(me, colors.slider_HotStepBk)
            }
            else
            {
                .aniStep1.apply(me, colors.slider_StepBk)
                .aniStep2.apply(me, colors.slider_HilightStepBk)
            }

            for var x = 0'f32; x < 1 + .steps; x += .steps
            {
                let posStep = .getPosBoxAt(x, metrics.slider_StepSize)

                var colorStep: Color = undefined
                if x <= .value1 and .sliderFlags.has(.HilightBar) do
                    colorStep = .aniStep2.cur()
                elif x >= .value1 and x <= .value2 and .sliderFlags.has(.HilightBar) and .sliderFlags.has(.Range) do
                    colorStep = .aniStep2.cur()
                else do
                    colorStep = .aniStep1.cur()
                theme.drawSubRect(bc.painter, posStep, &rects.slider_Step, colorStep)
            }
        }

        // Box value1
        .aniMark1.apply(me, colorMark1)
        theme.drawSubRect(bc.painter, posBox1, &rects.slider_Mark, .aniMark1.cur())

        // Box value2
        if .sliderFlags.has(.Range)
        {
            .aniMark2.apply(me, colorMark2)
            theme.drawSubRect(bc.painter, posBox2, &rects.slider_Mark, .aniMark2.cur())
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        let pos        = .surfaceToLocal(evt.surfacePos)
        let isVertical = .sliderFlags.has(.Vertical)

        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false
            return

        case MouseLeave:
            .isHot1, .isHot2 = false
            .invalidate()

        case MouseMove:
            if !.isMoving
            {
                .hitTest(evt.surfacePos)
                break
            }

            let clientRect = .getOffsetedClientRect()
            var f:         f32
            if isVertical
            {
                let diff = (pos.y - .correcMoving) - clientRect.y
                f = diff / clientRect.height
            }
            else
            {
                let diff = (pos.x - .correcMoving) - clientRect.x
                f = diff / clientRect.width
            }

            if .isPressed1 do
                .setValue1(f)
            elif .isPressed2 do
                .setValue2(f)
            .invalidate()

        case MousePressed:
            if evt.button == .Left
            {
                .isMoving = true
                .startChange()
                if .isHot1
                {
                    .isPressed1 = true
                    let posBox = .getPosBox1()
                    if isVertical do
                        .correcMoving = pos.y - posBox.vertCenter()
                    else do
                        .correcMoving = pos.x - posBox.horzCenter()
                }
                elif .isHot2
                {
                    .isPressed2 = true
                    let posBox = .getPosBox2()
                    if isVertical do
                        .correcMoving = pos.y - posBox.vertCenter()
                    else do
                        .correcMoving = pos.x - posBox.horzCenter()
                }
                elif !.sliderFlags.has(.Range)
                {
                    .correcMoving = 0
                    .isPressed1   = true
                    let clientRect = .getOffsetedClientRect()
                    if isVertical
                    {
                        let diff = pos.y - clientRect.y
                        .setValue1(diff / clientRect.height)
                    }
                    else
                    {
                        let diff = pos.x - clientRect.x
                        .setValue1(diff / clientRect.width)
                    }
                }
                else
                {
                    .correcMoving = 0
                    let clientRect = .getOffsetedClientRect()
                    var f:         f32
                    if isVertical
                    {
                        let diff = pos.y - clientRect.y
                        f = diff / clientRect.height
                    }
                    else
                    {
                        let diff = pos.x - clientRect.x
                        f = diff / clientRect.width
                    }

                    let m = (.value1 + .value2) * 0.5
                    if f <= m
                    {
                        .isPressed1 = true
                        .setValue1(f)
                    }
                    else
                    {
                        .isPressed2 = true
                        .setValue2(f)
                    }
                }

                .captureMouse()
                .invalidate()
            }

        case MouseReleased:
            if .isMoving and evt.button == .Left
            {
                .releaseMouse()
                .isPressed1, .isPressed2, .isMoving = false
                .hitTest(evt.surfacePos)
                .invalidate()
                .endChange()
            }
        }
    }
}

internal impl Slider
{
    mtd clamp(f: f32)->f32
    {
        if .steps > 0
        {
            let r = cast(s32) ((f / .steps) + 0.5)
            return r * .steps
        }

        if .sliderFlags.has(.Integer)
        {
            let val = Math.round((.min + (.max - .min) * f))
            return (val - .min) / (.max - .min)
        }

        return f
    }

    mtd setValue1(f: f32)
    {
        var fc = .clamp(Math.clamp(f, 0, 1))
        if .sliderFlags.has(.Range) do
            fc = Math.min(fc, .value2)
        if fc == .value1 and .hasValue1 do
            return
        .hasValue1 = true
        .value1    = fc
        .sigChanged.call(me)
        .invalidate()
    }

    mtd setValue2(f: f32)
    {
        var fc = .clamp(Math.clamp(f, 0, 1))
        if .sliderFlags.has(.Range) do
            fc = Math.max(fc, .value1)
        if fc == .value2 and .hasValue2 do
            return
        .hasValue2 = true
        .value2    = fc
        .sigChanged.call(me)
        .invalidate()
    }

    mtd hitTest(surfacePos: Math.Point)
    {
        let pos = .surfaceToLocal(surfacePos)
        if .isHot1 or .isHot2
        {
            .invalidate()
            .isHot1, .isHot2 = false
        }

        if .sliderFlags.has(.Range)
        {
            let rect = .getPosBox2()
            if rect.contains(pos)
            {
                .isHot2 = true
                .invalidate()
                return
            }
        }

        let rect = .getPosBox1()
        if rect.contains(pos)
        {
            .isHot1 = true
            .invalidate()
        }
    }

    mtd getOffsetedClientRect()->Math.Rectangle
    {
        var result: retval = .getClientRect()

        if .sliderFlags.has(.Vertical)
        {
            result.y += (result.width * 0.5)
            result.height -= result.width
        }
        else
        {
            result.x += (result.height * 0.5)
            result.width -= result.height
        }

        return result
    }

    mtd getPosBoxAt(val, size: f32)->Math.Rectangle
    {
        var result: retval = .getOffsetedClientRect()

        if .sliderFlags.has(.Vertical)
        {
            result.y += result.height * val
            result.y -= (size * 0.5)
            result.x = result.horzCenter() - (size * 0.5)
        }
        else
        {
            result.x += result.width * val
            result.x -= (size * 0.5)
            result.y = result.vertCenter() - (size * 0.5)
        }

        result.width, result.height = size
        return result
    }

    mtd getPosBox1()->Math.Rectangle
    {
        return .getPosBoxAt(.value1, .getThemeMetrics().slider_BoxSize)
    }

    mtd getPosBox2()->Math.Rectangle
    {
        return .getPosBoxAt(.value2, .getThemeMetrics().slider_BoxSize)
    }

    mtd startChange()
    {
        .sigStartChange.call(me)
    }

    mtd endChange()
    {
        .sigEndChange.call(me)
    }
}

impl Slider
{
    mtd const getValue()->f32      => .getValueLeft()
    mtd const getValueLeft()->f32  => .min + (.max - .min) * .value1
    mtd const getValueRight()->f32 => .min + (.max - .min) * .value2

    mtd setValue(f: f32)
    {
        .setValueLeft(f)
    }

    mtd setValueNoChange(f: f32)
    {
        .setValue1((f - .min) / (.max - .min))
    }

    // Set the left value (in case of a range).
    mtd setValueLeft(f: f32)
    {
        .startChange()
        .setValueNoChange(f)
        .endChange()
    }

    // Set the right value (in case of a range)
    mtd setValueRight(f: f32)
    {
        .startChange()
        .setValue2((f - .min) / (.max - .min))
        .endChange()
    }

    // Set the 'min', 'max' and 'steps' values
    func setMinMax(me, min, max: f32, steps: f32 = 0)
    {
        Debug.assert(min < max)
        Debug.assert(steps < (max - min))
        .min = min
        .max = max
        if steps > 0 do
            .steps = steps / (max - min) // Convert to normalized step size
        else do
            .steps = 0
    }

    // Set left and right values, for a range
    mtd setRangeValues(left, right: f32)
    {
        Debug.assert(left < right)
        Debug.assert(.sliderFlags.has(.Range))
        let diff = .max - .min
        .value1 = (left - .min) / diff
        .value2 = (right - .min) / diff
        .setValue1(.value1)
        .setValue2(.value2)
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null, range = false, vertical = false)->*Slider
    {
        Debug.assert(parent != null)
        var pos = position

        if pos.height == 0
        {
            if vertical do
                pos.height = parent.getThemeMetrics().slider_Width
            else do
                pos.height = parent.getThemeMetrics().slider_Height
        }

        if pos.width == 0
        {
            if vertical do
                pos.width = parent.getThemeMetrics().slider_Height
            else do
                pos.width = parent.getThemeMetrics().slider_Width
        }

        let res = Wnd.create'Slider(parent, null, pos, id)
        if range do
            res.sliderFlags.add(.Range)
        if vertical do
            res.sliderFlags.add(.Vertical)
        return res
    }
}
