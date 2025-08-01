#global public
using Core, Pixel

#[Swag.EnumFlags]
enum CircularFlags
{
    Zero       = 0
    HilightBar
}

struct Circular
{
    using wnd:          Wnd

    value:             f32
    value2:             f32
    min:                f32
    max:                f32 = 1.0
    steps:              f32
    sliderFlags         = CircularFlags.HilightBar
    sigStartChange:     SigArray'func||(*Self)
    sigChanged:         SigArray'func||(*Self)
    sigEndChange:       SigArray'func||(*Self)

    isMoving:           bool
    isPressed:          bool
    isHot:              bool
    hasValue:           bool
    startAngle:         f32 = 0
    endAngle:           f32 = Math.ConstF32.TwoPi
    radius:             f32
    centerX:            f32
    centerY:            f32

    aniMark:            BlendColor
    aniStep:            BlendColor
}

impl IWnd for Circular
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc      = evt.bc
        let theme   = getTheme()
        let metrics = getThemeMetrics()
        let colors  = getThemeColors()
        let rects   = getThemeRects()

        var colorBk:     Color = undefined
        var colorCircle: Color = undefined
        var colorMark:   Color = undefined

        if bc.isDisabled
        {
            colorBk     = colors.circular_DisabledBk
            colorCircle = colors.circular_DisabledCircle
            colorMark   = colors.circular_DisabledMark
        }
        else
        {
            colorBk     = colors.circular_Bk
            colorCircle = colors.circular_Circle
            if isPressed:
                colorMark = colors.circular_PressedMark
            elif isHot:
                colorMark = colors.circular_HotMark
            else:
                colorMark = colors.circular_Mark
        }

        updateGeometry()

        var clientRect = getClientRect()

        // Draw circular track
        drawCircularTrack(bc.painter, theme, colorCircle)

        // Display steps
        if steps > 0
        {
            if isHot or isMoving:
                aniStep.apply(self, colors.slider_HotStepBk)
            else:
                aniStep.apply(self, colors.slider_StepBk)

            let totalSteps = cast(s32) ((max - min) / steps)
            for var i = 0; i <= totalSteps; i += 1
            {
                let stepValue = cast(f32) i / cast(f32) totalSteps
                var posStep   = getPosBoxAt(stepValue, metrics.slider_StepSize)

                var colorStep: Color = undefined
                colorStep = aniStep.cur()
                theme.drawSubRect(bc.painter, posStep, &rects.slider_Step, colorStep)
            }
        }

        // Draw value marker
        aniMark.apply(self, colorMark)

        // Draw arc to show current value if HilightBar flag is set
        if sliderFlags & CircularFlags.HilightBar:
            drawValueArc(bc.painter, theme, colors.circular_Mark)

        // Draw mark
        var posBox = getPosBox()
        theme.drawSubRect(bc.painter, posBox, &rects.slider_Mark, aniMark.cur())
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        var pos = surfaceToLocal(evt.surfacePos)
        updateGeometry()

        switch evt.kind
        {
        case MouseWheel:
            evt.accepted = false
            return

        case MouseLeave:
            isHot = false
            invalidate()

        case MouseMove:
            if !isMoving
            {
                hitTest(evt.surfacePos)
                break
            }

            let angle           = getAngleFromPosition(pos)
            let normalizedValue = angleToValue(angle)
            if isPressed:
                setInternalValue(normalizedValue)
            invalidate()

        case MousePressed:
            if evt.button == .Left
            {
                isMoving = true
                startChange()

                let angle           = getAngleFromPosition(pos)
                let normalizedValue = angleToValue(angle)

                if !isHot:
                    setInternalValue(normalizedValue)
                isPressed = true

                captureMouse()
                invalidate()
            }

        case MouseReleased:
            if isMoving and evt.button == .Left
            {
                releaseMouse()
                isPressed, isMoving = false
                hitTest(evt.surfacePos)
                invalidate()
                endChange()
            }
        }
    }
}

internal impl Circular
{
    mtd updateGeometry()
    {
        var clientRect = getClientRect()
        centerX = clientRect.x + clientRect.width * 0.5
        centerY = clientRect.y + clientRect.height * 0.5
        radius  = Math.min(clientRect.width, clientRect.height) * 0.35
    }

    mtd getAngleFromPosition(pos: Math.Point)->f32
    {
        let dx    = pos.x - centerX
        let dy    = pos.y - centerY
        var angle = Math.atan2(dy, dx)

        // Normalize angle to our coordinate system
        // Convert from standard math coordinates to our circular slider coordinates
        angle = angle - startAngle

        // Normalize to [0, 2π] range
        while angle < 0:
            angle += Math.ConstF32.Pi * 2
        while angle > Math.ConstF32.Pi * 2:
            angle -= Math.ConstF32.Pi * 2

        return angle
    }

    mtd angleToValue(angle: f32)->f32
    {
        let totalAngle      = endAngle - startAngle
        var normalizedAngle = angle

        // Clamp angle to valid range
        if normalizedAngle > totalAngle:
            normalizedAngle = totalAngle
        if normalizedAngle < 0:
            normalizedAngle = 0

        return normalizedAngle / totalAngle
    }

    mtd valueToAngle(val: f32)->f32
    {
        let totalAngle = endAngle - startAngle
        return startAngle + (val * totalAngle)
    }

    mtd getPositionFromAngle(angle: f32)->Math.Vector2
    {
        var result: retval
        result.x = centerX + Math.cos(angle) * radius
        result.y = centerY + Math.sin(angle) * radius
        return result
    }

    mtd drawCircularTrack(painter: *Painter, theme: *Theme, color: Color)
    {
        // Draw the circular track arc
        let trackRadius = radius * 0.9
        let startPos    = getPositionFromAngle(startAngle)
        let endPos      = getPositionFromAngle(endAngle)

        // This would ideally use a proper arc drawing function
        // For now, we'll approximate with a series of small lines
        let segments  = 64
        let angleStep = (endAngle - startAngle) / cast(f32) segments

        let pen     = Pen.createSolid(color, 2.0)
        var prevPos = getPositionFromAngle(startAngle)
        for var i = 1; i <= segments; i += 1
        {
            let currentAngle = startAngle + angleStep * cast(f32) i
            var currentPos   = getPositionFromAngle(currentAngle)

            // Draw line segment (this would need proper painter line drawing)
            painter.drawLine(prevPos, currentPos, pen)
            prevPos = currentPos
        }
    }

    mtd drawValueArc(painter: *Painter, theme: *Theme, color: Color)
    {
        if !hasValue:
            return

        let valueAngle = valueToAngle(value)
        let segments   = cast(s32) (32 * value) + 1
        let angleStep  = (valueAngle - startAngle) / cast(f32) segments

        let pen     = Pen.createSolid(color, 3.0)
        var prevPos = getPositionFromAngle(startAngle)
        for var i = 1; i <= segments; i += 1
        {
            let currentAngle = startAngle + angleStep * cast(f32) i
            var currentPos   = getPositionFromAngle(currentAngle)

            // Draw highlighted arc segment
            painter.drawLine(prevPos, currentPos, pen)
            prevPos = currentPos
        }
    }

    mtd clamp(f: f32)->f32
    {
        if steps > 0
        {
            let r = cast(s32) ((f / steps) + 0.5)
            return r * steps
        }

        return Math.clamp(f, 0, 1)
    }

    mtd setInternalValue(f: f32)
    {
        var fc = clamp(Math.clamp(f, 0, 1))
        if fc == value and hasValue:
            return
        hasValue = true
        value   = fc
        sigChanged.call(self)
        invalidate()
    }

    mtd hitTest(surfacePos: Math.Point)
    {
        var pos = surfaceToLocal(surfacePos)
        updateGeometry()

        if isHot
        {
            invalidate()
            isHot = false
        }

        // Check if point is near the circular track
        let dx         = pos.x - centerX
        let dy         = pos.y - centerY
        let distance   = Math.sqrt(dx * dx + dy * dy)
        let trackWidth = getThemeMetrics().slider_BoxSize

        if Math.abs(distance - radius) <= trackWidth
        {
            let angle      = getAngleFromPosition(pos)
            let totalAngle = endAngle - startAngle
            if angle >= 0 and angle <= totalAngle
            {
                isHot = true
                invalidate()
            }
        }

        // Also check if we're near the current value marker
        var rect = getPosBox()
        if rect.contains(pos)
        {
            isHot = true
            invalidate()
        }
    }

    mtd getPosBoxAt(val, size: f32)->Math.Rectangle
    {
        updateGeometry()
        let angle = valueToAngle(val)
        var pos   = getPositionFromAngle(angle)

        var result: retval
        result.x      = pos.x - (size * 0.5)
        result.y      = pos.y - (size * 0.5)
        result.width  = size
        result.height = size
        return result
    }

    mtd getPosBox()->Math.Rectangle
    {
        return getPosBoxAt(value, getThemeMetrics().slider_BoxSize)
    }

    mtd startChange()
    {
        sigStartChange.call(self)
    }

    mtd endChange()
    {
        sigEndChange.call(self)
    }
}

impl Circular
{
    mtd const getValue()->f32 => min + (max - min) * value

    // Set the value
    mtd setValue(f: f32)
    {
        startChange()
        setInternalValue((f - min) / (max - min))
        endChange()
    }

    // Set the 'min', 'max' and 'steps' values
    func setMinMax(self, min, max: f32, steps: f32 = 0)
    {
        Debug.assert(min < max)
        if steps > 0:
            Debug.assert(steps <= (max - min))
        self.min   = min
        self.max   = max
        self.steps = steps
    }

    // Set the angular range of the circular slider
    func setAngleRange(self, startAngle, endAngle: Math.Angle)
    {
        Debug.assert(endAngle.rad > startAngle.rad)
        Debug.assert((endAngle.rad - startAngle.rad) <= Math.ConstF32.TwoPi)
        self.startAngle = startAngle.rad
        self.endAngle   = endAngle.rad
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null)->*Circular
    {
        Debug.assert(parent != null)
        var pos = position

        if pos.width == 0:
            pos.width = parent.getThemeMetrics().circular_Size
        if pos.height == 0:
            pos.height = pos.width

        let res = Wnd.create'Circular(parent, null, pos, id)
        return res
    }
}
