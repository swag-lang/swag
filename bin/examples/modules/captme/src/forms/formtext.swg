enum FormTextAlign
{
    LeftTop
    RightBottom
    Center
}

struct FormTextRune
{
    back:        LinePathList
    border:      LinePathList
    advance:     f32
}

struct FormText
{
    using base:     BaseForm
    fontSize:       f32 = 48
    boldSize:       f32 = 0
    text:           String
    alignHorz       = FormTextAlign.Center
    alignVert       = FormTextAlign.Center
    lineSpacing     = 14.0
    fontStyle       = FontFamilyStyle.Regular
    fontName:       String

    #[Serialization.NoSerialize]
    {
        gizmo:            GizmoRect
        isDirty:          bool
        isDirtyFont:      bool
        typeFace:         *TypeFace

        cacheFace:        *TypeFace
        cachebSize:       f32
        cachesSize:       f32
        cachefSize:       f32
        cacheStyle:       JoinStyle
        cacheFontN:       String
        cacheFontS:       FontFamilyStyle
        cacheBColorA:     u8

        totalY:           f32
        runes:            HashTable'(rune, FormTextRune)
        paths:            Array'LinePathList
    }
}

impl IForm for FormText
{
    mtd impl isSame(ptr: *BaseForm)->bool
    {
        let other = cast(*FormText) ptr
        return me.borderColor == other.borderColor and
               me.borderSize == other.borderSize and
               me.backColor == other.backColor and
               me.alpha == other.alpha and
               me.fontSize == other.fontSize and
               me.boldSize == other.boldSize and
               me.paintShadow == other.paintShadow and
               me.fontName == other.fontName and
               me.fontStyle == other.fontStyle and
               me.joinStyle == other.joinStyle
    }

    mtd impl copy(dst, src: *BaseForm)
    {
        let dstf = cast(*FormText) dst
        let srcf = cast(*FormText) src
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
        dstf.scale       = srcf.scale
    }

    mtd impl mustClipToBound()->bool
    {
        return true
    }

    mtd impl edit(evt: *MouseEvent, movingStart: Math.Point, localPos: Math.Point, zoom: f32)->bool
    {
        let xa = cast(s32) (movingStart.x / zoom)
        let ya = cast(s32) (movingStart.y / zoom)
        let xb = cast(s32) (localPos.x / zoom)
        let yb = cast(s32) (localPos.y / zoom)
        me.setBaseForm(xa, ya, xb - xa, yb - ya)
        if evt.kind == .MouseReleased and evt.button == .Left:
            return false
        return true
    }

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        me.x += newPosX
        me.y += newPosY
        me.w += newSizeX
        me.h += newSizeY
    }

    mtd impl scaleBaseForm(val: f32)
    {
        me.x = cast(s32) (me.x * val)
        me.y = cast(s32) (me.y * val)
        me.w = cast(s32) (me.w * val)
        me.h = cast(s32) (me.h * val)
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        if newSizeX < 0
        {
            me.x = newPosX + newSizeX
            me.w = -newSizeX
        }
        else
        {
            me.x = newPosX
            me.w = newSizeX
        }

        if newSizeY < 0
        {
            me.y = newPosY + newSizeY
            me.h = -newSizeY
        }
        else
        {
            me.y = newPosY
            me.h = newSizeY
        }
    }

    mtd impl moveForm(newPosX, newPosY: s32)
    {
        me.x += newPosX
        me.y += newPosY
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32) {}

    mtd impl boundRect()->Math.Rectangle
    {
        return Math.Rectangle{me.x, me.y, me.w, me.h}
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return me.gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags.has(.Hot | .Selected)
        {
            me.gizmo.rect = me.boundRect() * z
            me.gizmo.paint(painter, !flags.has(.Selected))
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        return false
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        let pp = me.boundRect() * z
        return pp.contains(pt)
    }

    mtd impl paint(painter: *Painter, cxt: FormPaintContext)
    {
        var cxtCpy = cxt
        cxtCpy.z = 1

        let bSize = BaseForm.adaptSize(me.borderSize, MinTextBorderSize, MaxTextBorderSize, cxtCpy, 2, 4)
        let fSize = BaseForm.adaptSize(me.fontSize, MinFontSize, MaxFontSize, cxtCpy, 20, 48)
        let sSize = BaseForm.adaptSize(me.boldSize, MinBoldSize, MaxBoldSize, cxtCpy, 1, 2)
        let z     = cxt.z

        let boundRect = me.boundRect() * z
        var pp        = boundRect
        pp.inflate(-bSize * 0.5)

        if me.text.isEmpty()
        {
            me.text    = "A"
            me.isDirty = true
        }

        // Font
        if !me.typeFace or me.isDirtyFont
        {
            me.isDirtyFont = false
            if !me.fontName.length:
                me.typeFace = null
            else
            {
                let en = FontFamily.getFromOs(me.fontName)
                me.typeFace = catch FontFamily.createTypeFace(en, me.fontStyle)
            }
        }

        var face = me.typeFace
        if !face
        {
            switch me.fontStyle
            {
            case Regular:
                face = g_App.theme.res.defaultTypeFaceR
            case Bold:
                face = g_App.theme.res.defaultTypeFaceB
            case Italic:
                face = g_App.theme.res.defaultTypeFaceI
            case BoldItalic:
                face = g_App.theme.res.defaultTypeFaceBI
            }
        }

        if face != me.cacheFace or
           bSize != me.cachebSize or
           fSize != me.cachefSize or
           sSize != me.cachesSize or
           me.joinStyle != me.cacheStyle or
           me.fontName != me.cacheFontN or
           me.fontStyle != me.cacheFontS or
           me.cacheBColorA != me.borderColor.a
        {
            me.isDirty = true
        }

        let tokens = Tokenize.split(me.text, '\n''rune)
        if me.isDirty
        {
            me.isDirty      = false
            me.cacheFace    = face
            me.cachesSize   = sSize
            me.cachebSize   = bSize
            me.cachefSize   = fSize
            me.cacheStyle   = me.joinStyle
            me.cacheFontN   = me.fontName
            me.cacheFontS   = me.fontStyle
            me.cacheBColorA = me.borderColor.a

            me.paths.clear()
            me.paths.resize(tokens.count * 2)
            me.runes.clear()

            me.totalY = 0
            foreach t, idx in tokens
            {
                let pathl  = &me.paths[idx * 2]
                let pathl1 = &me.paths[(idx * 2) + 1]

                Utf8.visitRunes(|c| t)
                {
                    var it = me.runes.tryFind(c)
                    if !it
                    {
                        var ftr: FormTextRune

                        // Back
                        ftr.advance = assume face.getRuneOutline(&ftr.back, c, cast() fSize, cast() sSize)
                        ftr.back.flatten(painter.curState.paintQuality)
                        ftr.back.clean()

                        // Border
                        if !Math.isZeroEpsilon(bSize) and me.borderColor.a
                        {
                            assume face.getRuneOutline(&ftr.border, c, cast() fSize, cast() sSize)
                            ftr.border.flatten(painter.curState.paintQuality)
                            ftr.border.offset(bSize, me.joinStyle, 0.5)
                        }

                        it = me.runes.add(c, ftr)
                    }

                    pathl.bvMax.x += it.value.advance
                    pathl.bvMin.y = Math.min(pathl.bvMin.y, it.value.back.bvMin.y)
                    pathl.bvMax.y = Math.max(pathl.bvMax.y, it.value.back.bvMax.y)

                    if it.value.border.paths.count
                    {
                        pathl1.bvMax.x += it.value.advance
                        pathl1.bvMin.y = Math.min(pathl1.bvMin.y, it.value.border.bvMin.y)
                        pathl1.bvMax.y = Math.max(pathl1.bvMax.y, it.value.border.bvMax.y)
                    }
                    else
                    {
                        pathl1.bvMin = pathl.bvMin
                        pathl1.bvMax = pathl.bvMax
                    }
                }

                me.totalY += (pathl.bvMax.y - pathl.bvMin.y)
            }
        }

        let zscale  = z * (cxt.quickMode ? 1 : Math.map(me.scale, 0, 100, 1'f32, 2'f32))
        var y0      = 0'f32
        let addSize = (bSize + sSize) * zscale

        let count = me.paths.count / 2

        var totalYZ = me.totalY * zscale
        totalYZ += cast(f32) ((count - 1) * (me.lineSpacing * zscale))

        let aV = cxt.quickMode ? FormTextAlign.Center : me.alignVert
        let aH = cxt.quickMode ? FormTextAlign.Center : me.alignHorz
        switch aV
        {
        case LeftTop:
            y0 = pp.y + addSize
        case Center:
            y0 = pp.vertCenter() - (totalYZ * 0.5)
        case RightBottom:
            y0 = pp.bottom() - addSize - totalYZ
        }

        for var i = 0; i < me.paths.count; i += 2
        {
            let pathl1 = &me.paths[i + 1]

            let wt = (pathl1.bvMax.x - pathl1.bvMin.x) * zscale
            let ht = (pathl1.bvMax.y - pathl1.bvMin.y) * zscale

            var x0 = 0'f32
            switch aH
            {
            case LeftTop:
                x0 = pp.x + addSize
            case Center:
                x0 = pp.horzCenter() - (wt * 0.5)
            case RightBottom:
                x0 = pp.right() - (wt + addSize)
            }

            var pl: LinePathList
            let tt  = painter.curState.transform

            // Border
            if !Math.isZeroEpsilon(bSize) and me.borderColor.a
            {
                painter.pushTransform()
                painter.curState.transform.tx = 0
                painter.curState.transform.ty = 0
                painter.scaleTransform(zscale, zscale)
                painter.translateTransform(tt.tx, tt.ty)
                painter.translateTransform(x0, y0 - (pathl1.bvMin.y * zscale))
                Utf8.visitRunes(|c| tokens[i / 2])
                {
                    let it = me.runes.find(c)
                    pl.paths.add(it.value.border.paths.toSlice())
                    painter.fillPath(&pl, me.borderColor)
                    pl.paths.count = 0
                    painter.translateTransform(cast(f32) it.value.advance * zscale, 0)
                }
                painter.popTransform()
            }

            // Back
            if me.backColor.a
            {
                painter.pushTransform()
                painter.curState.transform.tx = 0
                painter.curState.transform.ty = 0
                painter.scaleTransform(zscale, zscale)
                painter.translateTransform(tt.tx, tt.ty)
                painter.translateTransform(x0, y0 - (pathl1.bvMin.y * zscale))
                Utf8.visitRunes(|c| tokens[i / 2])
                {
                    let it = me.runes.find(c)
                    pl.paths.add(it.value.back.paths.toSlice())
                    painter.fillPath(&pl, me.backColor)
                    pl.paths.count = 0
                    painter.translateTransform(cast(f32) it.value.advance * zscale, 0)
                }
                painter.popTransform()
            }

            // Next line
            y0 += ht + (me.lineSpacing * zscale)
        }
    }

    mtd impl setup(capt: *Capture)
    {
        me.capture = capt

        with me.gizmo
        {
            .roundShape        = true
            .anchorSize        = 10
            .trackChange       = true
            .paintSimpleBorder = false
            .paintBigBorder    = true
        }

        me.gizmo.sigChanged = mtd||(g, first, hitMove)
        {
            g.apply(me, first, hitMove)
        }
    }
}
