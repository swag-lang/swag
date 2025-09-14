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
        let theme   = me.getTheme()
        let metrics = me.getThemeMetrics()
        let colors  = me.getThemeColors()
        let rects   = me.getThemeRects()

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
            if !me.sliderFlags.has(.Range) and me.sliderFlags.has(.HilightBar):
                colorBkLeft = colors.slider_HilightBk
            else:
                colorBkLeft = colors.slider_BkLeft
            if me.sliderFlags.has(.Range) and me.sliderFlags.has(.HilightBar):
                colorBkMiddle = colors.slider_HilightBk
            else:
                colorBkMiddle = colors.slider_BkMiddle

            if me.isPressed1:
                colorMark1 = colors.slider_PressedMark
            elif me.isHot1:
                colorMark1 = colors.slider_HotMark
            else:
                colorMark1 = colors.slider_Mark

            if me.isPressed2:
                colorMark2 = colors.slider_PressedMark
            elif me.isHot2:
                colorMark2 = colors.slider_HotMark
            else:
                colorMark2 = colors.slider_Mark
        }

        // Pos box
        let posBox1 = me.getPosBox1()
        let posBox2 = me.getPosBox2()

        var clientRect = me.getClientRect()

        let isVertical = me.sliderFlags.has(.Vertical)
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
            left.height = (clientRect.height * me.value1)
            theme.drawSubRect(bc.painter, left, &rects.slider_BkLeftVert, colorBkLeft)
        }
        else
        {
            left.width = (clientRect.width * me.value1)
            theme.drawSubRect(bc.painter, left, &rects.slider_BkLeft, colorBkLeft)
        }

        // Bar right
        let valueRight = me.sliderFlags.has(.Range) ? me.value2 : me.value1
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
        if me.sliderFlags.has(.Range)
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
        if me.steps > 0 and me.sliderFlags.has(.ShowSteps)
        {
            if me.isHot1 or me.isHot2 or me.isMoving
            {
                me.aniStep1.apply(me, colors.slider_HotStepBk)
                me.aniStep2.apply(me, colors.slider_HotStepBk)
            }
            else
            {
                me.aniStep1.apply(me, colors.slider_StepBk)
                me.aniStep2.apply(me, colors.slider_HilightStepBk)
            }

            for var x = 0'f32; x < 1 + me.steps; x += me.steps
            {
                let posStep = me.getPosBoxAt(x, metrics.slider_StepSize)

                var colorStep: Color = undefined
                if x <= me.value1 and me.sliderFlags.has(.HilightBar):
                    colorStep = me.aniStep2.cur()
                elif x >= me.value1 and x <= me.value2 and me.sliderFlags.has(.HilightBar) and me.sliderFlags.has(.Range):
                    colorStep = me.aniStep2.cur()
                else:
                    colorStep = me.aniStep1.cur()
                theme.drawSubRect(bc.painter, posStep, &rects.slider_Step, colorStep)
            }
        }

        // Box value1
        me.aniMark1.apply(me, colorMark1)
        theme.drawSubRect(bc.painter, posBox1, &rects.slider_Mark, me.aniMark1.cur())

        // Box value2
        if me.sliderFlags.has(.Range)
        {
            me.aniMark2.apply(me, colorMark2)
            theme.drawSubRect(bc.painter, posBox2, &rects.slider_Mark, me.aniMark2.cur())
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        let pos        = me.surfaceToLocal(evt.surfacePos)
        let isVertical = me.sliderFlags.has(.Vertical)

        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false
            return

        case MouseLeave:
            me.isHot1, me.isHot2 = false
            me.invalidate()

        case MouseMove:
            if !me.isMoving
            {
                me.hitTest(evt.surfacePos)
                break
            }

            let clientRect = me.getOffsetedClientRect()
            var f:         f32
            if isVertical
            {
                let diff = (pos.y - me.correcMoving) - clientRect.y
                f = diff / clientRect.height
            }
            else
            {
                let diff = (pos.x - me.correcMoving) - clientRect.x
                f = diff / clientRect.width
            }

            if me.isPressed1:
                me.setValue1(f)
            elif me.isPressed2:
                me.setValue2(f)
            me.invalidate()

        case MousePressed:
            if evt.button == .Left
            {
                me.isMoving = true
                me.startChange()
                if me.isHot1
                {
                    me.isPressed1 = true
                    let posBox = me.getPosBox1()
                    if isVertical:
                        me.correcMoving = pos.y - posBox.vertCenter()
                    else:
                        me.correcMoving = pos.x - posBox.horzCenter()
                }
                elif me.isHot2
                {
                    me.isPressed2 = true
                    let posBox = me.getPosBox2()
                    if isVertical:
                        me.correcMoving = pos.y - posBox.vertCenter()
                    else:
                        me.correcMoving = pos.x - posBox.horzCenter()
                }
                elif !me.sliderFlags.has(.Range)
                {
                    me.correcMoving = 0
                    me.isPressed1   = true
                    let clientRect = me.getOffsetedClientRect()
                    if isVertical
                    {
                        let diff = pos.y - clientRect.y
                        me.setValue1(diff / clientRect.height)
                    }
                    else
                    {
                        let diff = pos.x - clientRect.x
                        me.setValue1(diff / clientRect.width)
                    }
                }
                else
                {
                    me.correcMoving = 0
                    let clientRect = me.getOffsetedClientRect()
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

                    let m = (me.value1 + me.value2) * 0.5
                    if f <= m
                    {
                        me.isPressed1 = true
                        me.setValue1(f)
                    }
                    else
                    {
                        me.isPressed2 = true
                        me.setValue2(f)
                    }
                }

                me.captureMouse()
                me.invalidate()
            }

        case MouseReleased:
            if me.isMoving and evt.button == .Left
            {
                me.releaseMouse()
                me.isPressed1, me.isPressed2, me.isMoving = false
                me.hitTest(evt.surfacePos)
                me.invalidate()
                me.endChange()
            }
        }
    }
}

internal impl Slider
{
    mtd clamp(f: f32)->f32
    {
        if me.steps > 0
        {
            let r = cast(s32) ((f / me.steps) + 0.5)
            return r * me.steps
        }

        if me.sliderFlags.has(.Integer)
        {
            let val = Math.round((me.min + (me.max - me.min) * f))
            return (val - me.min) / (me.max - me.min)
        }

        return f
    }

    mtd setValue1(f: f32)
    {
        var fc = me.clamp(Math.clamp(f, 0, 1))
        if me.sliderFlags.has(.Range):
            fc = Math.min(fc, me.value2)
        if fc == me.value1 and me.hasValue1:
            return
        me.hasValue1 = true
        me.value1    = fc
        me.sigChanged.call(me)
        me.invalidate()
    }

    mtd setValue2(f: f32)
    {
        var fc = me.clamp(Math.clamp(f, 0, 1))
        if me.sliderFlags.has(.Range):
            fc = Math.max(fc, me.value1)
        if fc == me.value2 and me.hasValue2:
            return
        me.hasValue2 = true
        me.value2    = fc
        me.sigChanged.call(me)
        me.invalidate()
    }

    mtd hitTest(surfacePos: Math.Point)
    {
        let pos = me.surfaceToLocal(surfacePos)
        if me.isHot1 or me.isHot2
        {
            me.invalidate()
            me.isHot1, me.isHot2 = false
        }

        if me.sliderFlags.has(.Range)
        {
            let rect = me.getPosBox2()
            if rect.contains(pos)
            {
                me.isHot2 = true
                me.invalidate()
                return
            }
        }

        let rect = me.getPosBox1()
        if rect.contains(pos)
        {
            me.isHot1 = true
            me.invalidate()
        }
    }

    mtd getOffsetedClientRect()->Math.Rectangle
    {
        var result: retval = me.getClientRect()

        if me.sliderFlags.has(.Vertical)
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
        var result: retval = me.getOffsetedClientRect()

        if me.sliderFlags.has(.Vertical)
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
        return me.getPosBoxAt(me.value1, me.getThemeMetrics().slider_BoxSize)
    }

    mtd getPosBox2()->Math.Rectangle
    {
        return me.getPosBoxAt(me.value2, me.getThemeMetrics().slider_BoxSize)
    }

    mtd startChange()
    {
        me.sigStartChange.call(me)
    }

    mtd endChange()
    {
        me.sigEndChange.call(me)
    }
}

impl Slider
{
    mtd const getValue()->f32      => me.getValueLeft()
    mtd const getValueLeft()->f32  => me.min + (me.max - me.min) * me.value1
    mtd const getValueRight()->f32 => me.min + (me.max - me.min) * me.value2

    mtd setValue(f: f32)
    {
        me.setValueLeft(f)
    }

    mtd setValueNoChange(f: f32)
    {
        me.setValue1((f - me.min) / (me.max - me.min))
    }

    // Set the left value (in case of a range).
    mtd setValueLeft(f: f32)
    {
        me.startChange()
        me.setValueNoChange(f)
        me.endChange()
    }

    // Set the right value (in case of a range)
    mtd setValueRight(f: f32)
    {
        me.startChange()
        me.setValue2((f - me.min) / (me.max - me.min))
        me.endChange()
    }

    // Set the 'min', 'max' and 'steps' values
    func setMinMax(me, min, max: f32, steps: f32 = 0)
    {
        Debug.assert(min < max)
        Debug.assert(steps < (max - min))
        me.min = min
        me.max = max
        if steps > 0:
            me.steps = steps / (max - min) // Convert to normalized step size
        else:
            me.steps = 0
    }

    // Set left and right values, for a range
    mtd setRangeValues(left, right: f32)
    {
        Debug.assert(left < right)
        Debug.assert(me.sliderFlags.has(.Range))
        let diff = me.max - me.min
        me.value1 = (left - me.min) / diff
        me.value2 = (right - me.min) / diff
        me.setValue1(me.value1)
        me.setValue2(me.value2)
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null, range = false, vertical = false)->*Slider
    {
        Debug.assert(parent != null)
        var pos = position

        if pos.height == 0
        {
            if vertical:
                pos.height = parent.getThemeMetrics().slider_Width
            else:
                pos.height = parent.getThemeMetrics().slider_Height
        }

        if pos.width == 0
        {
            if vertical:
                pos.width = parent.getThemeMetrics().slider_Height
            else:
                pos.width = parent.getThemeMetrics().slider_Width
        }

        let res = Wnd.create'Slider(parent, null, pos, id)
        if range:
            res.sliderFlags.add(.Range)
        if vertical:
            res.sliderFlags.add(.Vertical)
        return res
    }
}
