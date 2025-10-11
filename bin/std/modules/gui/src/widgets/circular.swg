#global public
using Core, Pixel

#[Swag.EnumFlags]
enum CircularFlags
{
    Zero    = 0
    Range
    Integer
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
    using wnd: Wnd

    // Main values (0.0 to 1.0 range)
    value1:     f32     // Primary value (or left value in range mode)
    value2:     f32     // Secondary value (right value in range mode)

    // Value range (like in linear slider)
    min:       f32           // Minimum value
    max:       f32 = 1.0     // Maximum value
    steps:     f32           // Step size (0 = no stepping)

    // Configuration flags and visual settings
    circularFlags:     CircularFlags                    // Control behavior flags (Range, Integer, etc.)
    stepForm:          CircularStepForm = .Tick         // Visual form of step indicators (None, Dot, Tick)
    markerForm:        CircularMarkerForm = .Circle     // Visual form of value markers (None, Circle, Needle)

    // Angular configuration (in radians)
    startAngle:     f32 = 0                       // Starting angle of the circular range (0 = top)
    endAngle:       f32 = Math.ConstF32.TwoPi     // Ending angle of the circular range (2π = full circle)

    // Step visualization settings
    tickCount:                 u32 = 20     // Number of step indicators to draw
    tickInnerRadiusFactor:     f32 = 2      // Distance from main circle to inner edge of ticks
    tickOuterRadiusFactor:     f32 = 6      // Distance from main circle to outer edge of ticks
    circleGap:                 f32 = 0      // Gap between inner and outer circles (0 = single circle)

    // Colors
    rangeColor: Color // Color for the range arc (when in range mode)

    // Event signals
    sigStartChange:     SigArray'func||(*Circular)     // Fired when user starts changing value
    sigChanged:         SigArray'func||(*Circular)     // Fired when value changes
    sigEndChange:       SigArray'func||(*Circular)     // Fired when user finishes changing value

    private
    {
        // Internal state tracking
        isMoving:       bool     // True when user is dragging a marker
        isPressed1:     bool     // True when primary marker is being pressed
        isPressed2:     bool     // True when secondary marker is being pressed
        isHot1:         bool     // True when mouse is hovering over primary marker
        isHot2:         bool     // True when mouse is hovering over secondary marker
        hasValue1:      bool     // True if primary value has been set
        hasValue2:      bool     // True if secondary value has been set

        // Calculated geometry (updated in updateGeometry())
        radius:           f32     // Radius of the main circular track
        centerX:          f32     // X coordinate of circle center
        centerY:          f32     // Y coordinate of circle center
        correcMoving:     f32     // Movement correction factor

        // Animation blending for smooth color transitions
        aniMark1:     BlendColor     // Color animation for primary marker
        aniMark2:     BlendColor     // Color animation for secondary marker
        aniStep:      BlendColor     // Color animation for step indicators
    }
}

impl IWnd for Circular
{
    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        let bc     = evt.bc
        let colors = .getThemeColors()

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

            if .isPressed1 do
                colorMark1 = colors.circular_PressedMark
            elif .isHot1 do
                colorMark1 = colors.circular_HotMark
            else do
                colorMark1 = colors.circular_Mark

            if .isPressed2 do
                colorMark2 = colors.circular_PressedMark
            elif .isHot2 do
                colorMark2 = colors.circular_HotMark
            else do
                colorMark2 = colors.circular_Mark
        }

        .updateGeometry()

        bc.painter.setAntialiased(true)

        // Draw range arc if enabled
        if .circularFlags.has(.Range) do
            .drawRangeArc(bc.painter)

        // Draw circular track
        .drawCircularTrack(bc.painter, colorCircle)

        // Draw steps if enabled
        if .stepForm != .None and .tickCount > 0 do
            .drawSteps(bc.painter, colorStep)

        // Draw value1 marker
        .aniMark1.apply(me, colorMark1)
        .drawValueMarker(bc.painter, .aniMark1.cur(), .value1)

        // Draw value2 marker if range mode
        if .circularFlags.has(.Range)
        {
            .aniMark2.apply(me, colorMark2)
            .drawValueMarker(bc.painter, .aniMark2.cur(), .value2)
        }
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        let pos = .surfaceToLocal(evt.surfacePos)
        .updateGeometry()

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

            let angle           = .getAngleFromPosition(pos)
            let normalizedValue = .angleToValue(angle)

            if .isPressed1 do
                .setValue1(normalizedValue)
            elif .isPressed2 do
                .setValue2(normalizedValue)
            .invalidate()

        case MousePressed:
            if evt.button == .Left
            {
                .isMoving = true
                .startChange()

                let angle           = .getAngleFromPosition(pos)
                let normalizedValue = .angleToValue(angle)

                if .isHot1
                {
                    .isPressed1   = true
                    .correcMoving = 0
                }
                elif .isHot2
                {
                    .isPressed2   = true
                    .correcMoving = 0
                }
                elif !.circularFlags.has(.Range)
                {
                    .correcMoving = 0
                    .isPressed1   = true
                    .setValue1(normalizedValue)
                }
                else
                {
                    .correcMoving = 0
                    // Determine which marker is closest
                    let m = (.value1 + .value2) * 0.5
                    if normalizedValue <= m
                    {
                        .isPressed1 = true
                        .setValue1(normalizedValue)
                    }
                    else
                    {
                        .isPressed2 = true
                        .setValue2(normalizedValue)
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

internal impl Circular
{
    mtd updateGeometry()
    {
        let clientRect = .getClientRect()
        .centerX = clientRect.x + clientRect.width * 0.5
        .centerY = clientRect.y + clientRect.height * 0.5
        .radius  = Math.min(clientRect.width, clientRect.height) * 0.5
        .radius -= 2
        if .tickOuterRadiusFactor > 0 do
            .radius -= .tickOuterRadiusFactor
    }

    mtd angleToValue(angle: f32)->f32
    {
        let totalAngle      = .endAngle - .startAngle
        var normalizedAngle = angle

        // For partial arcs, we need to find the closest valid angle
        if normalizedAngle > totalAngle
        {
            // Determine which boundary is closer
            let distanceToEnd   = normalizedAngle - totalAngle
            let distanceToStart = (Math.ConstF32.TwoPi - normalizedAngle)

            // If we're closer to the start (going around the circle), snap to 0
            // Otherwise snap to the end
            if distanceToStart < distanceToEnd do
                normalizedAngle = 0
            else do
                normalizedAngle = totalAngle
        }

        if normalizedAngle < 0 do
            normalizedAngle = 0

        return normalizedAngle / totalAngle
    }

    mtd const valueToAngle(val: f32)->f32
    {
        let totalAngle = .endAngle - .startAngle
        return .startAngle + (val * totalAngle)
    }

    mtd getAngleFromPosition(pos: Math.Point)->f32
    {
        let dx = pos.x - .centerX
        let dy = pos.y - .centerY

        // Use atan2(dx, -dy) to have 0° pointing up and 90° pointing right
        // -dy because Y axis is inverted (down = positive)
        var angle = Math.atan2(dx, -dy)

        // Normalize the angle according to our coordinate system
        angle = angle - .startAngle

        // Normalize to range [0, 2π]
        while angle < 0 do
            angle += Math.ConstF32.TwoPi
        while angle > Math.ConstF32.TwoPi do
            angle -= Math.ConstF32.TwoPi

        return angle
    }

    mtd getPositionFromAngle(angle: f32)->Math.Vector2
    {
        // Add startAngle to return to global coordinate system
        let adjustedAngle = angle + .startAngle

        var result: retval
        // For 0° pointing up: x = sin(angle), y = -cos(angle)
        result.x = .centerX + Math.sin(adjustedAngle) * .radius
        result.y = .centerY - Math.cos(adjustedAngle) * .radius
        return result
    }

    // Range arc drawing
    mtd drawRangeArc(painter: *Painter)
    {
        let color    = .rangeColor.a ? .rangeColor : .getThemeColors().circular_RangeArc
        let penSize  = .circleGap ? .circleGap : 4.0
        let pen      = Pen.createSolid(color, penSize)
        let startArc = .valueToAngle(.value1)
        let endArc   = .valueToAngle(.value2)
        painter.drawArc(.centerX, .centerY, .radius, startArc, endArc, pen)
    }

    // Main marker drawing dispatcher
    mtd drawValueMarker(painter: *Painter, color: Color, value: f32)
    {
        #[Swag.Complete]
        switch .markerForm
        {
        case .None:
            return
        case .Circle:
            .drawCircleMarker(painter, color, value)
        case .Needle:
            .drawNeedleMarker(painter, color, value)
        }
    }

    // Circle marker
    mtd drawCircleMarker(painter: *Painter, color: Color, value: f32)
    {
        let angle        = .valueToAngle(value)
        let pos          = .getPositionFromAngle(angle)
        let markerRadius = .getThemeMetrics().circular_MarkerSize

        let brush = Brush.createSolid(color)
        let pen   = Pen.createSolid(color, 1.5)

        // Draw filled circle with border
        painter.fillCircle(pos.x, pos.y, markerRadius, brush)
        painter.drawCircle(pos.x, pos.y, markerRadius, pen)
    }

    // Needle marker (line from center to current position)
    mtd drawNeedleMarker(painter: *Painter, color: Color, value: f32)
    {
        let angle     = .valueToAngle(value)
        let trackPos  = .getPositionFromAngle(angle)
        let centerPos = Math.Vector2{.centerX, .centerY}

        // Create gradient pen for nice visual effect
        let pen = Pen.createSolid(color, 2.5)

        // Draw needle line from center to track
        painter.drawLine(centerPos, trackPos, pen)

        // Draw small circle at center as pivot point
        let pivotRadius = 3.0
        let brush       = Brush.createSolid(color)
        painter.fillCircle(.centerX, .centerY, pivotRadius, brush)

        // Draw small circle at track position as tip
        let tipRadius = 2.0
        painter.fillCircle(trackPos.x, trackPos.y, tipRadius, brush)
    }

    // Main step drawing dispatcher
    mtd drawSteps(painter: *Painter, color: Color)
    {
        #[Swag.Complete]
        switch .stepForm
        {
        case .None:
            return
        case .Dot:
            .drawStepDots(painter, color)
        case .Tick:
            .drawStepTicks(painter, color)
        }
    }

    // Dots implementation
    mtd drawStepDots(painter: *Painter, color: Color)
    {
        if .tickCount <= 1 do
            return

        let totalAngle  = .endAngle - .startAngle
        let stepAngle   = totalAngle / cast(f32) (.tickCount - 1)
        let brush       = Brush.createSolid(color)
        let dotRadius   = (.tickOuterRadiusFactor - .tickInnerRadiusFactor) * 0.5
        let dotDistance = .radius + (.tickOuterRadiusFactor + .tickInnerRadiusFactor) * 0.5

        for var i: u32 = 0; i < .tickCount; i += 1
        {
            let angle = .startAngle + stepAngle * cast(f32) i

            let pos = Math.Vector2{.centerX + Math.sin(angle) * dotDistance, .centerY - Math.cos(angle) * dotDistance}
            painter.fillCircle(pos.x, pos.y, dotRadius, brush)
        }
    }

    // Tick marks implementation
    mtd drawStepTicks(painter: *Painter, color: Color)
    {
        if .tickCount <= 1 do
            return

        let totalAngle = .endAngle - .startAngle
        let stepAngle  = totalAngle / cast(f32) (.tickCount - 1)
        let pen        = Pen.createSolid(color, 3.0)

        let innerRadius = .radius + .tickInnerRadiusFactor
        let outerRadius = .radius + .tickOuterRadiusFactor

        for var i: u32 = 0; i < .tickCount; i += 1
        {
            let angle = .startAngle + stepAngle * cast(f32) i

            let innerPos = Math.Vector2{.centerX + Math.sin(angle) * innerRadius, .centerY - Math.cos(angle) * innerRadius}
            let outerPos = Math.Vector2{.centerX + Math.sin(angle) * outerRadius, .centerY - Math.cos(angle) * outerRadius}
            painter.drawLine(innerPos, outerPos, pen)
        }
    }

    mtd drawCircularTrack(painter: *Painter, color: Color)
    {
        let pen = Pen.createSolid(color, 2.0)
        painter.drawCircle(.centerX, .centerY, .radius - .circleGap * 0.5, pen)
        if .circleGap != 0 do
            painter.drawCircle(.centerX, .centerY, .radius + .circleGap * 0.5, pen)
    }

    mtd clamp(f: f32)->f32
    {
        if .steps > 0
        {
            let r = cast(s32) ((f / .steps) + 0.5)
            return r * .steps
        }

        if .circularFlags.has(.Integer)
        {
            let val = Math.round((.min + (.max - .min) * f))
            return (val - .min) / (.max - .min)
        }

        return f
    }

    // Modified setValue methods that use the new clamp method:
    mtd setValue1(f: f32)
    {
        var fc = .clamp(f)
        if .circularFlags.has(.Range) do
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
        var fc = .clamp(f)
        if .circularFlags.has(.Range) do
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
        .updateGeometry()

        if .isHot1 or .isHot2
        {
            .invalidate()
            .isHot1, .isHot2 = false
        }

        // Check value2 first if range mode (like in horizontal slider)
        if .circularFlags.has(.Range)
        {
            let rect = .getPosBox2()
            if rect.contains(pos)
            {
                .isHot2 = true
                .invalidate()
                return
            }
        }

        // Check value1
        let rect = .getPosBox1()
        if rect.contains(pos)
        {
            .isHot1 = true
            .invalidate()
        }
    }

    mtd getPosBoxAt(val, size: f32)->Math.Rectangle
    {
        .updateGeometry()

        let angle   = .valueToAngle(val)
        let pos     = .getPositionFromAngle(angle)
        var result: retval
        result.x      = pos.x - size
        result.y      = pos.y - size
        result.width  = size * 2
        result.height = size * 2
        return result
    }

    mtd getPosBox1()->Math.Rectangle
    {
        return .getPosBoxAt(.value1, .getThemeMetrics().circular_MarkerSize)
    }

    mtd getPosBox2()->Math.Rectangle
    {
        return .getPosBoxAt(.value2, .getThemeMetrics().circular_MarkerSize)
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

impl Circular
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

    // Set left and right values, for a range
    mtd setRangeValues(left, right: f32)
    {
        Debug.assert(left < right)
        Debug.assert(.circularFlags.has(.Range))
        let diff = .max - .min
        .value1 = (left - .min) / diff
        .value2 = (right - .min) / diff
        .setValue1(.value1)
        .setValue2(.value2)
    }

    // Set the min and max values, and optional steps
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

    // Set the angular range of the circular slider
    func setAngleRange(me, startAngle, endAngle: Math.Angle)
    {
        Debug.assert(endAngle.rad > startAngle.rad)
        Debug.assert((endAngle.rad - startAngle.rad) <= Math.ConstF32.TwoPi)
        .startAngle = startAngle.rad
        .endAngle   = endAngle.rad
    }

    // Set the display stepForm (None, Dot, or Tick)
    mtd setStepForm(form: CircularStepForm)
    {
        if .stepForm != form
        {
            .stepForm = form
            .invalidate()
        }
    }

    // Set the number of step indicators to display (only applies when stepForm is Dot or Tick)
    mtd setStepCount(count: u32)
    {
        if .tickCount != count
        {
            .tickCount = count
            .invalidate()
        }
    }

    #[Swag.Discardable]
    func create(parent: *Wnd, position: Math.Rectangle = {}, id: WndId = null, range = false)->*Circular
    {
        Debug.assert(parent != null)
        var pos = position

        if pos.width == 0 do
            pos.width = parent.getThemeMetrics().circular_Size
        if pos.height == 0 do
            pos.height = pos.width

        let res = Wnd.create'Circular(parent, null, pos, id)
        if range do
            res.circularFlags.add(.Range)
        return res
    }
}
