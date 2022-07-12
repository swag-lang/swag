struct FormText
{
    using base: BaseForm
    fontSize:   f32 = 48
    boldSize:   f32 = 0
    text:       String

    #[Serialization.NoSerialize]
    {
        gizmo:      GizmoRect
        isDirty:    bool
        typeFace:   const *TypeFace
        cacheFace:  const *TypeFace
        cachebSize: f32
        cachesSize: f32
        cachefSize: f32

        pathl, pathl1: LinePathList
    }
}

impl IForm for FormText
{
    mtd isSame(ptr: *BaseForm)->bool
    {
        other := cast(*FormText) ptr
        return  borderColor == other.borderColor and
                borderSize  == other.borderSize and
                backColor   == other.backColor and
                alpha       == other.alpha and
                fontSize    == other.fontSize and
                boldSize    == other.boldSize
    }

    mtd copy(dst, src: *BaseForm)
    {
        dstf := cast(*FormText) dst
        srcf := cast(*FormText) src
        dstf.borderColor = srcf.borderColor
        dstf.borderSize  = srcf.borderSize
        dstf.backColor   = srcf.backColor
        dstf.alpha       = srcf.alpha
        dstf.fontSize    = srcf.fontSize
        dstf.boldSize    = srcf.boldSize
    }

    mtd edit(evt: *MouseEvent, paintRect: Math.Rectangle, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        xa := cast(s32) ((movingStart.x - paintRect.x) / zoom)
        ya := cast(s32) ((movingStart.y - paintRect.y) / zoom)
        xb := cast(s32) ((localPos.x - paintRect.x) / zoom)
        yb := cast(s32) ((localPos.y - paintRect.y) / zoom)
        setBaseForm(xa, ya, xb - xa, yb - ya)
        if evt.kind == .MouseReleased and evt.button == .Left
            return false
        return true
    }

    mtd moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        x += newPosX
        y += newPosY
        w += newSizeX
        h += newSizeY
    }

    mtd setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        if newSizeX < 0
        {
            x = newPosX + newSizeX
            w = -newSizeX
        }
        else
        {
            x = newPosX
            w = newSizeX
        }

        if newSizeY < 0
        {
            y = newPosY + newSizeY
            h = -newSizeY
        }
        else
        {
            y = newPosY
            h = newSizeY
        }
    }

    mtd moveForm(newPosX, newPosY: s32)
    {
        x += newPosX
        y += newPosY
    }

    mtd movePoint(pt: u32, newPosX, newPosY: s32)
    {
    }

    mtd boundRect(raw: bool)->Math.Rectangle
    {
        return Math.Rectangle{x, y, w, h}
    }

    mtd mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return gizmo.mouse(view, pos, evt)
    }

    mtd paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags & (.Hot | .Selected)
        {
            gizmo.rect = paintPos
            gizmo.paint(painter, flags & .Selected ? false : true)
        }
    }

    mtd hitTest(pt: Math.Point, z: f32)->bool
    {
        rect := paintPos
        rect.inflate(-2*borderSize)
        if backColor.a == 0 and rect.contains(pt)
            return false
        return true
    }

    mtd paint(painter: *Painter, z: f32, quickMode: bool)
    {
        bSize := adaptSize(borderSize, MinTextBorderSize, MaxTextBorderSize, z, quickMode, 2, 5)
        fSize := adaptSize(fontSize, MinFontSize, MaxFontSize, z, quickMode, 24, 48)
        sSize := adaptSize(boldSize, MinBoldSize, MaxBoldSize, z, quickMode, 1, 2)

        pp := Math.Rectangle{0, 0, paintPos.width, paintPos.height}
        pp.inflate(-bSize * 0.5)
        round := Math.max(bSize, 10)

        if text.isEmpty()
        {
            text = "A"
            isDirty = true
        }

        face := typeFace orelse g_App.theme.res.defaultTypeFaceR

        if  face  != cacheFace or
            bSize != cachebSize or
            fSize != cachefSize or
            sSize != cachesSize
        {
            isDirty = true
        }

        if isDirty
        {
            isDirty = false
            cacheFace = face
            cachesSize = sSize
            cachebSize = bSize
            cachefSize = fSize

            assume face.getStringOutline(&pathl, text, acast fSize)
            pathl.flatten(painter.curState.paintQuality)
            if !Math.isZeroEpsilon(sSize)
            {
                pathl.offset(sSize, JoinStyle.Round, 0.5)
                pathl.clean()
            }

            assume face.getStringOutline(&pathl1, text, acast fSize)
            pathl1.flatten(painter.curState.paintQuality)
            if !Math.isZeroEpsilon(bSize + sSize)
            {
                pathl1.offset(bSize + sSize, JoinStyle.Round, 0.5)
                pathl1.clean()
            }
        }

        x0 := pp.horzCenter() - (0.5 * (pathl.bvMin.x + pathl.bvMax.x))
        y0 := pp.vertCenter() - (0.5 * (pathl.bvMin.y + pathl.bvMax.y))
        painter.translateTransform(x0, y0)
        painter.fillPath(&pathl1, borderColor)
        painter.fillPath(&pathl, backColor)
    }

    mtd setup(capt: *Capture)
    {
        capture = capt

        gizmo.roundShape = true
        gizmo.anchorSize = 10
        gizmo.trackChange = true
        gizmo.paintSimpleBorder = false
        gizmo.paintBigBorder = true

        gizmo.sigChanged = @|self|(g, first, hitMove) {
            if !first
                self.capture.undo()
            undo := self.capture.newUndo(.MoveBaseForm)
            undo.baseForm = self
            zoom := self.capture.editView.zoom
            undo.newPosX  = cast(s32) ((g.movingRect.x - g.startRect.x) / zoom)
            undo.newPosY  = cast(s32) ((g.movingRect.y - g.startRect.y) / zoom)
            undo.newSizeX = cast(s32) ((g.movingRect.width - g.startRect.width) / zoom)
            undo.newSizeY = cast(s32) ((g.movingRect.height - g.startRect.height) / zoom)
            self.capture.pushUndo(undo)
            self.capture.editView.invalidate()
        }
    }
}
