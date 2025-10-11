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
        return .borderColor == other.borderColor and
               .borderSize == other.borderSize and
               .backColor == other.backColor and
               .alpha == other.alpha and
               .fontSize == other.fontSize and
               .boldSize == other.boldSize and
               .paintShadow == other.paintShadow and
               .fontName == other.fontName and
               .fontStyle == other.fontStyle and
               .joinStyle == other.joinStyle
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
        .setBaseForm(xa, ya, xb - xa, yb - ya)
        if evt.kind == .MouseReleased and evt.button == .Left do
            return false
        return true
    }

    mtd impl moveBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        .x += newPosX
        .y += newPosY
        .w += newSizeX
        .h += newSizeY
    }

    mtd impl scaleBaseForm(val: f32)
    {
        .x = cast(s32) (.x * val)
        .y = cast(s32) (.y * val)
        .w = cast(s32) (.w * val)
        .h = cast(s32) (.h * val)
    }

    mtd impl setBaseForm(newPosX, newPosY, newSizeX, newSizeY: s32)
    {
        if newSizeX < 0
        {
            .x = newPosX + newSizeX
            .w = -newSizeX
        }
        else
        {
            .x = newPosX
            .w = newSizeX
        }

        if newSizeY < 0
        {
            .y = newPosY + newSizeY
            .h = -newSizeY
        }
        else
        {
            .y = newPosY
            .h = newSizeY
        }
    }

    mtd impl moveForm(newPosX, newPosY: s32)
    {
        .x += newPosX
        .y += newPosY
    }

    mtd impl movePoint(pt: u32, newPosX, newPosY: s32) {}

    mtd impl boundRect()->Math.Rectangle
    {
        return Math.Rectangle{.x, .y, .w, .h}
    }

    mtd impl mouse(view: *EditView, pos: Math.Point, evt: *MouseEvent)->bool
    {
        return .gizmo.mouse(view, pos, evt)
    }

    mtd impl paintGizmo(painter: *Painter, z: f32, flags: FormFlags)
    {
        if flags.has(.Hot | .Selected)
        {
            .gizmo.rect = .boundRect() * z
            .gizmo.paint(painter, !flags.has(.Selected))
        }
    }

    mtd impl isInside(pt: Math.Point)->bool
    {
        return false
    }

    mtd impl hitTest(pt: Math.Point, z: f32)->bool
    {
        let pp = .boundRect() * z
        return pp.contains(pt)
    }

    mtd impl paint(painter: *Painter, cxt: FormPaintContext)
    {
        var cxtCpy = cxt
        cxtCpy.z = 1

        let bSize = BaseForm.adaptSize(.borderSize, MinTextBorderSize, MaxTextBorderSize, cxtCpy, 2, 4)
        let fSize = BaseForm.adaptSize(.fontSize, MinFontSize, MaxFontSize, cxtCpy, 20, 48)
        let sSize = BaseForm.adaptSize(.boldSize, MinBoldSize, MaxBoldSize, cxtCpy, 1, 2)
        let z     = cxt.z

        let boundRect = .boundRect() * z
        var pp        = boundRect
        pp.inflate(-bSize * 0.5)

        if .text.isEmpty()
        {
            .text    = "A"
            .isDirty = true
        }

        // Font
        if !.typeFace or .isDirtyFont
        {
            .isDirtyFont = false
            if !.fontName.length do
                .typeFace = null
            else
            {
                let en = FontFamily.getFromOs(.fontName)
                .typeFace = catch FontFamily.createTypeFace(en, .fontStyle)
            }
        }

        var face = .typeFace
        if !face
        {
            switch .fontStyle
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

        if face != .cacheFace or
           bSize != .cachebSize or
           fSize != .cachefSize or
           sSize != .cachesSize or
           .joinStyle != .cacheStyle or
           .fontName != .cacheFontN or
           .fontStyle != .cacheFontS or
           .cacheBColorA != .borderColor.a
        {
            .isDirty = true
        }

        let tokens = Tokenize.split(.text, '\n''rune)
        if .isDirty
        {
            .isDirty      = false
            .cacheFace    = face
            .cachesSize   = sSize
            .cachebSize   = bSize
            .cachefSize   = fSize
            .cacheStyle   = .joinStyle
            .cacheFontN   = .fontName
            .cacheFontS   = .fontStyle
            .cacheBColorA = .borderColor.a

            .paths.clear()
            .paths.resize(tokens.count * 2)
            .runes.clear()

            .totalY = 0
            foreach t, idx in tokens
            {
                let pathl  = &.paths[idx * 2]
                let pathl1 = &.paths[(idx * 2) + 1]

                Utf8.visitRunes(|c| t)
                {
                    var it = .runes.tryFind(c)
                    if !it
                    {
                        var ftr: FormTextRune

                        // Back
                        ftr.advance = assume face.getRuneOutline(&ftr.back, c, cast() fSize, cast() sSize)
                        ftr.back.flatten(painter.curState.paintQuality)
                        ftr.back.clean()

                        // Border
                        if !Math.isZeroEpsilon(bSize) and .borderColor.a
                        {
                            assume face.getRuneOutline(&ftr.border, c, cast() fSize, cast() sSize)
                            ftr.border.flatten(painter.curState.paintQuality)
                            ftr.border.offset(bSize, .joinStyle, 0.5)
                        }

                        it = .runes.add(c, ftr)
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

                .totalY += (pathl.bvMax.y - pathl.bvMin.y)
            }
        }

        let zscale  = z * (cxt.quickMode ? 1 : Math.map(.scale, 0, 100, 1'f32, 2'f32))
        var y0      = 0'f32
        let addSize = (bSize + sSize) * zscale

        let count = .paths.count / 2

        var totalYZ = .totalY * zscale
        totalYZ += cast(f32) ((count - 1) * (.lineSpacing * zscale))

        let aV = cxt.quickMode ? FormTextAlign.Center : .alignVert
        let aH = cxt.quickMode ? FormTextAlign.Center : .alignHorz
        switch aV
        {
        case LeftTop:
            y0 = pp.y + addSize
        case Center:
            y0 = pp.vertCenter() - (totalYZ * 0.5)
        case RightBottom:
            y0 = pp.bottom() - addSize - totalYZ
        }

        for var i = 0; i < .paths.count; i += 2
        {
            let pathl1 = &.paths[i + 1]

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
            if !Math.isZeroEpsilon(bSize) and .borderColor.a
            {
                painter.pushTransform()
                painter.curState.transform.tx = 0
                painter.curState.transform.ty = 0
                painter.scaleTransform(zscale, zscale)
                painter.translateTransform(tt.tx, tt.ty)
                painter.translateTransform(x0, y0 - (pathl1.bvMin.y * zscale))
                Utf8.visitRunes(|c| tokens[i / 2])
                {
                    let it = .runes.find(c)
                    pl.paths.add(it.value.border.paths.toSlice())
                    painter.fillPath(&pl, .borderColor)
                    pl.paths.count = 0
                    painter.translateTransform(cast(f32) it.value.advance * zscale, 0)
                }
                painter.popTransform()
            }

            // Back
            if .backColor.a
            {
                painter.pushTransform()
                painter.curState.transform.tx = 0
                painter.curState.transform.ty = 0
                painter.scaleTransform(zscale, zscale)
                painter.translateTransform(tt.tx, tt.ty)
                painter.translateTransform(x0, y0 - (pathl1.bvMin.y * zscale))
                Utf8.visitRunes(|c| tokens[i / 2])
                {
                    let it = .runes.find(c)
                    pl.paths.add(it.value.back.paths.toSlice())
                    painter.fillPath(&pl, .backColor)
                    pl.paths.count = 0
                    painter.translateTransform(cast(f32) it.value.advance * zscale, 0)
                }
                painter.popTransform()
            }

            // Next line
            y0 += ht + (.lineSpacing * zscale)
        }
    }

    mtd impl setup(capt: *Capture)
    {
        .capture = capt

        with me.gizmo
        {
            .roundShape        = true
            .anchorSize        = 10
            .trackChange       = true
            .paintSimpleBorder = false
            .paintBigBorder    = true
        }

        .gizmo.sigChanged = mtd||(g, first, hitMove)
        {
            g.apply(me, first, hitMove)
        }
    }
}
