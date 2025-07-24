using Core, Win32, Gdi32, Pixel

struct CaptureRectWnd
{
    using wnd:        Wnd

    main:             *MainWnd
    startGrab:        bool
    cancelGrag:       bool
    tryStartGrab:     bool
    autoGrab:         bool
    lerpGrab:         Math.Rectangle
}

impl IWnd for CaptureRectWnd
{
    mtd testAutoGrab()
    {
        let pt = Input.getMousePos()
        let r = getWndRectAtPos(surface, pt.x, pt.y)
        if !r.isEmpty()
        {
            g_StartGrabX = r.x
            g_StartGrabY = r.y
            g_EndGrabX   = r.right()
            g_EndGrabY   = r.bottom()
        }
    }

    mtd impl onKeyEvent(evt: *KeyEvent)
    {
        if evt.kind == .KeyPressed and evt.key == .Escape:
            g_Exit = true

        if !tryStartGrab and !startGrab
        {
            if evt.key == .Shift or evt.key == .RShift
            {
                if evt.kind == .KeyPressed
                {
                    autoGrab = true
                    testAutoGrab()
                }
                else
                {
                    let pt = Input.getMousePos()
                    g_StartGrabX, g_EndGrabX = pt.x
                    g_StartGrabY, g_EndGrabY = pt.y
                    autoGrab = false
                }
            }
        }

        invalidate()
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MousePressed where evt.button == .Right:
            startGrab = false
            tryStartGrab = false
            cancelGrag = true

        case MousePressed where evt.button == .Left:
            tryStartGrab = true

        case MouseReleased where evt.button == .Left:
            if cancelGrag
            {
                cancelGrag = false
                break
            }

            g_GrabDone = startGrab or autoGrab
            g_Exit = true

        case MouseMove where tryStartGrab:
            g_EndGrabX = evt.surfacePos.x
            g_EndGrabY = evt.surfacePos.y

            if Math.abs(g_EndGrabX - g_StartGrabX) > 1 and
               Math.abs(g_EndGrabY - g_StartGrabY) > 1
            {
                tryStartGrab = false
                startGrab    = true
            }

        case MouseMove where startGrab:
            g_EndGrabX = evt.surfacePos.x
            g_EndGrabY = evt.surfacePos.y
            invalidate()

        case MouseMove:
            if !autoGrab
            {
                g_StartGrabX, g_EndGrabX = evt.surfacePos.x
                g_StartGrabY, g_EndGrabY = evt.surfacePos.y
            }
            else
            {
                testAutoGrab()
            }

            invalidate()
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        var grabRect = getGrabRect()

        // Lerp grab rectangle
        if autoGrab
        {
            let dt = 0.1'f32
            lerpGrab.x      = Math.lerp(lerpGrab.x, grabRect.x, dt)
            lerpGrab.y      = Math.lerp(lerpGrab.y, grabRect.y, dt)
            lerpGrab.width  = Math.lerp(lerpGrab.width, grabRect.width, dt)
            lerpGrab.height = Math.lerp(lerpGrab.height, grabRect.height, dt)
            var roundRect = lerpGrab
            roundRect.round()
            if !grabRect.isEqualEpsilon(roundRect):
                invalidate()
            grabRect = roundRect
        }
        else
        {
            lerpGrab = grabRect
        }

        let painter = evt.bc.painter

        // Draw the desktop back ima
        painter.setInterpolationMode(.Linear)
        painter.drawTexture(position, g_DesktopTexture)

        // Draw the normal desktop image inside the grab rectangle
        let col: Color = {0, 0, 0, 100}
        if !grabRect.isEmpty()
        {
            painter.fillRect(0, 0, grabRect.x, position.height, col)
            painter.fillRect(grabRect.x, 0, grabRect.width, grabRect.y, col)
            painter.fillRect(grabRect.right(), 0, position.width - grabRect.right(), position.bottom(), col)
            painter.fillRect(grabRect.x, grabRect.bottom(), grabRect.width, position.bottom() - grabRect.bottom(), col)
        }
        else
        {
            painter.fillRect(position, col)
        }

        painter.drawRect(grabRect, Argb.White)

        // Rectangles
        if main.captureOptions.drawCross and !autoGrab
        {
            painter.drawLine(g_EndGrabX, 0, g_EndGrabX, cast() g_ScreenHeight, Argb.White)
            painter.drawLine(0, g_EndGrabY, cast() g_ScreenWidth, g_EndGrabY, Argb.White)
        }

        // Dimensions
        if grabRect.width > 0 and grabRect.height > 0
        {
            let fontR = getFont().regular
            let str   = Format.toString("% x %", grabRect.width, grabRect.height)
            var xx    = grabRect.x
            var yy    = grabRect.y - fontR.height
            if xx < 0 or yy < 0
            {
                xx = grabRect.x + 5
                yy = grabRect.y + 5
            }

            painter.drawString(xx, yy, str, fontR, Argb.White)
        }
    }
}
