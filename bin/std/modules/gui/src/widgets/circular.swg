#global public
using Core, Pixel

#[Swag.EnumFlags]
enum CircularFlags
{
    Zero = 0
}

enum CircularStepForm
{
    NoSteps     = 0
    LittleLines
}

struct Circular
{
    using wnd:                 Wnd

    value:                     f32
    sliderFlags:               CircularFlags
    stepForm:                  CircularStepForm = .LittleLines
    startAngle:                f32 = 0
    endAngle:                  f32 = Math.ConstF32.TwoPi
    stepCount:                 u32 = 20
    sigStartChange:            SigArray'func||(*Self)
    sigChanged:                SigArray'func||(*Self)
    sigEndChange:              SigArray'func||(*Self)
    radiusFactor:              f32 = 0.35
    stepInnerRadiusFactor:     f32 = 4
    stepOuterRadiusFactor:     f32 = 6

    isMoving:                  bool
    isPressed:                 bool
    isHot:                     bool
    hasValue:                  bool
    radius:                    f32
    centerX:                   f32
    centerY:                   f32

    aniMark:                   BlendColor
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
        var colorMark:   Color = undefined
        var colorStep:   Color = undefined

        if bc.isDisabled
        {
            colorBk     = colors.circular_DisabledBk
            colorCircle = colors.circular_DisabledCircle
            colorMark   = colors.circular_DisabledMark
            colorStep   = colors.circular_DisabledStep
        }
        else
        {
            colorBk     = colors.circular_Bk
            colorCircle = colors.circular_Circle
            colorStep   = colors.circular_Step
            if isPressed:
                colorMark = colors.circular_PressedMark
            elif isHot:
                colorMark = colors.circular_HotMark
            else:
                colorMark = colors.circular_Mark
        }

        updateGeometry()

        var clientRect = getClientRect()
        bc.painter.setAntialiased(true)

        // Draw circular track
        drawCircularTrack(bc.painter, theme, colorCircle)

        // Draw steps if enabled
        if stepForm == .LittleLines and stepCount > 0:
            drawStepLines(bc.painter, theme, colorStep)

        // Draw value marker
        var posBox = getPosBox()
        aniMark.apply(self, colorMark)
        theme.drawSubRect(bc.painter, posBox, &rects.circular_Mark, aniMark.cur())
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
        radius  = Math.min(clientRect.width, clientRect.height) * radiusFactor
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

    mtd drawStepLines(painter: *Painter, theme: *Theme, color: Color)
    {
        if stepCount <= 1:
            return

        let totalAngle = endAngle - startAngle
        let stepAngle  = totalAngle / cast(f32) (stepCount - 1)
        let pen        = Pen.createSolid(color, 1.5)

        // Dimensions des lignes d'étape
        let innerRadius = radius + stepInnerRadiusFactor
        let outerRadius = radius + stepOuterRadiusFactor

        for var i: u32 = 0; i < stepCount; i += 1
        {
            let angle = startAngle + stepAngle * cast(f32) i

            // Calculer les positions intérieure et extérieure pour la ligne d'étape
            // Utiliser sin/cos cohérent avec getPositionFromAngle
            let innerPos = Math.Vector2{centerX + Math.sin(angle) * innerRadius, centerY - Math.cos(angle) * innerRadius}
            let outerPos = Math.Vector2{centerX + Math.sin(angle) * outerRadius, centerY - Math.cos(angle) * outerRadius}

            // Dessiner la ligne d'étape
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

    mtd setInternalValue(f: f32)
    {
        var fc = clamp(Math.clamp(f, 0, 1))
        if fc == value and hasValue:
            return
        hasValue = true
        value    = fc
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

        let angle   = valueToAngle(val)
        var pos     = getPositionFromAngle(angle)
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
        value = normalizedAngle / totalAngle

        endChange()
    }

    // Get the angle
    mtd const getAngle()->Math.Angle
    {
        let currentAngle = valueToAngle(value)
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

    // Set the display stepForm (NoSteps or LittleLines)
    mtd setStepForm(form: CircularStepForm)
    {
        if stepForm != form
        {
            stepForm = form
            invalidate()
        }
    }

    // Set the number of step lines to display (only applies when stepForm is LittleLines)
    mtd setStepCount(count: u32)
    {
        if stepCount != count
        {
            stepCount = count
            invalidate()
        }
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
