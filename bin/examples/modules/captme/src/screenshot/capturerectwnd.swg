using Core, Win32, Gdi32, Pixel

struct CaptureRectWnd
{
    using wnd:         Wnd

    main:              *MainWnd
    startGrab:         bool
    cancelGrag:        bool
    tryStartGrab:      bool
    autoGrab:          bool
    lerpGrab:          Math.Rectangle
    clickStartPos:     Math.Point         // Track where the click started
}

impl IWnd for CaptureRectWnd
{
    mtd testAutoGrab()
    {
        let pt = Input.getMousePos()
        let r  = getWndRectAtPos(.surface, pt.x, pt.y)
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
        if evt.kind == .KeyPressed and evt.key == .Escape do
            g_Exit = true

        .invalidate()
    }

    mtd impl onMouseEvent(evt: *MouseEvent)
    {
        switch evt.kind
        {
        case MousePressed where evt.button == .Right:
            .startGrab    = false
            .tryStartGrab = false
            .cancelGrag   = true
            .autoGrab     = true // Return to auto grab mode

        case MousePressed where evt.button == .Left:
            .tryStartGrab    = true
            .clickStartPos.x = evt.surfacePos.x
            .clickStartPos.y = evt.surfacePos.y

        case MouseReleased where evt.button == .Left:
            if .cancelGrag
            {
                .cancelGrag = false
                break
            }

            // If we're still in tryStartGrab mode (didn't move mouse significantly)
            if .tryStartGrab
            {
                // Validate the current auto grab selection
                g_GrabDone = true
                g_Exit     = true
            }
            elif .startGrab
            {
                // Manual rectangle selection completed
                g_GrabDone = true
                g_Exit     = true
            }

        case MouseMove where .tryStartGrab:
            g_EndGrabX = evt.surfacePos.x
            g_EndGrabY = evt.surfacePos.y

            // Check if mouse moved significantly from click start position
            if Math.abs(evt.surfacePos.x - .clickStartPos.x) > 1 and
               Math.abs(evt.surfacePos.y - .clickStartPos.y) > 1
            {
                // Switch to manual rectangle selection mode
                .tryStartGrab = false
                .startGrab    = true
                .autoGrab     = false

                // Set grab start to click position
                g_StartGrabX = .clickStartPos.x
                g_StartGrabY = .clickStartPos.y
            }

        case MouseMove where .startGrab:
            g_EndGrabX = evt.surfacePos.x
            g_EndGrabY = evt.surfacePos.y
            .invalidate()

        case MouseMove:
            // Auto grab is always active when not dragging
            if !.startGrab
            {
                .autoGrab = true
                .testAutoGrab()
                .invalidate()
            }
        }
    }

    mtd impl onPaintEvent(evt: *PaintEvent)
    {
        var grabRect = getGrabRect()

        // Lerp grab rectangle only in auto grab mode
        if .autoGrab and !.startGrab
        {
            let dt = 100.0'f32
            .lerpGrab.x      = Math.moveTowards(.lerpGrab.x, grabRect.x, dt)
            .lerpGrab.y      = Math.moveTowards(.lerpGrab.y, grabRect.y, dt)
            .lerpGrab.width  = Math.moveTowards(.lerpGrab.width, grabRect.width, dt)
            .lerpGrab.height = Math.moveTowards(.lerpGrab.height, grabRect.height, dt)
            var roundRect = .lerpGrab
            roundRect.round()
            if !grabRect.isEqualEpsilon(roundRect) do
                .invalidate()
            grabRect = roundRect
        }
        else
        {
            .lerpGrab = grabRect
        }

        let painter = evt.bc.painter

        // Draw the desktop back image
        painter.setInterpolationMode(.Linear)
        painter.drawTexture(.position, g_DesktopTexture)

        // Draw the normal desktop image inside the grab rectangle
        let bkCol: Color = {0, 0, 0, 100}
        if !grabRect.isEmpty()
        {
            painter.fillRect(0, 0, grabRect.x, .position.height, bkCol)
            painter.fillRect(grabRect.x, 0, grabRect.width, grabRect.y, bkCol)
            painter.fillRect(grabRect.right(), 0, .position.width - grabRect.right(), .position.bottom(), bkCol)
            painter.fillRect(grabRect.x, grabRect.bottom(), grabRect.width, .position.bottom() - grabRect.bottom(), bkCol)
        }
        else
        {
            painter.fillRect(.position, bkCol)
        }

        let col = .getThemeColors().hilight
        painter.drawRect(grabRect, col, 2)

        // Rectangles - only show crosshairs in manual grab mode
        if .main.captureOptions.drawCross and !.autoGrab
        {
            painter.drawLine(g_EndGrabX, 0, g_EndGrabX, cast() g_ScreenHeight, Argb.White)
            painter.drawLine(0, g_EndGrabY, cast() g_ScreenWidth, g_EndGrabY, Argb.White)
        }

        // Dimensions
        if grabRect.width > 0 and grabRect.height > 0
        {
            let fontR = .getFont().regular
            let str   = Format.toString("% x %", grabRect.width, grabRect.height)
            var xx    = grabRect.x + 4
            var yy    = grabRect.y - fontR.height - 8
            if xx < 0 or yy < 0
            {
                xx = grabRect.x + 5
                yy = grabRect.y + 5
            }

            let m = Painter.measureString(str, fontR)
            painter.fillRoundRect({xx - 4, yy - 4, m.width + 8, fontR.height + 8}, 2, 2, col)

            painter.drawString(xx, yy, str, fontR, Argb.White)
        }
    }
}
