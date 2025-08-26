#global marked
impl MainWnd
{
    mtd updateModel(f: IForm)
    {
        let form = cast(*BaseForm) @dataof(f)

        if form.subKind == .Highlight:
            me.mdlHighlight = f
        elif form.subKind == .PixSel:
            me.mdlPixSel = f

        #[Swag.Complete]
        switch f
        {
        case FormShape as b:
            me.mdlFormShape = dref b
            me.mdlFavorite  = &me.mdlFormShape
            me.mdlHighlight = &me.mdlFormShape
            me.mdlPixSel    = &me.mdlFormShape
        case FormImage as b:
            me.mdlFormImage = dref b
            me.mdlFavorite  = &me.mdlFormImage
        case FormStep as b:
            me.mdlFormStep    = dref b
            me.mdlFormStep.ws = b.w
            me.mdlFormStep.hs = b.h
            me.mdlFavorite    = &me.mdlFormStep
        case FormArrow as b:
            me.mdlFormArrow = dref b
            me.mdlFavorite  = &me.mdlFormArrow
        case FormLine as b:
            me.mdlFormLine  = dref b
            me.mdlFavorite  = &me.mdlFormLine
            me.mdlHighlight = &me.mdlFormLine
        case FormPolygon as b:
            me.mdlFormPolygon = dref b
            me.mdlFavorite    = &me.mdlFormPolygon
            me.mdlHighlight   = &me.mdlFormPolygon
            me.mdlPixSel      = &me.mdlFormPolygon
        case FormText as b:
            me.mdlFormText = dref b
            me.mdlFavorite = &me.mdlFormText
        case FormDraw as b:
            me.mdlFormDraw  = dref b
            me.mdlFavorite  = &me.mdlFormDraw
            me.mdlHighlight = &me.mdlFormDraw
            me.mdlPixSel    = &me.mdlFormDraw
        }
    }

    mtd updateEditMode(f: IForm)
    {
        if me.editMode == .Favorites
        {
            #[Swag.Complete]
            switch f
            {
            case FormShape:
                me.mdlFavorite = me.mdlFormShape
            case FormImage:
                me.mdlFavorite = me.mdlFormImage
            case FormStep:
                me.mdlFavorite = me.mdlFormStep
            case FormArrow:
                me.mdlFavorite = me.mdlFormArrow
            case FormLine:
                me.mdlFavorite = me.mdlFormLine
            case FormPolygon:
                me.mdlFavorite = me.mdlFormPolygon
            case FormText:
                me.mdlFavorite = me.mdlFormText
            case FormDraw:
                me.mdlFavorite = me.mdlFormDraw
            }

            return
        }

        let form = cast(*BaseForm) @dataof(f)
        if form.subKind == .Highlight
        {
            me.editMode = .Highlight
            return
        }
        elif form.subKind == .PixSel
        {
            me.editMode = .PixSel
            return
        }

        #[Swag.Complete]
        switch f
        {
        case FormShape:
            me.editMode = .Shape
        case FormImage:
            me.editMode = .Image
        case FormStep:
            me.editMode = .Step
        case FormArrow:
            me.editMode = .Arrow
        case FormLine:
            me.editMode = .Line
        case FormPolygon:
            me.editMode = .Polygon
        case FormText:
            me.editMode = .Text
        case FormDraw:
            me.editMode = .Draw
        }
    }

    func formToEditMode(f: IForm)->EditMode
    {
        #[Swag.Complete]
        switch f
        {
        case FormShape:
            return .Shape
        case FormArrow:
            return .Arrow
        case FormLine:
            return .Line
        case FormPolygon:
            return .Polygon
        case FormText:
            return .Text
        case FormDraw:
            return .Draw
        case FormStep:
            return .Step
        case FormImage:
            return .Image
        }

        unreachable
    }

    mtd editModeToModel(mode: EditMode)->#null IForm
    {
        var formItf: IForm

        #[Swag.Complete]
        switch mode
        {
        case Arrow:
            formItf = cast(IForm) me.mdlFormArrow
        case Shape:
            formItf = cast(IForm) me.mdlFormShape
        case Image:
            formItf = cast(IForm) me.mdlFormImage
        case Step:
            formItf = cast(IForm) me.mdlFormStep
        case Line:
            formItf = cast(IForm) me.mdlFormLine
        case Polygon:
            formItf = cast(IForm) me.mdlFormPolygon
        case Text:
            formItf = cast(IForm) me.mdlFormText
        case Draw:
            formItf = cast(IForm) me.mdlFormDraw
        case Favorites, Selection:
            formItf = me.mdlFavorite
        case Highlight:
            formItf = me.mdlHighlight
        case PixSel:
            formItf = me.mdlPixSel
        }

        return formItf
    }

    mtd newFormFromModel(mode: EditMode, clear = false)->#null IForm
    {
        #[Swag.Complete]
        switch mode
        {
        case Arrow:
            let mdl = Memory.new'FormArrow()
            mdl.IForm.copy(mdl, &me.mdlFormArrow)
            return mdl
        case Shape:
            let mdl = Memory.new'FormShape()
            mdl.IForm.copy(mdl, &me.mdlFormShape)
            return mdl
        case Image:
            let mdl = Memory.new'FormImage()
            mdl.IForm.copy(mdl, &me.mdlFormImage)
            if clear:
                mdl.image.release()
            return mdl
        case Step:
            let mdl = Memory.new'FormStep()
            mdl.IForm.copy(mdl, &me.mdlFormStep)
            return mdl
        case Line:
            let mdl = Memory.new'FormLine()
            mdl.IForm.copy(mdl, &me.mdlFormLine)
            return mdl
        case Polygon:
            let mdl = Memory.new'FormPolygon()
            mdl.IForm.copy(mdl, &me.mdlFormPolygon)
            if clear:
                mdl.points.clear()
            return mdl
        case Text:
            let mdl = Memory.new'FormText()
            mdl.IForm.copy(mdl, &me.mdlFormText)
            return mdl
        case Draw:
            let mdl = Memory.new'FormDraw()
            mdl.IForm.copy(mdl, &me.mdlFormDraw)
            if clear:
                mdl.points.clear()
            return mdl
        case Favorites, Highlight, Selection, PixSel:
            break
        }

        unreachable
    }

    func newFormFromForm(it: IForm)->*BaseForm
    {
        var mdlValue: *BaseForm

        #[Swag.Complete]
        switch it
        {
        case FormShape as b:
            mdlValue = Memory.new'FormShape()
            it.copy(mdlValue, b)
        case FormImage as b:
            mdlValue = Memory.new'FormImage()
            it.copy(mdlValue, b)
        case FormStep as b:
            mdlValue = Memory.new'FormStep()
            it.copy(mdlValue, b)
        case FormLine as b:
            mdlValue = Memory.new'FormLine()
            it.copy(mdlValue, b)
        case FormPolygon as b:
            mdlValue = Memory.new'FormPolygon()
            it.copy(mdlValue, b)
        case FormArrow as b:
            mdlValue = Memory.new'FormArrow()
            it.copy(mdlValue, b)
        case FormText as b:
            mdlValue = Memory.new'FormText()
            it.copy(mdlValue, b)
        case FormDraw as b:
            mdlValue = Memory.new'FormDraw()
            it.copy(mdlValue, b)
        }

        return mdlValue
    }
}
