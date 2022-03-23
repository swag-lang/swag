#global public
using Pixel

struct RichEditLexerSwag
{
    currentStyle:   RichEditStyleRef
}

impl IRichEditLexer for RichEditLexerSwag
{
    #[Swag.Incomplete]
    enum SwagStyle: RichEditStyleRef
    {
        Default
        LineComment
    }

    mtd setup(ed: *RichEdit)
    {
        ed.styles[1].colFg = Argb.Red
    }

    mtd setStyleRef(style: RichEditStyleRef)
    {
        currentStyle = style
    }

    mtd getStyleRef()->RichEditStyleRef
    {
        return currentStyle
    }

    mtd compute(line: *RichEditLine)
    {
        seek := 0'uint

        #[Swag.Mixin]
        func getCur()->rune
        {
            if seek >= line.chars.count return 0
            return line.chars[seek]
        }

        #[Swag.Mixin]
        func getNext()->rune
        {
            if seek >= line.chars.count - 1 return 0
            return line.chars[seek + 1]
        }

        #[Swag.Mixin]
        func seekNext()
        {
            line.styles[seek] = currentStyle
            seek += 1
        }

        loop
        {
            c := getCur()
            if !c break
            cn := getNext()

            if currentStyle == SwagStyle.Default
            {
                if c == "/"'rune and cn == c
                {
                    currentStyle = SwagStyle.LineComment
                    seekNext()
                    seekNext()
                    continue
                }
            }

            seekNext()
        }

        if currentStyle == SwagStyle.LineComment
            currentStyle = SwagStyle.Default
    }
}