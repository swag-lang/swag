#global public
using Core, Pixel

#[Swag.EnumFlags]
enum CircularFlags
{
    Zero  = 0
    Range
}

enum CircularStepForm
{
    None = 0
    Dot
    Tick
}

enum CircularMarkerForm
{
    None   = 0
    Circle
    Needle
}

struct Circular
{
    using wnd:                 Wnd

    value1:                    f32
    value2:                    f32
    circularFlags:             CircularFlags
    stepForm:                  CircularStepForm = .Tick
    markerForm:                CircularMarkerForm = .Circle
    startAngle:                f32 = 0
    endAngle:                  f32 = Math.ConstF32.TwoPi
    stepCount:                 u32 = 20
    sigStartChange:            SigArray'func||(*Self)
    sigChanged:                SigArray'func||(*Self)
    sigEndChange:              SigArray'func||(*Self)
    stepInnerRadiusFactor:     f32 = 2
    stepOuterRadiusFactor:     f32 = 6

    isMoving:                  bool
    isPressed1:                bool
    isPressed2:                bool
    isHot1:                    bool
    isHot2:                    bool
    hasValue1:                 bool
    hasValue2:                 bool
    radius:                    f32
    centerX:                   f32
    centerY:                   f32
    correcMoving:              f32

    aniMark1:                  BlendColor
    aniMark2:                  BlendColor
    aniStep:                   BlendColor
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
        var colorMark1:  Color = undefined
        var colorMark2:  Color = undefined
        var colorStep:   Color = undefined

        if bc.isDisabled
        {
            colorBk     = colors.circular_DisabledBk
            colorCircle = colors.circular_DisabledCircle
            colorMark1  = colors.circular_DisabledMark
            colorMark2  = colors.circular_DisabledMark
            colorStep   = colors.circular_DisabledStep
        }
        else
        {
            colorBk     = colors.circular_Bk
            colorCircle = colors.circular_Circle
            colorStep   = colors.circular_Step

            if isPressed1:
                colorMark1 = colors.circular_PressedMark
            elif isHot1:
                colorMark1 = colors.circular_HotMark
            else:
                colorMark1 = colors.circular_Mark

            if isPressed2:
                colorMark2 = colors.circular_PressedMark
            elif isHot2:
                colorMark2 = colors.circular_HotMark
            else:
                colorMark2 = colors.circular_Mark
        }

        updateGeometry()

        var clientRect = getClientRect()
        bc.painter.setAntialiased(true)

        // Draw circular track
        drawCircularTrack(bc.painter, theme, colorCircle)

        // Draw range arc if enabled
        if circularFlags.has(.Range):
            drawRangeArc(bc.painter, theme, colors.circular_RangeArc)

        // Draw steps if enabled
        if stepForm != .None and stepCount > 0:
            drawSteps(bc.painter, theme, colorStep)

        // Draw value1 marker
        aniMark1.apply(self, colorMark1)
        drawValueMarker(bc.painter, theme, aniMark1.cur(), value1)

        // Draw value2 marker if range mode
        if circularFlags.has(.Range)
        {
            aniMark2.apply(self, colorMark2)
            drawValueMarker(bc.painter, theme, aniMark2.cur(), value2)
        }
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
            isHot1, isHot2 = false
            invalidate()

        case MouseMove:
            if !isMoving
            {
                hitTest(evt.surfacePos)
                break
            }

            let angle           = getAngleFromPosition(pos)
            let normalizedValue = angleToValue(angle)

            if isPressed1:
                setValue1(normalizedValue)
            elif isPressed2:
                setValue2(normalizedValue)
            invalidate()

        case MousePressed:
            if evt.button == .Left
            {
                isMoving = true
                startChange()

                let angle           = getAngleFromPosition(pos)
                let normalizedValue = angleToValue(angle)

                if isHot1
                {
                    isPressed1   = true
                    correcMoving = 0
                }
                elif isHot2
                {
                    isPressed2   = true
                    correcMoving = 0
                }
                elif !circularFlags.has(.Range)
                {
                    correcMoving = 0
                    isPressed1   = true
                    setValue1(normalizedValue)
                }
                else
                {
                    correcMoving = 0
                    // Détermine quel marqueur est le plus proche
                    let m = (value1 + value2) * 0.5
                    if normalizedValue <= m
                    {
                        isPressed1 = true
                        setValue1(normalizedValue)
                    }
                    else
                    {
                        isPressed2 = true
                        setValue2(normalizedValue)
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
        radius  = Math.min(clientRect.width, clientRect.height) * 0.5
        radius -= 2
        if stepOuterRadiusFactor > 0:
            radius -= stepOuterRadiusFactor
    }

    mtd angleToValue(angle: f32)->f32
    {
        let totalAngle      = endAngle - startAngle
        var normalizedAngle = angle

        // For partial arcs, we need to find the closest valid angle
        if normalizedAngle > totalAngle
        {
            // Determine which boundary is closer
            let distanceToEnd   = normalizedAngle - totalAngle
            let distanceToStart = (Math.ConstF32.TwoPi - normalizedAngle)

            // If we're closer to the start (going around the circle), snap to 0
            // Otherwise snap to the end
            if distanceToStart < distanceToEnd:
                normalizedAngle = 0
            else:
                normalizedAngle = totalAngle
        }

        if normalizedAngle < 0:
            normalizedAngle = 0

        return normalizedAngle / totalAngle
    }

    mtd const valueToAngle(val: f32)->f32
    {
        let totalAngle = endAngle - startAngle
        return startAngle + (val * totalAngle)
    }

    mtd getAngleFromPosition(pos: Math.Point)->f32
    {
        let dx = pos.x - centerX
        let dy = pos.y - centerY

        // Utiliser atan2(dx, -dy) pour avoir 0° vers le haut et 90° vers la droite
        // -dy car l'axe Y est inversé (vers le bas = positif)
        var angle = Math.atan2(dx, -dy)

        // Normaliser l'angle selon notre système de coordonnées
        angle = angle - startAngle

        // Normaliser dans la plage [0, 2π]
        while angle < 0:
            angle += Math.ConstF32.TwoPi
        while angle > Math.ConstF32.TwoPi:
            angle -= Math.ConstF32.TwoPi

        return angle
    }

    mtd getPositionFromAngle(angle: f32)->Math.Vector2
    {
        // Ajouter startAngle pour revenir au système de coordonnées global
        let adjustedAngle = angle + startAngle

        var result: retval
        // Pour 0° vers le haut : x = sin(angle), y = -cos(angle)
        result.x = centerX + Math.sin(adjustedAngle) * radius
        result.y = centerY - Math.cos(adjustedAngle) * radius
        return result
    }

    // Range arc drawing
    mtd drawRangeArc(painter: *Painter, theme: *Theme, color: Color)
    {
        let pen = Pen.createSolid(color, 4.0)
        let startArc = valueToAngle(value1)
        let endArc   = valueToAngle(value2)
        painter.drawArc(centerX, centerY, radius, startArc, endArc, pen)
    }

    // Main marker drawing dispatcher
    mtd drawValueMarker(painter: *Painter, theme: *Theme, color: Color, value: f32)
    {
        #[Swag.Complete]
        switch markerForm
        {
        case .None:
            return
        case .Circle:
            drawCircleMarker(painter, theme, color, value)
        case .Needle:
            drawNeedleMarker(painter, theme, color, value)
        }
    }

    // Circle marker
    mtd drawCircleMarker(painter: *Painter, theme: *Theme, color: Color, value: f32)
    {
        let angle        = valueToAngle(value)
        let pos          = getPositionFromAngle(angle)
        let markerRadius = getThemeMetrics().circular_MarkerSize

        let brush = Brush.createSolid(color)
        let pen   = Pen.createSolid(color, 1.5)

        // Draw filled circle with border
        painter.fillCircle(pos.x, pos.y, markerRadius, brush)
        painter.drawCircle(pos.x, pos.y, markerRadius, pen)
    }

    // Needle marker (line from center to current position)
    mtd drawNeedleMarker(painter: *Painter, theme: *Theme, color: Color, value: f32)
    {
        let angle     = valueToAngle(value)
        let trackPos  = getPositionFromAngle(angle)
        let centerPos = Math.Vector2{centerX, centerY}

        // Create gradient pen for nice visual effect
        let pen = Pen.createSolid(color, 2.5)

        // Draw needle line from center to track
        painter.drawLine(centerPos, trackPos, pen)

        // Draw small circle at center as pivot point
        let pivotRadius = 3.0
        let brush       = Brush.createSolid(color)
        painter.fillCircle(centerX, centerY, pivotRadius, brush)

        // Draw small circle at track position as tip
        let tipRadius = 2.0
        painter.fillCircle(trackPos.x, trackPos.y, tipRadius, brush)
    }

    // Main step drawing dispatcher
    mtd drawSteps(painter: *Painter, theme: *Theme, color: Color)
    {
        #[Swag.Complete]
        switch stepForm
        {
        case .None:
            return
        case .Dot:
            drawStepDots(painter, theme, color)
        case .Tick:
            drawStepTicks(painter, theme, color)
        }
    }

    // Dots implementation
    mtd drawStepDots(painter: *Painter, theme: *Theme, color: Color)
    {
        if stepCount <= 1:
            return

        let totalAngle  = endAngle - startAngle
        let stepAngle   = totalAngle / cast(f32) (stepCount - 1)
        let brush       = Brush.createSolid(color)
        let dotRadius   = (stepOuterRadiusFactor - stepInnerRadiusFactor) * 0.5
        let dotDistance = radius + (stepOuterRadiusFactor + stepInnerRadiusFactor) * 0.5

        for var i: u32 = 0; i < stepCount; i += 1
        {
            let angle = startAngle + stepAngle * cast(f32) i

            let pos = Math.Vector2{centerX + Math.sin(angle) * dotDistance, centerY - Math.cos(angle) * dotDistance}
            painter.fillCircle(pos.x, pos.y, dotRadius, brush)
        }
    }

    // Thick ticks implementation
    mtd drawStepTicks(painter: *Painter, theme: *Theme, color: Color)
    {
        if stepCount <= 1:
            return

        let totalAngle = endAngle - startAngle
        let stepAngle  = totalAngle / cast(f32) (stepCount - 1)
        let pen        = Pen.createSolid(color, 3.0)

        let innerRadius = radius + stepInnerRadiusFactor
        let outerRadius = radius + stepOuterRadiusFactor

        for var i: u32 = 0; i < stepCount; i += 1
        {
            let angle = startAngle + stepAngle * cast(f32) i

            let innerPos = Math.Vector2{centerX + Math.sin(angle) * innerRadius, centerY - Math.cos(angle) * innerRadius}
            let outerPos = Math.Vector2{centerX + Math.sin(angle) * outerRadius, centerY - Math.cos(angle) * outerRadius}
            painter.drawLine(innerPos, outerPos, pen)
        }
    }

    mtd drawCircularTrack(painter: *Painter, theme: *Theme, color: Color)
    {
        let pen = Pen.createSolid(color, 2.0)
        painter.drawCircle(centerX, centerY, radius, pen)
    }

    mtd clamp(f: f32)->f32
    {
        return Math.clamp(f, 0, 1)
    }

    // Modified setValue methods that use this validation:
    mtd setValue1(f: f32)
    {
        let fc = clamp(f)
        if fc == value1 and hasValue1:
            return

        hasValue1 = true
        value1    = fc

        sigChanged.call(self)
        invalidate()
    }

    mtd setValue2(f: f32)
    {
        let fc = clamp(f)
        if fc == value2 and hasValue2:
            return

        hasValue2 = true
        value2    = fc

        sigChanged.call(self)
        invalidate()
    }

    mtd hitTest(surfacePos: Math.Point)
    {
        var pos = surfaceToLocal(surfacePos)
        updateGeometry()

        if isHot1 or isHot2
        {
            invalidate()
            isHot1, isHot2 = false
        }

        // Check value2 first if range mode (comme dans le slider horizontal)
        if circularFlags.has(.Range)
        {
            var rect = getPosBox2()
            if rect.contains(pos)
            {
                isHot2 = true
                invalidate()
                return
            }
        }

        // Check value1
        var rect = getPosBox1()
        if rect.contains(pos)
        {
            isHot1 = true
            invalidate()
        }
    }

    mtd getPosBoxAt(val, size: f32)->Math.Rectangle
    {
        updateGeometry()

        let angle   = valueToAngle(val)
        var pos     = getPositionFromAngle(angle)
        var result: retval
        result.x      = pos.x - size
        result.y      = pos.y - size
        result.width  = size * 2
        result.height = size * 2
        return result
    }

    mtd getPosBox1()->Math.Rectangle
    {
        return getPosBoxAt(value1, getThemeMetrics().circular_MarkerSize)
    }

    mtd getPosBox2()->Math.Rectangle
    {
        return getPosBoxAt(value2, getThemeMetrics().circular_MarkerSize)
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
    mtd const getValue()->f32      => getValueLeft()
    mtd const getValueLeft()->f32  => value1
    mtd const getValueRight()->f32 => value2

    mtd setValue(f: f32)
    {
        setValueLeft(f)
    }

    // Set the left value (in case of a range).
    mtd setValueLeft(f: f32)
    {
        startChange()
        setValue1(f)
        endChange()
    }

    // Set the right value (in case of a range)
    mtd setValueRight(f: f32)
    {
        startChange()
        setValue2(f)
        endChange()
    }

    // Set left and right values, for a range
    mtd setRangeValues(left, right: f32)
    {
        Debug.assert(left < right)
        Debug.assert(circularFlags.has(.Range))
        value1 = left
        value2 = right
        setValue1(value1)
        setValue2(value2)
    }

    // Set the current angle
    mtd setAngle(f: Math.Angle)
    {
        startChange()

        // Normalize the angle to the range [startAngle, endAngle]
        var normalizedAngle = f.rad - startAngle

        // Clamp to valid range
        let totalAngle = endAngle - startAngle
        normalizedAngle = Math.clamp(normalizedAngle, 0, totalAngle)

        // Convert to normalized value (0-1)
        value1 = normalizedAngle / totalAngle

        endChange()
    }

    // Get the angle
    mtd const getAngle()->Math.Angle
    {
        let currentAngle = valueToAngle(value1)
        return Math.Angle{rad: currentAngle}
    }

    // Set the angular range of the circular slider
    func setAngleRange(self, startAngle, endAngle: Math.Angle)
    {
        Debug.assert(endAngle.rad > startAngle.rad)
        Debug.assert((endAngle.rad - startAngle.rad) <= Math.ConstF32.TwoPi)
        self.startAngle = startAngle.rad
        self.endAngle   = endAngle.rad
    }

    // Set the display stepForm (NoSteps or Lines)
    mtd setStepForm(form: CircularStepForm)
    {
        if stepForm != form
        {
            stepForm = form
            invalidate()
        }
    }

    // Set the number of step lines to display (only applies when stepForm is Lines)
    mtd setStepCount(count: u32)
    {
        if stepCount != count
        {
            stepCount = count
            invalidate()
        }
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null, range = false)->*Circular
    {
        Debug.assert(parent != null)
        var pos = position

        if pos.width == 0:
            pos.width = parent.getThemeMetrics().circular_Size
        if pos.height == 0:
            pos.height = pos.width

        let res = Wnd.create'Circular(parent, null, pos, id)
        if range:
            res.circularFlags.add(.Range)
        return res
    }
}
