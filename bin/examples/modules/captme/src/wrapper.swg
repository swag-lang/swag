impl MainWnd
{
    mtd updateModelValues(f: IForm)
    {
        switch f
        {
        case FormStep as b:
            .mdlFormStep    = dref b
            .mdlFormStep.ws = b.w
            .mdlFormStep.hs = b.h
        }
    }

    mtd updateModel(f: IForm)
    {
        let form = cast(*BaseForm) @dataof(f)

        if form.subKind == .Highlight do
            .mdlHighlight = f
        elif form.subKind == .PixSel do
            .mdlPixSel = f

        #[Swag.Complete]
        switch f
        {
        case FormShape as b:
            .mdlFormShape = dref b
            .mdlFavorite  = &.mdlFormShape
            .mdlHighlight = &.mdlFormShape
            .mdlPixSel    = &.mdlFormShape
        case FormImage as b:
            .mdlFormImage = dref b
            .mdlFavorite  = &.mdlFormImage
        case FormStep as b:
            .mdlFormStep    = dref b
            .mdlFormStep.ws = b.w
            .mdlFormStep.hs = b.h
            .mdlFavorite    = &.mdlFormStep
        case FormArrow as b:
            .mdlFormArrow = dref b
            .mdlFavorite  = &.mdlFormArrow
        case FormLine as b:
            .mdlFormLine  = dref b
            .mdlFavorite  = &.mdlFormLine
            .mdlHighlight = &.mdlFormLine
        case FormPolygon as b:
            .mdlFormPolygon = dref b
            .mdlFavorite    = &.mdlFormPolygon
            .mdlHighlight   = &.mdlFormPolygon
            .mdlPixSel      = &.mdlFormPolygon
        case FormText as b:
            .mdlFormText = dref b
            .mdlFavorite = &.mdlFormText
        case FormDraw as b:
            .mdlFormDraw  = dref b
            .mdlFavorite  = &.mdlFormDraw
            .mdlHighlight = &.mdlFormDraw
            .mdlPixSel    = &.mdlFormDraw
        }
    }

    mtd updateEditMode(f: IForm)
    {
        if .editMode == .Favorites
        {
            #[Swag.Complete]
            switch f
            {
            case FormShape:
                .mdlFavorite = .mdlFormShape
            case FormImage:
                .mdlFavorite = .mdlFormImage
            case FormStep:
                .mdlFavorite = .mdlFormStep
            case FormArrow:
                .mdlFavorite = .mdlFormArrow
            case FormLine:
                .mdlFavorite = .mdlFormLine
            case FormPolygon:
                .mdlFavorite = .mdlFormPolygon
            case FormText:
                .mdlFavorite = .mdlFormText
            case FormDraw:
                .mdlFavorite = .mdlFormDraw
            }

            return
        }

        let form = cast(*BaseForm) @dataof(f)
        if form.subKind == .Highlight
        {
            .editMode = .Highlight
            return
        }
        elif form.subKind == .PixSel
        {
            .editMode = .PixSel
            return
        }

        #[Swag.Complete]
        switch f
        {
        case FormShape:
            .editMode = .Shape
        case FormImage:
            .editMode = .Image
        case FormStep:
            .editMode = .Step
        case FormArrow:
            .editMode = .Arrow
        case FormLine:
            .editMode = .Line
        case FormPolygon:
            .editMode = .Polygon
        case FormText:
            .editMode = .Text
        case FormDraw:
            .editMode = .Draw
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
            formItf = cast(IForm) .mdlFormArrow
        case Shape:
            formItf = cast(IForm) .mdlFormShape
        case Image:
            formItf = cast(IForm) .mdlFormImage
        case Step:
            formItf = cast(IForm) .mdlFormStep
        case Line:
            formItf = cast(IForm) .mdlFormLine
        case Polygon:
            formItf = cast(IForm) .mdlFormPolygon
        case Text:
            formItf = cast(IForm) .mdlFormText
        case Draw:
            formItf = cast(IForm) .mdlFormDraw
        case Favorites, Selection:
            formItf = .mdlFavorite
        case Highlight:
            formItf = .mdlHighlight
        case PixSel:
            formItf = .mdlPixSel
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
            mdl.IForm.copy(mdl, &.mdlFormArrow)
            return mdl
        case Shape:
            let mdl = Memory.new'FormShape()
            mdl.IForm.copy(mdl, &.mdlFormShape)
            return mdl
        case Image:
            let mdl = Memory.new'FormImage()
            mdl.IForm.copy(mdl, &.mdlFormImage)
            if clear do
                mdl.image.release()
            return mdl
        case Step:
            let mdl = Memory.new'FormStep()
            mdl.IForm.copy(mdl, &.mdlFormStep)
            return mdl
        case Line:
            let mdl = Memory.new'FormLine()
            mdl.IForm.copy(mdl, &.mdlFormLine)
            return mdl
        case Polygon:
            let mdl = Memory.new'FormPolygon()
            mdl.IForm.copy(mdl, &.mdlFormPolygon)
            if clear do
                mdl.points.clear()
            return mdl
        case Text:
            let mdl = Memory.new'FormText()
            mdl.IForm.copy(mdl, &.mdlFormText)
            return mdl
        case Draw:
            let mdl = Memory.new'FormDraw()
            mdl.IForm.copy(mdl, &.mdlFormDraw)
            if clear do
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
