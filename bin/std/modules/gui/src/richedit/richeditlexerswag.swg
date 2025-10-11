#global public
using Core, Pixel

struct RichEditLexerSwag
{
    #[Swag.Incomplete]
    enum Style: RichEditStyleRef
    {
        Default
        Comment
        Keyword
        Control
        Intrinsic
        Buildin
        Function
        Preprocessor
        CharEscape
        Type
        Class
        String
        Character
        Numeric
        Error
    }

    #[Swag.Incomplete]
    enum State: s32
    {
        Default
        LineComment
        Comment
        Keyword
        Declaration
        Control
        Intrinsic
        Buildin
        Compiler
        CompilerFunction
        CompilerGlobal
        CompilerStatement
        Modifier
        FunctionCall
        FunctionSpecOp
        Type
        StorageModifier
        UserType
        Constant
        String
        RawString
        Character
        Numeric
        NumericHex
        NumericBin
        NumericFrac
        Attribute
        Error
    }

    mapStyles:     [256] RichEditStyleRef
    mapWords:      HashTable'(string, s32)
}

impl IRichEditLexer for RichEditLexerSwag
{
    mtd impl setup(ed: *RichEditCtrl)
    {
        using RichEditLexerSwag.Style
        .mapStyles[State.LineComment]       = Comment
        .mapStyles[State.Comment]           = Comment
        .mapStyles[State.Keyword]           = Keyword
        .mapStyles[State.Control]           = Control
        .mapStyles[State.Declaration]       = Keyword
        .mapStyles[State.Intrinsic]         = Intrinsic
        .mapStyles[State.Buildin]           = Buildin
        .mapStyles[State.CompilerFunction]  = Function
        .mapStyles[State.CompilerGlobal]    = Preprocessor
        .mapStyles[State.CompilerStatement] = Preprocessor
        .mapStyles[State.Compiler]          = CharEscape
        .mapStyles[State.Modifier]          = Keyword
        .mapStyles[State.FunctionSpecOp]    = Intrinsic
        .mapStyles[State.Type]              = Type
        .mapStyles[State.StorageModifier]   = Keyword
        .mapStyles[State.UserType]          = Class
        .mapStyles[State.FunctionCall]      = Function
        .mapStyles[State.Constant]          = Keyword
        .mapStyles[State.String]            = String
        .mapStyles[State.RawString]         = String
        .mapStyles[State.Character]         = Character
        .mapStyles[State.Numeric]           = Numeric
        .mapStyles[State.NumericFrac]       = Numeric
        .mapStyles[State.NumericHex]        = Numeric
        .mapStyles[State.NumericBin]        = Numeric
        .mapStyles[State.Attribute]         = Preprocessor
        .mapStyles[State.Error]             = Error

        .mapWords.add("if", State.Control)
        .mapWords.add("else", State.Control)
        .mapWords.add("elif", State.Control)
        .mapWords.add("for", State.Control)
        .mapWords.add("while", State.Control)
        .mapWords.add("switch", State.Control)
        .mapWords.add("defer", State.Control)
        .mapWords.add("foreach", State.Control)
        .mapWords.add("where", State.Control)
        .mapWords.add("break", State.Control)
        .mapWords.add("fallthrough", State.Control)
        .mapWords.add("unreachable", State.Control)
        .mapWords.add("return", State.Control)
        .mapWords.add("case", State.Control)
        .mapWords.add("continue", State.Control)
        .mapWords.add("default", State.Control)
        .mapWords.add("and", State.Control)
        .mapWords.add("or", State.Control)
        .mapWords.add("orelse", State.Control)
        .mapWords.add("to", State.Control)
        .mapWords.add("until", State.Control)
        .mapWords.add("in", State.Control)
        .mapWords.add("as", State.Control)
        .mapWords.add("is", State.Control)

        .mapWords.add("using", State.Keyword)
        .mapWords.add("with", State.Keyword)
        .mapWords.add("cast", State.Keyword)
        .mapWords.add("dref", State.Keyword)
        .mapWords.add("moveref", State.Declaration)
        .mapWords.add("retval", State.Keyword)
        .mapWords.add("try", State.Keyword)
        .mapWords.add("catch", State.Keyword)
        .mapWords.add("trycatch", State.Keyword)
        .mapWords.add("assume", State.Keyword)
        .mapWords.add("throw", State.Keyword)
        .mapWords.add("discard", State.Keyword)

        .mapWords.add("enum", State.Declaration)
        .mapWords.add("struct", State.Declaration)
        .mapWords.add("union", State.Declaration)
        .mapWords.add("impl", State.Declaration)
        .mapWords.add("interface", State.Declaration)
        .mapWords.add("func", State.Declaration)
        .mapWords.add("mtd", State.Declaration)
        .mapWords.add("namespace", State.Declaration)
        .mapWords.add("alias", State.Declaration)
        .mapWords.add("attr", State.Declaration)

        .mapWords.add("let", State.Declaration)
        .mapWords.add("var", State.Declaration)
        .mapWords.add("const", State.Declaration)

        .mapWords.add("public", State.StorageModifier)
        .mapWords.add("internal", State.StorageModifier)
        .mapWords.add("private", State.StorageModifier)

        .mapWords.add("true", State.Constant)
        .mapWords.add("false", State.Constant)
        .mapWords.add("null", State.Constant)
        .mapWords.add("undefined", State.Constant)

        .mapWords.add("not", State.Error)
        .mapWords.add("do", State.Error)

        .mapWords.add("#defined", State.Intrinsic)
        .mapWords.add("#offsetof", State.Intrinsic)
        .mapWords.add("#alignof", State.Intrinsic)
        .mapWords.add("#sizeof", State.Intrinsic)
        .mapWords.add("#typeof", State.Intrinsic)
        .mapWords.add("#stringof", State.Intrinsic)
        .mapWords.add("#nameof", State.Intrinsic)
        .mapWords.add("#isconstexpr", State.Intrinsic)
        .mapWords.add("#location", State.Intrinsic)
        .mapWords.add("#decltype", State.Intrinsic)
        .mapWords.add("#hastag", State.Intrinsic)
        .mapWords.add("#gettag", State.Intrinsic)
        .mapWords.add("#runes", State.Intrinsic)
        .mapWords.add("#safety", State.Intrinsic)
        .mapWords.add("#include", State.Intrinsic)

        .mapWords.add("#uniq0", State.Intrinsic)
        .mapWords.add("#uniq1", State.Intrinsic)
        .mapWords.add("#uniq2", State.Intrinsic)
        .mapWords.add("#uniq3", State.Intrinsic)
        .mapWords.add("#uniq4", State.Intrinsic)
        .mapWords.add("#uniq5", State.Intrinsic)
        .mapWords.add("#uniq6", State.Intrinsic)
        .mapWords.add("#uniq7", State.Intrinsic)
        .mapWords.add("#uniq8", State.Intrinsic)
        .mapWords.add("#uniq9", State.Intrinsic)
        .mapWords.add("#alias0", State.Intrinsic)
        .mapWords.add("#alias1", State.Intrinsic)
        .mapWords.add("#alias2", State.Intrinsic)
        .mapWords.add("#alias3", State.Intrinsic)
        .mapWords.add("#alias4", State.Intrinsic)
        .mapWords.add("#alias5", State.Intrinsic)
        .mapWords.add("#alias6", State.Intrinsic)
        .mapWords.add("#alias7", State.Intrinsic)
        .mapWords.add("#alias8", State.Intrinsic)
        .mapWords.add("#alias9", State.Intrinsic)

        .mapWords.add("@rtflags", State.Intrinsic)
        .mapWords.add("@err", State.Intrinsic)
        .mapWords.add("@args", State.Intrinsic)
        .mapWords.add("@pinfos", State.Intrinsic)
        .mapWords.add("@bytecode", State.Intrinsic)
        .mapWords.add("@compiler", State.Intrinsic)
        .mapWords.add("@modules", State.Intrinsic)
        .mapWords.add("@gvtd", State.Intrinsic)
        .mapWords.add("@index", State.Intrinsic)

        .mapWords.add("@kindof", State.Intrinsic)
        .mapWords.add("@countof", State.Intrinsic)
        .mapWords.add("@dataof", State.Intrinsic)
        .mapWords.add("@mkslice", State.Intrinsic)
        .mapWords.add("@mkstring", State.Intrinsic)
        .mapWords.add("@mkcallback", State.Intrinsic)
        .mapWords.add("@mkany", State.Intrinsic)
        .mapWords.add("@mkinterface", State.Intrinsic)
        .mapWords.add("@dbgalloc", State.Intrinsic)
        .mapWords.add("@sysalloc", State.Intrinsic)
        .mapWords.add("@stringcmp", State.Intrinsic)
        .mapWords.add("@typecmp", State.Intrinsic)
        .mapWords.add("@is", State.Intrinsic)
        .mapWords.add("@as", State.Intrinsic)
        .mapWords.add("@getcontext", State.Intrinsic)
        .mapWords.add("@tableof", State.Intrinsic)

        .mapWords.add("@assert", State.Intrinsic)
        .mapWords.add("@breakpoint", State.Intrinsic)
        .mapWords.add("@init", State.Intrinsic)
        .mapWords.add("@drop", State.Intrinsic)
        .mapWords.add("@postcopy", State.Intrinsic)
        .mapWords.add("@postmove", State.Intrinsic)
        .mapWords.add("@compilererror", State.Intrinsic)
        .mapWords.add("@compilerwarning", State.Intrinsic)
        .mapWords.add("@panic", State.Intrinsic)
        .mapWords.add("@print", State.Intrinsic)
        .mapWords.add("@setcontext", State.Intrinsic)

        .mapWords.add("opSlice", State.FunctionSpecOp)
        .mapWords.add("opBinary", State.FunctionSpecOp)
        .mapWords.add("opUnary", State.FunctionSpecOp)
        .mapWords.add("opAssign", State.FunctionSpecOp)
        .mapWords.add("opIndexAssign", State.FunctionSpecOp)
        .mapWords.add("opIndexAffect", State.FunctionSpecOp)
        .mapWords.add("opCast", State.FunctionSpecOp)
        .mapWords.add("opCount", State.FunctionSpecOp)
        .mapWords.add("opData", State.FunctionSpecOp)
        .mapWords.add("opVisit", State.FunctionSpecOp)
        .mapWords.add("opInit", State.FunctionSpecOp)
        .mapWords.add("opReloc", State.FunctionSpecOp)
        .mapWords.add("opEquals", State.FunctionSpecOp)
        .mapWords.add("opCmp", State.FunctionSpecOp)
        .mapWords.add("opPostCopy", State.FunctionSpecOp)
        .mapWords.add("opPostMove", State.FunctionSpecOp)
        .mapWords.add("opDrop", State.FunctionSpecOp)
        .mapWords.add("opCount", State.FunctionSpecOp)
        .mapWords.add("opAffect", State.FunctionSpecOp)
        .mapWords.add("opAffectLiteral", State.FunctionSpecOp)
        .mapWords.add("opIndex", State.FunctionSpecOp)
        .mapWords.add("opIndexAssign", State.FunctionSpecOp)

        .mapWords.add("me", State.Type)
        .mapWords.add("cvarargs", State.Type)

        .mapWords.add("any", State.Type)
        .mapWords.add("void", State.Type)
        .mapWords.add("rune", State.Type)
        .mapWords.add("f32", State.Type)
        .mapWords.add("f64", State.Type)
        .mapWords.add("s8", State.Type)
        .mapWords.add("s16", State.Type)
        .mapWords.add("s32", State.Type)
        .mapWords.add("s64", State.Type)
        .mapWords.add("u8", State.Type)
        .mapWords.add("u16", State.Type)
        .mapWords.add("u32", State.Type)
        .mapWords.add("u64", State.Type)
        .mapWords.add("bool", State.Type)
        .mapWords.add("string", State.Type)
        .mapWords.add("cstring", State.Type)
        .mapWords.add("typeinfo", State.Type)

        .mapWords.add("@abs", State.Buildin)
        .mapWords.add("@acos", State.Buildin)
        .mapWords.add("@asin", State.Buildin)
        .mapWords.add("@atan", State.Buildin)
        .mapWords.add("@atan2", State.Buildin)
        .mapWords.add("@alloc", State.Buildin)
        .mapWords.add("@atomadd", State.Buildin)
        .mapWords.add("@atomand", State.Buildin)
        .mapWords.add("@atomcmpxchg", State.Buildin)
        .mapWords.add("@atomor", State.Buildin)
        .mapWords.add("@atomxchg", State.Buildin)
        .mapWords.add("@atomxor", State.Buildin)
        .mapWords.add("@bitcountnz", State.Buildin)
        .mapWords.add("@bitcountlz", State.Buildin)
        .mapWords.add("@bitcounttz", State.Buildin)
        .mapWords.add("@byteswap", State.Buildin)
        .mapWords.add("@cvaarg", State.Buildin)
        .mapWords.add("@cvaend", State.Buildin)
        .mapWords.add("@cvastart", State.Buildin)
        .mapWords.add("@ceil", State.Buildin)
        .mapWords.add("@cos", State.Buildin)
        .mapWords.add("@cosh", State.Buildin)
        .mapWords.add("@exp", State.Buildin)
        .mapWords.add("@exp2", State.Buildin)
        .mapWords.add("@floor", State.Buildin)
        .mapWords.add("@free", State.Buildin)
        .mapWords.add("@log", State.Buildin)
        .mapWords.add("@log10", State.Buildin)
        .mapWords.add("@log2", State.Buildin)
        .mapWords.add("@max", State.Buildin)
        .mapWords.add("@memcmp", State.Buildin)
        .mapWords.add("@memcpy", State.Buildin)
        .mapWords.add("@memmove", State.Buildin)
        .mapWords.add("@memset", State.Buildin)
        .mapWords.add("@min", State.Buildin)
        .mapWords.add("@muladd", State.Buildin)
        .mapWords.add("@pow", State.Buildin)
        .mapWords.add("@realloc", State.Buildin)
        .mapWords.add("@rol", State.Buildin)
        .mapWords.add("@ror", State.Buildin)
        .mapWords.add("@round", State.Buildin)
        .mapWords.add("@sin", State.Buildin)
        .mapWords.add("@sinh", State.Buildin)
        .mapWords.add("@sqrt", State.Buildin)
        .mapWords.add("@strcmp", State.Buildin)
        .mapWords.add("@strlen", State.Buildin)
        .mapWords.add("@tan", State.Buildin)
        .mapWords.add("@tanh", State.Buildin)
        .mapWords.add("@trunc", State.Buildin)

        .mapWords.add("#run", State.CompilerFunction)
        .mapWords.add("#test", State.CompilerFunction)
        .mapWords.add("#init", State.CompilerFunction)
        .mapWords.add("#drop", State.CompilerFunction)
        .mapWords.add("#premain", State.CompilerFunction)
        .mapWords.add("#main", State.CompilerFunction)
        .mapWords.add("#message", State.CompilerFunction)
        .mapWords.add("#ast", State.CompilerFunction)
        .mapWords.add("#dependencies", State.CompilerFunction)

        .mapWords.add("#global", State.CompilerGlobal)
        .mapWords.add("#load", State.CompilerStatement)
        .mapWords.add("#foreignlib", State.CompilerStatement)
        .mapWords.add("#assert", State.CompilerStatement)
        .mapWords.add("#print", State.CompilerStatement)
        .mapWords.add("#error", State.CompilerStatement)
        .mapWords.add("#warning", State.CompilerStatement)
        .mapWords.add("#import", State.CompilerStatement)
        .mapWords.add("#inject", State.CompilerStatement)
        .mapWords.add("#macro", State.CompilerStatement)
        .mapWords.add("#if", State.CompilerStatement)
        .mapWords.add("#else", State.CompilerStatement)
        .mapWords.add("#elif", State.CompilerStatement)
        .mapWords.add("#scope", State.CompilerStatement)

        .mapWords.add("#cfg", State.Compiler)
        .mapWords.add("#os", State.Compiler)
        .mapWords.add("#arch", State.Compiler)
        .mapWords.add("#cpu", State.Compiler)
        .mapWords.add("#backend", State.Compiler)
        .mapWords.add("#me", State.Compiler)
        .mapWords.add("#module", State.Compiler)
        .mapWords.add("#file", State.Compiler)
        .mapWords.add("#line", State.Compiler)
        .mapWords.add("#curlocation", State.Compiler)
        .mapWords.add("#callerlocation", State.Compiler)
        .mapWords.add("#callerfunction", State.Compiler)
        .mapWords.add("#swagversion", State.Compiler)
        .mapWords.add("#swagrevision", State.Compiler)
        .mapWords.add("#swagbuildnum", State.Compiler)
        .mapWords.add("#swagos", State.Compiler)

        .mapWords.add("#code", State.Intrinsic)
        .mapWords.add("#type", State.Intrinsic)
        .mapWords.add("#up", State.Intrinsic)

        .mapWords.add("#prom", State.Modifier)
        .mapWords.add("#wrap", State.Modifier)
        .mapWords.add("#nodrop", State.Modifier)
        .mapWords.add("#move", State.Modifier)
        .mapWords.add("#moveraw", State.Modifier)
        .mapWords.add("#reverse", State.Modifier)
        .mapWords.add("#ref", State.Modifier)
        .mapWords.add("#constref", State.Modifier)
        .mapWords.add("#null", State.Modifier)
    }

    mtd impl setupTheme(ed: *RichEditCtrl)
    {
        using RichEditLexerSwag.Style
        ed.styles[Comment].colFg      = 0xFF6A9955
        ed.styles[Keyword].colFg      = 0xFF569cd6
        ed.styles[Control].colFg      = 0xFFD8A0DF
        ed.styles[Intrinsic].colFg    = 0xFFDCDCAA
        ed.styles[Buildin].colFg      = 0xFFF67F56
        ed.styles[Function].colFg     = 0xFFFF7411
        ed.styles[Preprocessor].colFg = 0xFFAAAAAA
        ed.styles[CharEscape].colFg   = 0xFFD7BA7D
        ed.styles[Type].colFg         = 0xFFF6CC86
        ed.styles[Class].colFg        = 0xFF4EC9B0
        ed.styles[String].colFg       = 0xFFCE9178
        ed.styles[Character].colFg    = 0xFFCE9178
        ed.styles[Numeric].colFg      = 0xFFB5CEA8
        ed.styles[Error].colFg        = 0xFFF44747
    }

    mtd impl insertRune(ed: *RichEditCtrl, ch: rune)->bool
    {
        // Smart return
        if ch == '\n'
        {
            let cp   = ed.getBegSelection()
            let line = ed.lines[cp.lineIndex]
            var str: String = "\n"

            if cp.charPosInLine
            {
                var cpt = 0
                while cpt < line.chars.count and Unicode.isSpace(line.chars[cpt])
                {
                    str += line.chars[cpt]
                    cpt += 1
                }

                if cpt < line.chars.count and line.chars[cpt] == '{' do
                    str += '\t''rune
            }

            ed.pushUndo()
            ed.insertText(str)
            ed.popUndo()
            ed.ensureCaretIsVisible()
            return true
        }

        return false
    }

    mtd impl compute(linePrev: *RichEditLine, line: *RichEditLine, lineNext: *RichEditLine)
    {
        var seek          = 0'u64
        var currentState  = linePrev ? linePrev.lexerEndingState : State.Default
        var currentCptCmt = linePrev ? linePrev.lexerCptCmt : 0

        #[Swag.Mixin]
        func getCur()->rune
        {
            if seek >= line.chars.count do
                return 0
            return line.chars[seek]
        }

        #[Swag.Mixin]
        func getNext()->rune
        {
            if !line.chars.count do
                return 0
            if seek >= line.chars.count - 1 do
                return 0
            return line.chars[seek + 1]
        }

        #[Swag.Mixin]
        mtd seekNext()
        {
            if seek >= line.chars.count do
                return
            line.styles[seek] = .mapStyles[currentState]
            seek += 1
        }

        for
        {
            var c  = getCur()
            let cn = getNext()
            if !c
            {
                switch currentState
                {
                case State.NumericFrac, State.Numeric, State.NumericHex, State.NumericBin:
                    currentState = State.Default
                }

                break
            }

            switch currentState
            {
            case State.NumericFrac:
                if Unicode.isNumber(c) or c == '_'
                {
                    .seekNext()
                    continue
                }

                if c == 'e'
                {
                    .seekNext()
                    if cn == '+' or cn == '-' do
                        .seekNext()
                    currentState = State.Numeric
                    continue
                }

                currentState = State.Default

            case State.NumericHex:
                if Unicode.isNumber(c) or c == '_'
                {
                    .seekNext()
                    continue
                }

                if !Unicode.isLatin1(c) or !Latin1.isHexDigit(cast(u8) c)
                {
                    currentState = State.Default
                    .seekNext()
                    continue
                }

            case State.NumericBin:
                if Unicode.isNumber(c) or c == '_'
                {
                    .seekNext()
                    continue
                }

                if c != '0' and c != '1'
                {
                    currentState = State.Default
                    .seekNext()
                    if getCur() == '\'' do
                        .seekNext()
                    continue
                }

            case State.Numeric:
                if c == '.' and cn == '.'
                {
                    currentState = State.Default
                    .seekNext()
                    continue
                }

                if c == '.' or c == 'e' or c == 'E'
                {
                    currentState = State.NumericFrac
                    .seekNext()
                    continue
                }

                if !Unicode.isNumber(c) and c != '.'
                {
                    currentState = State.Default
                    .seekNext()
                    if getCur() == '\'' do
                        .seekNext()
                    continue
                }

            case State.String:
                if c == '\\'
                {
                    .seekNext()
                    .seekNext()
                    continue
                }

                if c == '"'
                {
                    .seekNext()
                    currentState = State.Default
                    if getCur() == '\'' do
                        .seekNext()
                    continue
                }

            case State.RawString:
                if c == '"' and cn == '@'
                {
                    .seekNext()
                    .seekNext()
                    currentState = State.Default
                    continue
                }

            case State.Character:
                if c == '\\'
                {
                    .seekNext()
                    .seekNext()
                    continue
                }

                if c == '\''
                {
                    .seekNext()
                    currentState = State.Default
                    continue
                }

            case State.Comment:
                if c == '*' and cn == '/'
                {
                    .seekNext()
                    .seekNext()
                    if currentCptCmt do
                        currentCptCmt -= 1
                    if !currentCptCmt do
                        currentState = State.Default
                    continue
                }

                if c == '/' and cn == '*'
                {
                    currentCptCmt += 1
                    .seekNext()
                    .seekNext()
                    continue
                }

            case State.Attribute:
                if c == ']'
                {
                    .seekNext()
                    currentState = State.Default
                    continue
                }

            case State.Default:
                if c == '#' and cn == '['
                {
                    currentState = State.Attribute
                    .seekNext()
                    .seekNext()
                    continue
                }

                if c == '@' and cn == '"'
                {
                    currentState = State.RawString
                    .seekNext()
                    .seekNext()
                    continue
                }

                if Unicode.isLetter(c) or c == '_' or c == '@' or c == '#'
                {
                    var word:    String
                    let saveSeek = seek
                    while Unicode.isLetterOrDigit(c) or c == '_' or c == '@' or c == '#'
                    {
                        word += c
                        seek += 1
                        c = getCur()
                    }

                    let seekEnd = seek
                    seek = saveSeek

                    if Utf8.startsWith(word, "opVisit") do
                        currentState = State.FunctionSpecOp
                    elif !word.isEmpty()
                    {
                        let res = .mapWords.tryFind(word.toString())
                        if res do
                            currentState = res.value
                        elif word == "@" do
                            currentState = State.Keyword
                        elif Latin1.isUpper(word[0]) do
                            currentState = State.UserType
                        elif c == '(' and (Latin1.isLetter(word[0]) or word[0] == '_') do
                            currentState = State.FunctionCall
                        else do
                            currentState = State.Default
                    }

                    while seek != seekEnd do
                        .seekNext()
                    currentState = State.Default
                    if c == '\'' do
                        .seekNext()
                    continue
                }

                if c == '"'
                {
                    currentState = State.String
                    .seekNext()
                    continue
                }

                if c == '\''
                {
                    currentState = State.Character
                    .seekNext()
                    continue
                }

                if Unicode.isNumber(c) and Unicode.toLower(cn) == 'b'
                {
                    currentState = State.NumericBin
                    .seekNext()
                    .seekNext()
                    continue
                }

                if Unicode.isNumber(c) and Unicode.toLower(cn) == 'x'
                {
                    currentState = State.NumericHex
                    .seekNext()
                    .seekNext()
                    continue
                }

                if Unicode.isNumber(c)
                {
                    currentState = State.Numeric
                    .seekNext()
                    continue
                }

                if c == '/' and cn == '*'
                {
                    currentCptCmt += 1
                    currentState = State.Comment
                    .seekNext()
                    .seekNext()
                    continue
                }

                if c == '/' and cn == c
                {
                    currentState = State.LineComment
                    .seekNext()
                    .seekNext()
                    continue
                }
            }

            .seekNext()
        }

        if currentState == State.LineComment do
            currentState = State.Default

        if line.lexerEndingState != currentState
        {
            line.lexerEndingState = currentState
            if lineNext do
                lineNext.dirtyStyle = true
        }

        if line.lexerCptCmt != currentCptCmt
        {
            line.lexerCptCmt = currentCptCmt
            if lineNext do
                lineNext.dirtyStyle = true
        }
    }
}
