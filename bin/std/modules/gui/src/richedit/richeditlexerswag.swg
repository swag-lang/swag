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
        me.mapStyles[State.LineComment]       = Comment
        me.mapStyles[State.Comment]           = Comment
        me.mapStyles[State.Keyword]           = Keyword
        me.mapStyles[State.Control]           = Control
        me.mapStyles[State.Declaration]       = Keyword
        me.mapStyles[State.Intrinsic]         = Intrinsic
        me.mapStyles[State.Buildin]           = Buildin
        me.mapStyles[State.CompilerFunction]  = Function
        me.mapStyles[State.CompilerGlobal]    = Preprocessor
        me.mapStyles[State.CompilerStatement] = Preprocessor
        me.mapStyles[State.Compiler]          = CharEscape
        me.mapStyles[State.Modifier]          = Keyword
        me.mapStyles[State.FunctionSpecOp]    = Intrinsic
        me.mapStyles[State.Type]              = Type
        me.mapStyles[State.StorageModifier]   = Keyword
        me.mapStyles[State.UserType]          = Class
        me.mapStyles[State.FunctionCall]      = Function
        me.mapStyles[State.Constant]          = Keyword
        me.mapStyles[State.String]            = String
        me.mapStyles[State.RawString]         = String
        me.mapStyles[State.Character]         = Character
        me.mapStyles[State.Numeric]           = Numeric
        me.mapStyles[State.NumericFrac]       = Numeric
        me.mapStyles[State.NumericHex]        = Numeric
        me.mapStyles[State.NumericBin]        = Numeric
        me.mapStyles[State.Attribute]         = Preprocessor
        me.mapStyles[State.Error]             = Error

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

        me.mapWords.add("if", State.Control)
        me.mapWords.add("else", State.Control)
        me.mapWords.add("elif", State.Control)
        me.mapWords.add("for", State.Control)
        me.mapWords.add("while", State.Control)
        me.mapWords.add("switch", State.Control)
        me.mapWords.add("defer", State.Control)
        me.mapWords.add("foreach", State.Control)
        me.mapWords.add("where", State.Control)
        me.mapWords.add("break", State.Control)
        me.mapWords.add("fallthrough", State.Control)
        me.mapWords.add("unreachable", State.Control)
        me.mapWords.add("return", State.Control)
        me.mapWords.add("case", State.Control)
        me.mapWords.add("continue", State.Control)
        me.mapWords.add("default", State.Control)
        me.mapWords.add("and", State.Control)
        me.mapWords.add("or", State.Control)
        me.mapWords.add("orelse", State.Control)
        me.mapWords.add("to", State.Control)
        me.mapWords.add("until", State.Control)
        me.mapWords.add("in", State.Control)
        me.mapWords.add("as", State.Control)
        me.mapWords.add("is", State.Control)

        me.mapWords.add("using", State.Keyword)
        me.mapWords.add("with", State.Keyword)
        me.mapWords.add("cast", State.Keyword)
        me.mapWords.add("dref", State.Keyword)
        me.mapWords.add("moveref", State.Declaration)
        me.mapWords.add("retval", State.Keyword)
        me.mapWords.add("try", State.Keyword)
        me.mapWords.add("catch", State.Keyword)
        me.mapWords.add("trycatch", State.Keyword)
        me.mapWords.add("assume", State.Keyword)
        me.mapWords.add("throw", State.Keyword)
        me.mapWords.add("discard", State.Keyword)

        me.mapWords.add("enum", State.Declaration)
        me.mapWords.add("struct", State.Declaration)
        me.mapWords.add("union", State.Declaration)
        me.mapWords.add("impl", State.Declaration)
        me.mapWords.add("interface", State.Declaration)
        me.mapWords.add("func", State.Declaration)
        me.mapWords.add("mtd", State.Declaration)
        me.mapWords.add("namespace", State.Declaration)
        me.mapWords.add("alias", State.Declaration)
        me.mapWords.add("attr", State.Declaration)

        me.mapWords.add("let", State.Declaration)
        me.mapWords.add("var", State.Declaration)
        me.mapWords.add("const", State.Declaration)

        me.mapWords.add("public", State.StorageModifier)
        me.mapWords.add("internal", State.StorageModifier)
        me.mapWords.add("private", State.StorageModifier)

        me.mapWords.add("true", State.Constant)
        me.mapWords.add("false", State.Constant)
        me.mapWords.add("null", State.Constant)
        me.mapWords.add("undefined", State.Constant)

        me.mapWords.add("not", State.Error)
        me.mapWords.add("do", State.Error)

        me.mapWords.add("#defined", State.Intrinsic)
        me.mapWords.add("#offsetof", State.Intrinsic)
        me.mapWords.add("#alignof", State.Intrinsic)
        me.mapWords.add("#sizeof", State.Intrinsic)
        me.mapWords.add("#typeof", State.Intrinsic)
        me.mapWords.add("#stringof", State.Intrinsic)
        me.mapWords.add("#nameof", State.Intrinsic)
        me.mapWords.add("#isconstexpr", State.Intrinsic)
        me.mapWords.add("#location", State.Intrinsic)
        me.mapWords.add("#decltype", State.Intrinsic)
        me.mapWords.add("#hastag", State.Intrinsic)
        me.mapWords.add("#gettag", State.Intrinsic)
        me.mapWords.add("#runes", State.Intrinsic)
        me.mapWords.add("#safety", State.Intrinsic)
        me.mapWords.add("#include", State.Intrinsic)

        me.mapWords.add("#uniq0", State.Intrinsic)
        me.mapWords.add("#uniq1", State.Intrinsic)
        me.mapWords.add("#uniq2", State.Intrinsic)
        me.mapWords.add("#uniq3", State.Intrinsic)
        me.mapWords.add("#uniq4", State.Intrinsic)
        me.mapWords.add("#uniq5", State.Intrinsic)
        me.mapWords.add("#uniq6", State.Intrinsic)
        me.mapWords.add("#uniq7", State.Intrinsic)
        me.mapWords.add("#uniq8", State.Intrinsic)
        me.mapWords.add("#uniq9", State.Intrinsic)
        me.mapWords.add("#alias0", State.Intrinsic)
        me.mapWords.add("#alias1", State.Intrinsic)
        me.mapWords.add("#alias2", State.Intrinsic)
        me.mapWords.add("#alias3", State.Intrinsic)
        me.mapWords.add("#alias4", State.Intrinsic)
        me.mapWords.add("#alias5", State.Intrinsic)
        me.mapWords.add("#alias6", State.Intrinsic)
        me.mapWords.add("#alias7", State.Intrinsic)
        me.mapWords.add("#alias8", State.Intrinsic)
        me.mapWords.add("#alias9", State.Intrinsic)

        me.mapWords.add("@rtflags", State.Intrinsic)
        me.mapWords.add("@err", State.Intrinsic)
        me.mapWords.add("@args", State.Intrinsic)
        me.mapWords.add("@pinfos", State.Intrinsic)
        me.mapWords.add("@bytecode", State.Intrinsic)
        me.mapWords.add("@compiler", State.Intrinsic)
        me.mapWords.add("@modules", State.Intrinsic)
        me.mapWords.add("@gvtd", State.Intrinsic)
        me.mapWords.add("@index", State.Intrinsic)

        me.mapWords.add("@kindof", State.Intrinsic)
        me.mapWords.add("@countof", State.Intrinsic)
        me.mapWords.add("@dataof", State.Intrinsic)
        me.mapWords.add("@mkslice", State.Intrinsic)
        me.mapWords.add("@mkstring", State.Intrinsic)
        me.mapWords.add("@mkcallback", State.Intrinsic)
        me.mapWords.add("@mkany", State.Intrinsic)
        me.mapWords.add("@mkinterface", State.Intrinsic)
        me.mapWords.add("@dbgalloc", State.Intrinsic)
        me.mapWords.add("@sysalloc", State.Intrinsic)
        me.mapWords.add("@stringcmp", State.Intrinsic)
        me.mapWords.add("@typecmp", State.Intrinsic)
        me.mapWords.add("@is", State.Intrinsic)
        me.mapWords.add("@as", State.Intrinsic)
        me.mapWords.add("@getcontext", State.Intrinsic)
        me.mapWords.add("@tableof", State.Intrinsic)

        me.mapWords.add("@assert", State.Intrinsic)
        me.mapWords.add("@breakpoint", State.Intrinsic)
        me.mapWords.add("@init", State.Intrinsic)
        me.mapWords.add("@drop", State.Intrinsic)
        me.mapWords.add("@postcopy", State.Intrinsic)
        me.mapWords.add("@postmove", State.Intrinsic)
        me.mapWords.add("@compilererror", State.Intrinsic)
        me.mapWords.add("@compilerwarning", State.Intrinsic)
        me.mapWords.add("@panic", State.Intrinsic)
        me.mapWords.add("@print", State.Intrinsic)
        me.mapWords.add("@setcontext", State.Intrinsic)

        me.mapWords.add("opSlice", State.FunctionSpecOp)
        me.mapWords.add("opBinary", State.FunctionSpecOp)
        me.mapWords.add("opUnary", State.FunctionSpecOp)
        me.mapWords.add("opAssign", State.FunctionSpecOp)
        me.mapWords.add("opIndexAssign", State.FunctionSpecOp)
        me.mapWords.add("opIndexAffect", State.FunctionSpecOp)
        me.mapWords.add("opCast", State.FunctionSpecOp)
        me.mapWords.add("opCount", State.FunctionSpecOp)
        me.mapWords.add("opData", State.FunctionSpecOp)
        me.mapWords.add("opVisit", State.FunctionSpecOp)
        me.mapWords.add("opInit", State.FunctionSpecOp)
        me.mapWords.add("opReloc", State.FunctionSpecOp)
        me.mapWords.add("opEquals", State.FunctionSpecOp)
        me.mapWords.add("opCmp", State.FunctionSpecOp)
        me.mapWords.add("opPostCopy", State.FunctionSpecOp)
        me.mapWords.add("opPostMove", State.FunctionSpecOp)
        me.mapWords.add("opDrop", State.FunctionSpecOp)
        me.mapWords.add("opCount", State.FunctionSpecOp)
        me.mapWords.add("opAffect", State.FunctionSpecOp)
        me.mapWords.add("opAffectLiteral", State.FunctionSpecOp)
        me.mapWords.add("opIndex", State.FunctionSpecOp)
        me.mapWords.add("opIndexAssign", State.FunctionSpecOp)

        me.mapWords.add("me", State.Type)
        me.mapWords.add("cvarargs", State.Type)

        me.mapWords.add("any", State.Type)
        me.mapWords.add("void", State.Type)
        me.mapWords.add("rune", State.Type)
        me.mapWords.add("f32", State.Type)
        me.mapWords.add("f64", State.Type)
        me.mapWords.add("s8", State.Type)
        me.mapWords.add("s16", State.Type)
        me.mapWords.add("s32", State.Type)
        me.mapWords.add("s64", State.Type)
        me.mapWords.add("u8", State.Type)
        me.mapWords.add("u16", State.Type)
        me.mapWords.add("u32", State.Type)
        me.mapWords.add("u64", State.Type)
        me.mapWords.add("bool", State.Type)
        me.mapWords.add("string", State.Type)
        me.mapWords.add("cstring", State.Type)
        me.mapWords.add("typeinfo", State.Type)

        me.mapWords.add("@abs", State.Buildin)
        me.mapWords.add("@acos", State.Buildin)
        me.mapWords.add("@asin", State.Buildin)
        me.mapWords.add("@atan", State.Buildin)
        me.mapWords.add("@atan2", State.Buildin)
        me.mapWords.add("@alloc", State.Buildin)
        me.mapWords.add("@atomadd", State.Buildin)
        me.mapWords.add("@atomand", State.Buildin)
        me.mapWords.add("@atomcmpxchg", State.Buildin)
        me.mapWords.add("@atomor", State.Buildin)
        me.mapWords.add("@atomxchg", State.Buildin)
        me.mapWords.add("@atomxor", State.Buildin)
        me.mapWords.add("@bitcountnz", State.Buildin)
        me.mapWords.add("@bitcountlz", State.Buildin)
        me.mapWords.add("@bitcounttz", State.Buildin)
        me.mapWords.add("@byteswap", State.Buildin)
        me.mapWords.add("@cvaarg", State.Buildin)
        me.mapWords.add("@cvaend", State.Buildin)
        me.mapWords.add("@cvastart", State.Buildin)
        me.mapWords.add("@ceil", State.Buildin)
        me.mapWords.add("@cos", State.Buildin)
        me.mapWords.add("@cosh", State.Buildin)
        me.mapWords.add("@exp", State.Buildin)
        me.mapWords.add("@exp2", State.Buildin)
        me.mapWords.add("@floor", State.Buildin)
        me.mapWords.add("@free", State.Buildin)
        me.mapWords.add("@log", State.Buildin)
        me.mapWords.add("@log10", State.Buildin)
        me.mapWords.add("@log2", State.Buildin)
        me.mapWords.add("@max", State.Buildin)
        me.mapWords.add("@memcmp", State.Buildin)
        me.mapWords.add("@memcpy", State.Buildin)
        me.mapWords.add("@memmove", State.Buildin)
        me.mapWords.add("@memset", State.Buildin)
        me.mapWords.add("@min", State.Buildin)
        me.mapWords.add("@muladd", State.Buildin)
        me.mapWords.add("@pow", State.Buildin)
        me.mapWords.add("@realloc", State.Buildin)
        me.mapWords.add("@rol", State.Buildin)
        me.mapWords.add("@ror", State.Buildin)
        me.mapWords.add("@round", State.Buildin)
        me.mapWords.add("@sin", State.Buildin)
        me.mapWords.add("@sinh", State.Buildin)
        me.mapWords.add("@sqrt", State.Buildin)
        me.mapWords.add("@strcmp", State.Buildin)
        me.mapWords.add("@strlen", State.Buildin)
        me.mapWords.add("@tan", State.Buildin)
        me.mapWords.add("@tanh", State.Buildin)
        me.mapWords.add("@trunc", State.Buildin)

        me.mapWords.add("#run", State.CompilerFunction)
        me.mapWords.add("#test", State.CompilerFunction)
        me.mapWords.add("#init", State.CompilerFunction)
        me.mapWords.add("#drop", State.CompilerFunction)
        me.mapWords.add("#premain", State.CompilerFunction)
        me.mapWords.add("#main", State.CompilerFunction)
        me.mapWords.add("#message", State.CompilerFunction)
        me.mapWords.add("#ast", State.CompilerFunction)
        me.mapWords.add("#dependencies", State.CompilerFunction)

        me.mapWords.add("#global", State.CompilerGlobal)
        me.mapWords.add("#load", State.CompilerStatement)
        me.mapWords.add("#foreignlib", State.CompilerStatement)
        me.mapWords.add("#assert", State.CompilerStatement)
        me.mapWords.add("#print", State.CompilerStatement)
        me.mapWords.add("#error", State.CompilerStatement)
        me.mapWords.add("#warning", State.CompilerStatement)
        me.mapWords.add("#import", State.CompilerStatement)
        me.mapWords.add("#inject", State.CompilerStatement)
        me.mapWords.add("#macro", State.CompilerStatement)
        me.mapWords.add("#if", State.CompilerStatement)
        me.mapWords.add("#else", State.CompilerStatement)
        me.mapWords.add("#elif", State.CompilerStatement)
        me.mapWords.add("#scope", State.CompilerStatement)

        me.mapWords.add("#cfg", State.Compiler)
        me.mapWords.add("#os", State.Compiler)
        me.mapWords.add("#arch", State.Compiler)
        me.mapWords.add("#cpu", State.Compiler)
        me.mapWords.add("#backend", State.Compiler)
        me.mapWords.add("#me", State.Compiler)
        me.mapWords.add("#module", State.Compiler)
        me.mapWords.add("#file", State.Compiler)
        me.mapWords.add("#line", State.Compiler)
        me.mapWords.add("#curlocation", State.Compiler)
        me.mapWords.add("#callerlocation", State.Compiler)
        me.mapWords.add("#callerfunction", State.Compiler)
        me.mapWords.add("#swagversion", State.Compiler)
        me.mapWords.add("#swagrevision", State.Compiler)
        me.mapWords.add("#swagbuildnum", State.Compiler)
        me.mapWords.add("#swagos", State.Compiler)

        me.mapWords.add("#code", State.Intrinsic)
        me.mapWords.add("#type", State.Intrinsic)
        me.mapWords.add("#up", State.Intrinsic)

        me.mapWords.add("#prom", State.Modifier)
        me.mapWords.add("#wrap", State.Modifier)
        me.mapWords.add("#nodrop", State.Modifier)
        me.mapWords.add("#move", State.Modifier)
        me.mapWords.add("#moveraw", State.Modifier)
        me.mapWords.add("#reverse", State.Modifier)
        me.mapWords.add("#ref", State.Modifier)
        me.mapWords.add("#constref", State.Modifier)
        me.mapWords.add("#null", State.Modifier)
    }

    mtd impl insertRune(ed: *RichEditCtrl, ch: rune)->bool
    {
        // Smart return
        if ch == '\n'
        {
            var cp   = ed.getBegSelection()
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

                if cpt < line.chars.count and line.chars[cpt] == '{':
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
            if seek >= line.chars.count:
                return 0
            return line.chars[seek]
        }

        #[Swag.Mixin]
        func getNext()->rune
        {
            if !line.chars.count:
                return 0
            if seek >= line.chars.count - 1:
                return 0
            return line.chars[seek + 1]
        }

        #[Swag.Mixin]
        func seekNext()
        {
            if seek >= line.chars.count:
                return
            line.styles[seek] = me.mapStyles[currentState]
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
                    seekNext()
                    continue
                }

                if c == 'e'
                {
                    seekNext()
                    if cn == '+' or cn == '-':
                        seekNext()
                    currentState = State.Numeric
                    continue
                }

                currentState = State.Default

            case State.NumericHex:
                if Unicode.isNumber(c) or c == '_'
                {
                    seekNext()
                    continue
                }

                if !Unicode.isLatin1(c) or !Latin1.isHexDigit(cast(u8) c)
                {
                    currentState = State.Default
                    seekNext()
                    continue
                }

            case State.NumericBin:
                if Unicode.isNumber(c) or c == '_'
                {
                    seekNext()
                    continue
                }

                if c != '0' and c != '1'
                {
                    currentState = State.Default
                    seekNext()
                    if getCur() == '\'':
                        seekNext()
                    continue
                }

            case State.Numeric:
                if c == '.' and cn == '.'
                {
                    currentState = State.Default
                    seekNext()
                    continue
                }

                if c == '.' or c == 'e' or c == 'E'
                {
                    currentState = State.NumericFrac
                    seekNext()
                    continue
                }

                if !Unicode.isNumber(c) and c != '.'
                {
                    currentState = State.Default
                    seekNext()
                    if getCur() == '\'':
                        seekNext()
                    continue
                }

            case State.String:
                if c == '\\'
                {
                    seekNext()
                    seekNext()
                    continue
                }

                if c == '"'
                {
                    seekNext()
                    currentState = State.Default
                    if getCur() == '\'':
                        seekNext()
                    continue
                }

            case State.RawString:
                if c == '"' and cn == '@'
                {
                    seekNext()
                    seekNext()
                    currentState = State.Default
                    continue
                }

            case State.Character:
                if c == '\\'
                {
                    seekNext()
                    seekNext()
                    continue
                }

                if c == '\''
                {
                    seekNext()
                    currentState = State.Default
                    continue
                }

            case State.Comment:
                if c == '*' and cn == '/'
                {
                    seekNext()
                    seekNext()
                    if currentCptCmt:
                        currentCptCmt -= 1
                    if !currentCptCmt:
                        currentState = State.Default
                    continue
                }

                if c == '/' and cn == '*'
                {
                    currentCptCmt += 1
                    seekNext()
                    seekNext()
                    continue
                }

            case State.Attribute:
                if c == ']'
                {
                    seekNext()
                    currentState = State.Default
                    continue
                }

            case State.Default:
                if c == '#' and cn == '['
                {
                    currentState = State.Attribute
                    seekNext()
                    seekNext()
                    continue
                }

                if c == '@' and cn == '"'
                {
                    currentState = State.RawString
                    seekNext()
                    seekNext()
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

                    if Utf8.startsWith(word, "opVisit"):
                        currentState = State.FunctionSpecOp
                    else
                    {
                        let res = me.mapWords.tryFind(word.toString())
                        if res:
                            currentState = res.value
                        elif word == "@":
                            currentState = State.Keyword
                        elif Latin1.isUpper(word[0]):
                            currentState = State.UserType
                        elif c == '(' and (Latin1.isLetter(word[0]) or word[0] == '_'):
                            currentState = State.FunctionCall
                        else:
                            currentState = State.Default
                    }

                    while seek != seekEnd:
                        seekNext()
                    currentState = State.Default
                    if c == '\'':
                        seekNext()
                    continue
                }

                if c == '"'
                {
                    currentState = State.String
                    seekNext()
                    continue
                }

                if c == '\''
                {
                    currentState = State.Character
                    seekNext()
                    continue
                }

                if Unicode.isNumber(c) and Unicode.toLower(cn) == 'b'
                {
                    currentState = State.NumericBin
                    seekNext()
                    seekNext()
                    continue
                }

                if Unicode.isNumber(c) and Unicode.toLower(cn) == 'x'
                {
                    currentState = State.NumericHex
                    seekNext()
                    seekNext()
                    continue
                }

                if Unicode.isNumber(c)
                {
                    currentState = State.Numeric
                    seekNext()
                    continue
                }

                if c == '/' and cn == '*'
                {
                    currentCptCmt += 1
                    currentState = State.Comment
                    seekNext()
                    seekNext()
                    continue
                }

                if c == '/' and cn == c
                {
                    currentState = State.LineComment
                    seekNext()
                    seekNext()
                    continue
                }
            }

            seekNext()
        }

        if currentState == State.LineComment:
            currentState = State.Default

        if line.lexerEndingState != currentState
        {
            line.lexerEndingState = currentState
            if lineNext:
                lineNext.dirtyStyle = true
        }

        if line.lexerCptCmt != currentCptCmt
        {
            line.lexerCptCmt = currentCptCmt
            if lineNext:
                lineNext.dirtyStyle = true
        }
    }
}
