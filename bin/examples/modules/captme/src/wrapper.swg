impl MainWnd
{
    mtd updateModel(f: IForm)
    { 
        let form = cast(*BaseForm) @dataof(f)
        
        if form.subKind == .Highlight:
            mdlHighlight = f
        elif form.subKind == .PixSel:
            mdlPixSel = f

        #[Swag.Complete]
        switch f
        {
        case FormShape as b:
            mdlFormShape = dref b
            mdlFavorite  = &mdlFormShape
            mdlHighlight = &mdlFormShape
            mdlPixSel    = &mdlFormShape
        case FormImage as b:
            mdlFormImage = dref b
            mdlFavorite  = &mdlFormImage
        case FormStep as b:
            mdlFormStep    = dref b
            mdlFormStep.ws = b.w
            mdlFormStep.hs = b.h
            mdlFavorite = &mdlFormStep
        case FormArrow as b:
            mdlFormArrow = dref b
            mdlFavorite  = &mdlFormArrow
        case FormLine as b:
            mdlFormLine = dref b
            mdlFavorite  = &mdlFormLine
            mdlHighlight = &mdlFormLine
        case FormPolygon as b:
            mdlFormPolygon = dref b
            mdlFavorite    = &mdlFormPolygon
            mdlHighlight   = &mdlFormPolygon
            mdlPixSel      = &mdlFormPolygon
        case FormText as b:
            mdlFormText = dref b
            mdlFavorite = &mdlFormText
        case FormDraw as b:
            mdlFormDraw = dref b
            mdlFavorite  = &mdlFormDraw
            mdlHighlight = &mdlFormDraw
            mdlPixSel    = &mdlFormDraw
        }               
    }

    mtd updateEditMode(f: IForm)
    {
        if editMode == .Favorites
        {
            #[Swag.Complete]
            switch f
            {
            case FormShape:
                mdlFavorite = mdlFormShape
            case FormImage:
                mdlFavorite = mdlFormImage
            case FormStep:
                mdlFavorite = mdlFormStep
            case FormArrow:
                mdlFavorite = mdlFormArrow
            case FormLine:
                mdlFavorite = mdlFormLine
            case FormPolygon:
                mdlFavorite = mdlFormPolygon
            case FormText:
                mdlFavorite = mdlFormText
            case FormDraw:
                mdlFavorite = mdlFormDraw
            }

            return
        }

        let form = cast(*BaseForm) @dataof(f)
        if form.subKind == .Highlight
        {
            editMode = .Highlight
            return
        }
        elif form.subKind == .PixSel
        {
            editMode = .PixSel
            return
        }

        #[Swag.Complete]
        switch f
        {
        case FormShape:
            editMode = .Shape
        case FormImage:
            editMode = .Image
        case FormStep:
            editMode = .Step
        case FormArrow:
            editMode = .Arrow
        case FormLine:
            editMode = .Line
        case FormPolygon:
            editMode = .Polygon
        case FormText:
            editMode = .Text
        case FormDraw:
            editMode = .Draw
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

    mtd editModeToForm(mode: EditMode)->nl IForm
    {  
        var formItf: IForm

        #[Swag.Complete]
        switch mode
        {
        case Arrow:
            formItf = cast(IForm) mdlFormArrow
        case Shape:
            formItf = cast(IForm) mdlFormShape
        case Image:
            formItf = cast(IForm) mdlFormImage
        case Step:
            formItf = cast(IForm) mdlFormStep
        case Line:
            formItf = cast(IForm) mdlFormLine
        case Polygon:
            formItf = cast(IForm) mdlFormPolygon
        case Text:
            formItf = cast(IForm) mdlFormText
        case Draw:
            formItf = cast(IForm) mdlFormDraw
        case Favorites:
            formItf = mdlFavorite
        case Highlight:
            formItf = mdlHighlight
        case PixSel:
            formItf = mdlPixSel
        case Selection:
            break
        }   

        return formItf           
    }
}
