enum FormTextAlign
{
    LeftTop
    RightBottom
    Center
}

struct FormTextRune
{
    advanceBack:    f32
    advanceBorder:  f32
    back:           LinePathList
    border:         LinePathList
}

struct FormText
{
    using base: BaseForm
    fontSize:   f32 = 48
    boldSize:   f32 = 0
    text:       String
    alignHorz   = FormTextAlign.Center
    alignVert   = FormTextAlign.Center
    lineSpacing = 14.0
    joinStyle   = JoinStyle.Round
    fontStyle   = FontFamilyStyle.Regular
    fontName:   String

    #[Serialization.NoSerialize]
    {
        gizmo:          GizmoRect
        isDirty:        bool
        isDirtyFont:    bool
        typeFace:       *TypeFace

        cacheFace:      *TypeFace
        cachebSize:     f32
        cachesSize:     f32
        cachefSize:     f32
        cacheStyle:     JoinStyle
        cacheFontN:     String
        cacheFontS:     FontFamilyStyle
        cacheBColorA:   u8

        totalY:     f32
        runes:      HashTable'(rune, FormTextRune)
        paths:      Array'LinePathList
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
                boldSize    == other.boldSize and
                paintShadow == other.paintShadow and
                fontName    == other.fontName and
                fontStyle   == other.fontStyle and
                joinStyle   == other.joinStyle
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
        dstf.paintShadow = srcf.paintShadow
        dstf.joinStyle   = srcf.joinStyle
        dstf.fontName    = srcf.fontName
        dstf.fontStyle   = srcf.fontStyle
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

    mtd isInside(pt: Math.Point)->bool
    {
        return false
    }

    mtd hitTest(pt: Math.Point, z: f32)->bool
    {
        rect := paintPos
        rect.inflate(-2*borderSize)
        if backColor.a == 0 and rect.contains(pt)
            return false
        return true
    }

    mtd paint(app: *Application, painter: *Painter, z: f32, quickMode: bool)
    {
        bSize := adaptSize(borderSize, MinTextBorderSize, MaxTextBorderSize, 1, quickMode, 2, 4)
        fSize := adaptSize(fontSize, MinFontSize, MaxFontSize, 1, quickMode, 24, 48)
        sSize := adaptSize(boldSize, MinBoldSize, MaxBoldSize, 1, quickMode, 1, 2)

        pp := Math.Rectangle{0, 0, paintPos.width, paintPos.height}
        pp.inflate(-bSize * 0.5)
        round := Math.max(bSize, 10)

        if text.isEmpty()
        {
            text = "A"
            isDirty = true
        }

        // Font
        if !typeFace or isDirtyFont
        {
            isDirtyFont = false
            if !fontName.length
                typeFace = null
            else
            {
                en := FontFamily.getFromOs(fontName)
                typeFace = catch FontFamily.createTypeFace(en, fontStyle)
            }
        }

        face := typeFace orelse g_App.theme.res.defaultTypeFaceR

        if face  != cacheFace or
           bSize != cachebSize or
           fSize != cachefSize or
           sSize != cachesSize or
           joinStyle != cacheStyle or
           fontName != cacheFontN or
           fontStyle != cacheFontS or
           cacheBColorA != borderColor.a
        {
            isDirty = true
        }

        tokens := Tokenize.split(text, "\n"'rune)
        if isDirty
        {
            isDirty = false
            cacheFace = face
            cachesSize = sSize
            cachebSize = bSize
            cachefSize = fSize
            cacheStyle = joinStyle
            cacheFontN = fontName
            cacheFontS = fontStyle
            cacheBColorA = borderColor.a

            paths.clear()
            paths.resize(tokens.count * 2)
            runes.clear()

            totalY = 0
            visit t, idx: tokens
            {
                pathl  := &paths[idx * 2]
                pathl1 := &paths[(idx * 2) + 1]

                Utf8.visitRunes(|c| t)
                {
                    it := runes.find(c)
                    if !it
                    {
                        var ftr: FormTextRune

                        // Back
                        ftr.advanceBack = assume face.getRuneOutline(&ftr.back, c, acast fSize)
                        if ftr.back.paths.count
                        {
                            ftr.back.flatten(painter.curState.paintQuality)
                            if !Math.isZeroEpsilon(sSize)
                            {
                                ftr.back.offset(sSize, joinStyle, 0.5)
                                ftr.back.clean()
                            }
                        }

                        // Border
                        if !Math.isZeroEpsilon(bSize) and borderColor.a
                        {
                            ftr.advanceBorder = assume face.getRuneOutline(&ftr.border, c, acast fSize)
                            if ftr.border.paths.count
                            {
                                ftr.border.flatten(painter.curState.paintQuality)
                                ftr.border.offset(sSize + bSize, joinStyle, 0.5)
                                ftr.border.clean()
                            }
                        }

                        it = runes.add(c, ftr)
                    }

                    pathl.bvMax.x += it.value.advanceBack
                    pathl.bvMin.y = Math.min(pathl.bvMin.y, it.value.back.bvMin.y)
                    pathl.bvMax.y = Math.max(pathl.bvMax.y, it.value.back.bvMax.y)

                    if it.value.border.paths.count
                    {
                        pathl1.bvMax.x += it.value.advanceBorder
                        pathl1.bvMin.y = Math.min(pathl1.bvMin.y, it.value.border.bvMin.y)
                        pathl1.bvMax.y = Math.max(pathl1.bvMax.y, it.value.border.bvMax.y)
                    }
                    else
                    {
                        pathl1.bvMin = pathl.bvMin
                        pathl1.bvMax = pathl.bvMax
                    }
                }

                totalY += (pathl.bvMax.y - pathl.bvMin.y)
            }
        }

        y0 := 0'f32
        addSize := (bSize + sSize) * z

        count := paths.count / 2

        totalYZ := totalY * z
        totalYZ += cast(f32) ((count - 1) * (lineSpacing * z))

        aV := quickMode ? FormTextAlign.Center : alignVert
        aH := quickMode ? FormTextAlign.Center : alignHorz
        switch aV
        {
        case LeftTop:       y0 = pp.y + addSize
        case Center:        y0 = pp.vertCenter() - (totalYZ * 0.5)
        case RightBottom:   y0 = pp.bottom() - addSize - totalYZ
        }

        for i := 0; i < paths.count; i += 2
        {
            pathl  := &paths[i]
            pathl1 := &paths[i + 1]

            wt := (pathl1.bvMax.x - pathl1.bvMin.x) * z
            ht := (pathl1.bvMax.y - pathl1.bvMin.y) * z

            x0 := 0'f32
            switch aH
            {
            case LeftTop:       x0 = pp.x + addSize
            case Center:        x0 = pp.horzCenter() - (wt * 0.5)
            case RightBottom:   x0 = pp.right() - (wt + addSize)
            }

            var pl: LinePathList

            // Border
            painter.pushState()
            painter.scaleTransform(z, z)
            painter.translateTransform(x0, y0 - (pathl1.bvMin.y * z))
            Utf8.visitRunes(|c| tokens[i / 2])
            {
                it := runes.find(c)
                pl.paths.add(it.value.border.paths.toSlice())
                painter.fillPath(&pl, borderColor)
                pl.paths.count = 0
                painter.translateTransform(cast(f32) it.value.advanceBorder*z, 0)
            }
            painter.popState()

            // Back
            painter.pushState()
            painter.scaleTransform(z, z)
            painter.translateTransform(x0, y0 - (pathl1.bvMin.y * z))
            Utf8.visitRunes(|c| tokens[i / 2])
            {
                it := runes.find(c)
                pl.paths.add(it.value.back.paths.toSlice())
                painter.fillPath(&pl, backColor)
                pl.paths.count = 0
                painter.translateTransform(cast(f32) it.value.advanceBack*z, 0)
            }
            painter.popState()

            // Next line
            y0 += ht + (lineSpacing * z)
        }
    }

    mtd setup(capt: *Capture)
    {
        capture = capt

        with gizmo
        {
            .roundShape = true
            .anchorSize = 10
            .trackChange = true
            .paintSimpleBorder = false
            .paintBigBorder = true
        }

        gizmo.sigChanged = @|self|(g, first, hitMove) {
            g.apply(self, first, hitMove)
        }
    }
}
