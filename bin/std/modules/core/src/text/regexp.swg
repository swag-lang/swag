#global namespace Parser

// Simple regular expression matcher
// Supported:
//
// *    zero or more
// ?    zero or one
// +    one or more
// .    any rune
//
// a|b  a or b
//
// [abc]     match any in list
// [a-zA-Z]  match any in range
//
// Unicode:
//
// /pL      letter
// /p{Ll}   lower letter
// /p{Lu}   upper letter
// /pN      number
// /pS      symbol
// /p{Sm}   symbol math
//
// Ascii only:
//
// [[:digit:]]
// [[:hexa:]]
// [[:alpha:]]
// [[:alnum:]]
// [[:lower:]]
// [[:upper:]]
// [[:blank:]]
// [[:space:]]
// [[:word:]]
// [[:ascii:]]
//
// Special escape characters:
//
// \d => [[:digit:]]
// \l => [[:lower:]]
// \u => [[:upper:]]
// \b => [[:blank:]]
// \s => [[:space:]]
// \w => [[:word:]]

const RuneAsciiDigit    = cast(rune) 0x8000_0000
const RuneAsciiLetter   = cast(rune) 0x8000_0001
const RuneAsciiLower    = cast(rune) 0x8000_0002
const RuneAsciiUpper    = cast(rune) 0x8000_0003
const RuneAsciiSpace    = cast(rune) 0x8000_0004
const RuneAscii         = cast(rune) 0x8000_0005

const RuneUnicodeLetter         = cast(rune) 0x8001_0006
const RuneUnicodeLetterLower    = cast(rune) 0x8001_0007
const RuneUnicodeLetterUpper    = cast(rune) 0x8001_0008
const RuneUnicodeNumber         = cast(rune) 0x8001_0009
const RuneUnicodeSymbol         = cast(rune) 0x8001_000a
const RuneUnicodeSymbolMath     = cast(rune) 0x8001_000b


struct NfaNode
{
    transitions:    HashTable'(rune, Array'(*NfaNode))
    terminal:       bool
}

struct NfaTable
{
    level:  u32
    nodes:  Array'(*NfaNode)
}

struct DfaNode
{
    table:          HashSet'(*NfaNode)
    transitions:    HashTable'(rune, *DfaNode)
    terminal:       bool
}

#[Swag.Opaque]
public struct RegExp
{
    nfaNodes:       Array'(*NfaNode)
    dfaNodes:       Array'(*DfaNode)
    nfaStack:       Array'(*NfaTable)
    nfaTables:      Array'(*NfaTable)
    charSet:        HashSet'rune
    openParen:      Array'u32
    orOpStack:      Array'rune
    orOpLevel:      Array'u32
    preprocessed:   Array'rune
    ignoreCase:     bool
}

impl RegExp
{
    public mtd opDrop()
    {
        clear()
    }

    // Clear content
    public mtd clear()
    {
        visit p: nfaNodes
            Memory.delete(p)
        nfaNodes.clear()

        visit p: dfaNodes
            Memory.delete(p)
        dfaNodes.clear()

        visit p: nfaTables
            Memory.delete(p)
        nfaTables.clear()

        charSet.clear()
        openParen.clear()
        orOpStack.clear()
        orOpLevel.clear()
        preprocessed.clear()
    }

    // Compile the expression
    public mtd compile(str: string, ignoreCase: bool = false) throw
    {
        clear()
        self.ignoreCase = #self.ignoreCase
        preprocess(str)
        nfaBuild()
        dfaBuild()
    }

    // Returns true if 'str' matches the regexp
    public mtd match(str: string)->bool
    {
        if dfaNodes.isEmpty()
            return false

        var stack1, stack2: Array'(*DfaNode)
        stack1.add(dfaNodes[0])

        var pstack1 = &stack1
        var pstack2 = &stack2

        var cpt = 0
        var srcPtr = @dataof(str)
        let srcLength = @countof(str)

        while cpt < srcLength and !pstack1.isEmpty()
        {
            var (c, eat) = Utf8.decodeRune(@mkslice(srcPtr, srcLength - cpt))
            if ignoreCase
                c = Unicode.toUpper(c)

            var ok = false
            while !pstack1.isEmpty()
            {
                let curState = pstack1.popBack()
                if dfaMatch(pstack2, c, curState)
                    ok = true
            }

            if ok
                cpt, srcPtr += eat

            swap(&pstack1, &pstack2)
        }

        if cpt != srcLength
            return false

        while !pstack1.isEmpty()
        {
            let curState = pstack1.popBack()
            if curState.terminal
                return true
        }

        return false
    }

    // Find all the occurences in the string
    public mtd grepAll(str: string)->Array'string
    {
        var result: retval
        var res = str
        let end = @dataof(str) + @countof(str)
        loop
        {
            res = grep(res)
            if !res
                break
            result.add(res)
            let newStart = @dataof(res) + @countof(res)
            res = @mkstring(newStart, end - newStart)
        }

        return result
    }

    // Find the first occurence in the string
    public mtd grep(str: string)->string
    {
        var beg = @dataof(str)
        var copy = @dataof(str)
        let pzEnd = beg + @countof(str)

        var end: const *u8
        while !end and (copy != pzEnd)
        {
            end = matchGrep(@mkstring(beg, pzEnd - beg))
            if !end
            {
                copy += 1
                beg = copy
            }
        }

        if !end
            return null
        return @mkstring(beg, end - beg)
    }

    mtd matchGrep(str: string)->const *u8
    {
        if dfaNodes.isEmpty()
            return null

        var stack1, stack2: Array'(*DfaNode)
        stack1.add(dfaNodes[0])
        var pstack1 = &stack1
        var pstack2 = &stack2

        var cpt = 0
        var srcPtr = @dataof(str)
        let srcLength = @countof(str)

        var endState = false
        while cpt < srcLength and !pstack1.isEmpty()
        {
            var (c, eat) = Utf8.decodeRune(@mkslice(srcPtr, srcLength - cpt))
            if ignoreCase
                c = Unicode.toUpper(c)

            var ok = false
            endState = false
            while !pstack1.isEmpty()
            {
                let curState = pstack1.popBack()
                if dfaMatch(pstack2, c, curState)
                    ok = true
                if curState.terminal
                    endState = true
            }

            if ok
                cpt, srcPtr += eat

            swap(&pstack1, &pstack2)
        }

        while !endState and !pstack1.isEmpty()
        {
            let curState = pstack1.popBack()
            if curState.terminal
                return srcPtr
        }

        if !endState or srcPtr == @dataof(str)
            return null
        return srcPtr
    }

    func dfaMatch(out: *Array'(*DfaNode), c: rune, curState: *DfaNode)->bool
    {
        var ok = false

        #[Swag.Mixin]
        func checkTr(c: rune)
        {
            {
                let find = curState.transitions.find(c)
                if find
                {
                    out.add(find.value)
                    ok = true
                }
            }
        }

        // Should be optimized, because some are exclusive

        if Unicode.isLatin1(c)
        {
            let c8 = cast(u8) c
            if Latin1.isDigit(c8)   checkTr(RuneAsciiDigit)
            if Latin1.isLetter(c8)  checkTr(RuneAsciiLetter)
            if Latin1.isLower(c8)   checkTr(RuneAsciiLower)
            if Latin1.isUpper(c8)   checkTr(RuneAsciiUpper)
            if Latin1.isSpace(c8)   checkTr(RuneAsciiSpace)
            checkTr(RuneAscii)
        }

        if Unicode.isLetter(c)      checkTr(RuneUnicodeLetter)
        if Unicode.isLower(c)       checkTr(RuneUnicodeLetterLower)
        if Unicode.isUpper(c)       checkTr(RuneUnicodeLetterUpper)
        if Unicode.isNumber(c)      checkTr(RuneUnicodeNumber)
        if Unicode.isSymbol(c)      checkTr(RuneUnicodeSymbol)
        if Unicode.isSymbolMath(c)  checkTr(RuneUnicodeSymbolMath)

        checkTr(c)
        checkTr(0'rune)

        return ok
    }

    func addTransition(node: *NfaNode, c: rune, toNode: *NfaNode)
    {
        var here = node.transitions.find(c)
        if !here
        {
            var toT: Array'(*NfaNode)
            here = node.transitions.add(c, toT)
        }

        here.value.add(toNode)
    }

    mtd nfaPop()->*NfaTable throw
    {
        if nfaStack.isEmpty()
            throw "nfaPop"
        return nfaStack.popBack()
    }

    mtd nfaPush(c: rune)
    {
        // Creates 2 new states, and one transition (by c) between them
        let table = Memory.new'NfaTable()
        let s1 = Memory.new'NfaNode()
        let s2 = Memory.new'NfaNode()

        table.level = cast(u32) openParen.count
        nfaTables.add(table)
        nfaNodes.add(s1)
        nfaNodes.add(s2)
        s1.addTransition(c, s2)

        table.nodes.add(s1)
        table.nodes.add(s2)
        nfaStack.add(table)

        // Add the character to the list of terminal characters.
        charSet.add(c)
    }

    mtd operatorUnion() throw
    {
        let table2 = try nfaPop()
        let table1 = try nfaPop()

        let ns1 = Memory.new'NfaNode()
        let ns2 = Memory.new'NfaNode()
        nfaNodes.add(ns1)
        nfaNodes.add(ns2)

        let s1 = table1.nodes.front()
        let s2 = table1.nodes.back()
        let s3 = table2.nodes.front()
        let s4 = table2.nodes.back()

        ns1.addTransition(0'rune, s1)
        s2.addTransition(0'rune, ns2)
        ns1.addTransition(0'rune, s3)
        s4.addTransition(0'rune, ns2)

        table1.nodes.add(table2.nodes.toSlice())
        table1.nodes.insertAt(0, ns1)
        table1.nodes.add(ns2)
        nfaStack.add(table1)
    }

    mtd unionValidate() throw
    {
        if orOpStack.isEmpty()
            return
        if nfaStack.count < 2
            return

        // Check if the two tables have the same nested level
        let table2 = try nfaPop()
        let table1 = try nfaPop()
        nfaStack.add(table1)
        nfaStack.add(table2)
        if table1.level != table2.level
            return

        // Operator must be at the same level
        if orOpLevel[orOpLevel.count - 1] != table1.level
            return

        // Else validate union
        orOpStack.removeBack()
        orOpLevel.removeBack()
        try operatorUnion()
    }

    mtd operatorStar() throw
    {
        let table = try nfaPop()
        let s1 = table.nodes.front()
        let s2 = table.nodes.back()
        s1.addTransition(0'rune, s2)
        s2.addTransition(0'rune, s1)
        nfaStack.add(table)
    }

    mtd operatorPlus() throw
    {
        let table = try nfaPop()
        let s1 = table.nodes.front()
        let s2 = table.nodes.back()
        s2.addTransition(0'rune, s1)
        nfaStack.add(table)
    }

    mtd operatorQuestion() throw
    {
        let table = try nfaPop()
        let s1 = table.nodes.front()
        let s2 = table.nodes.back()
        s1.addTransition(0'rune, s2)
        nfaStack.add(table)
    }

    func isReservedChar(c: rune)->bool
    {
        switch c
        {
        case `\\`, `(`, `)`, `*`, `?`,
             `+`, `.`, `|`, `[`, `]`,
             `{`, `}`:
             return true
        }

        return false
    }

    mtd nfaBuild() throw
    {
        var pz = preprocessed.buffer
        var cpt = 0

        while cpt < preprocessed.count
        {
            var c = dref pz
            switch c
            {
            case `(`:
                openParen.add(cast(u32) nfaStack.count)

            case `)`:
                try unionValidate()
                if openParen.isEmpty()
                    throw "syntax, unexpected right parenthesis"
                let op = openParen.popBack()
                try operatorConcat(nfaStack.count - op)

            case `*`:
                try operatorStar()

            case `+`:
                try operatorPlus()

            case `?`:
                try operatorQuestion()

            case `|`:
                try unionValidate()
                orOpStack.add(c)
                orOpLevel.add(cast(u32) openParen.count)

            case `.`:
                try unionValidate()
                nfaPush(0'rune)

            case `\\`:
                if isReservedChar(pz[1])
                {
                    c = pz[1]
                    cpt, pz += 1
                }

                try unionValidate()
                nfaPush(c)
                break

            default:
                try unionValidate()
                nfaPush(c)
                break
            }

            cpt, pz += 1
        }

        // Remaining operators
        try unionValidate()
        if !orOpStack.isEmpty()
            throw "syntax"

        // Concat the remaining nodes
        try operatorConcat(nfaStack.count)
    }

    mtd operatorConcat(cpt: u64) throw
    {
        var count = cpt
        while count >= 2
        {
            let table2 = try nfaPop()
            let table1 = try nfaPop()

            // Update nested level
            table1.level = cast(u32) openParen.count
            table2.level = cast(u32) openParen.count

            let s1 = table1.nodes.back()
            let s2 = table2.nodes.front()
            s1.addTransition(0'rune, s2)

            table1.nodes.add(table2.nodes.toSlice())
            nfaStack.add(table1)

            count -= 1
        }

        // Update nested level
        if count == 1
        {
            let table1 = try nfaPop()
            table1.level = cast(u32) openParen.count
            nfaStack.add(table1)
        }
    }

    func dfaEClosure(inH, outH: *HashSet'(*NfaNode))
    {
        outH.clear()

        var stack: Array'(*NfaNode)
        visit n: dref inH
            stack.add(n)

        while stack.count
        {
            let state = stack.popBack()
            outH.add(state)

            let find = state.transitions.find(0'rune)
            if !find
                continue

            visit t: find.value
            {
                if !outH.contains(t)
                    stack.add(t)
            }
        }
    }

    func setTable(node: *DfaNode, tbl: *HashSet'(*NfaNode))
    {
        node.table =,move dref tbl

        // A DFA node is terminal if one of the NFA nodes used to construct it is a
        // terminal too
        visit n: node.table
        {
            if n.terminal
            {
                node.terminal = true
                break
            }
        }
    }

    func dfaMoveNode(out: *HashSet'(*NfaNode), c: rune, inH: HashSet'(*NfaNode))
    {
        out.clear()
        visit state: inH
        {
            // The real terminal character
            let pfind = state.transitions.find(c)
            if pfind
            {
                visit t: pfind.value
                    out.add(t)
            }
        }
    }

    func equals(s1, s2: *HashSet'(*NfaNode))->bool
    {
        if s1.count != s2.count
            return false
        visit t1: dref s1
        {
            if !s2.contains(t1)
                return false
        }

        return true
    }

    mtd dfaBuild() throw
    {
        // One table must remain
        let table = try nfaPop()

        // Last state of NFA table is a terminal
        table.nodes.back().terminal = true

        // Build the first state of the DFA table
        var nfaStart, dfaStart: HashSet'(*NfaNode)
        nfaStart.add(table.nodes.front())
        dfaEClosure(&nfaStart, &dfaStart)

        var state = Memory.new'DfaNode()
        state.setTable(&dfaStart)
        dfaNodes.add(state)

        var moveRes, epsilonRes: HashSet'(*NfaNode)
        var stack: Array'(*DfaNode)
        stack.add(state)
        while stack.count
        {
            state = stack.popBack()
            visit c: charSet
            {
                dfaMoveNode(&moveRes, c, state.table)
                if moveRes.count == 0
                    continue
                dfaEClosure(&moveRes, &epsilonRes)

                var found: *DfaNode
                visit tstate: dfaNodes
                {
                    if tstate.table.count == epsilonRes.count
                    {
                        if equals(&tstate.table, &epsilonRes)
                        {
                            found = tstate
                            break
                        }
                    }
                }

                if !found
                {
                    found = Memory.new'DfaNode()
                    found.setTable(&epsilonRes)
                    stack.add(found)
                    dfaNodes.add(found)
                }

                state.transitions.add(c, found)
            }
        }
    }

    #[Swag.Discardable]
    mtd preprocessInter(sl: const [..] rune)->const ^rune throw
    {
        var pz = @dataof(sl)
        let pzStart = pz
        let pzEnd = pz + @countof(sl)

        preprocessed.add(`(`)
        pz += 1

        var firstExp = true
        while pz != pzEnd and pz[0] != `]`
        {
            if !firstExp
                preprocessed.add(`|`)
            firstExp = false

            let subSlice = @mkslice(pz, pzEnd - pz)

            // Predefined sets
            if pz[0] == `[`
            {
                if Unicode.compare(subSlice, @runes("[:digit:]"), false, 9) == 0
                {
                    preprocessed.add(RuneAsciiDigit)
                    pz += 9
                    continue
                }

                if Unicode.compare(subSlice, @runes("[:hexa:]"), false, 8) == 0
                {
                    preprocessed.add(`(`)
                    try preprocessInter(@runes("[a-fA-F]"))
                    preprocessed.add(`|`)
                    preprocessed.add(RuneAsciiDigit)
                    preprocessed.add(`)`)
                    pz += 8
                    continue
                }

                if Unicode.compare(subSlice, @runes("[:alpha:]"), false, 9) == 0
                {
                    preprocessed.add(RuneAsciiLetter)
                    pz += 9
                    continue
                }

                if Unicode.compare(subSlice, @runes("[:alnum:]"), false, 9) == 0
                {
                    preprocessed.add(`(`)
                    preprocessed.add(RuneAsciiLetter)
                    preprocessed.add(`|`)
                    preprocessed.add(RuneAsciiDigit)
                    preprocessed.add(`)`)
                    pz += 9
                    continue
                }

                if Unicode.compare(subSlice, @runes("[:lower:]"), false, 9) == 0
                {
                    preprocessed.add(RuneAsciiLower)
                    pz += 9
                    continue
                }

                if Unicode.compare(subSlice, @runes("[:upper:]"), false, 9) == 0
                {
                    preprocessed.add(RuneAsciiUpper)
                    pz += 9
                    continue
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:blank:]"), false, 9) == 0
                {
                    preprocessed.add(@runes(" |\t"))
                    pz += 9
                    continue
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:space:]"), false, 9) == 0
                {
                    preprocessed.add(RuneAsciiSpace)
                    pz += 9
                    continue
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:ascii:]"), false, 9) == 0
                {
                    preprocessed.add(RuneAscii)
                    pz += 9
                    continue
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:word:]"), false, 8) == 0
                {
                    preprocessed.add(`(`)
                    preprocessed.add(RuneAsciiLetter)
                    preprocessed.add(`|`)
                    preprocessed.add(RuneAsciiDigit)
                    preprocessed.add(`|`)
                    preprocessed.add(`_`)
                    preprocessed.add(`)`)
                    pz += 8
                    continue
                }
            }

            // Specified interval
            if @countof(subSlice) >= 3 and pz[1] == `-` and pz[2] != `]`
            {
                var first = pz[0]
                var end = pz[2]
                if first > end
                    swap(&first, &end)
                while first < end
                {
                    preprocessed.add(first)
                    preprocessed.add(`|`)
                    first += 1
                }

                preprocessed.add(first)
                pz += 3
                continue
            }

            // This is not an interval
            if isReservedChar(dref pz)
            {
                preprocessed.add(`\\`)
                preprocessed.add(dref pz)
                pz += 1
            }
            else
            {
                pz = try preprocessEscape(@mkslice(pz, pz - pzStart))
            }
        }

        if pz != pzEnd
            pz += 1
        preprocessed.add(`)`)

        return pz
    }

    func toI32(pz, pzEnd: const ^rune, val: *s32)->const ^rune throw
    {
        if !Unicode.isLatin1(pz[0]) or !Latin1.isDigit(cast(u8) pz[0])
            throw "syntax"

        var pzStart = pz
        var n = 0
        while pzStart != pzEnd and Unicode.isLatin1(pzStart[0]) and Latin1.isDigit(cast(u8) pzStart[0])
        {
            n *= 10
            n += cast(u8) pzStart[0] - `0`
            pzStart += 1
        }

        dref val = n
        return pzStart
    }

    mtd preprocessEnum(sl: const [..] rune)->const ^rune throw
    {
        var pz = @dataof(sl)
        let pzEnd = pz + @countof(sl)

        if preprocessed.count == 0
            throw "syntax, { without a previous expression"

        var m = -1
        pz += 1

        // n value
        var n: s32 = 0
        pz = try toI32(pz, pzEnd, &n)

        // m value
        if pz[0] == `,`
        {
            m = -1
            pz += 1
            if pz[0] != `}`
                pz = try toI32(pz, pzEnd, &m)
        }
        else
        {
            m = n
        }

        if pz[0] != `}`
            throw "syntax, missing '}'"
        if m != -1 and m < n
            throw "syntax, m < n in {n,m}"
        if m == 0 and n == 0
            throw "syntax, m and n are zero in {n,m}"

        pz += 1

        if n == 0 and m == -1 // {0,}
            preprocessed.add(`*`)
        elif n == 0 and m == 1 // {0,1}
            preprocessed.add(`?`)
        elif n == 1 and m == -1 // {1,}
            preprocessed.add(`+`)
        elif n != 1 or m != 1 // {1,1}
        {
            // Isolate expression to duplicate
            let pze = preprocessed.buffer + preprocessed.count - 1
            var pz1 = pze
            var cptParen = 0
            if pz[1] == `)`
                cptParen += 1
            while cptParen and pz1 != preprocessed.buffer
            {
                pz1 -= 1
                if pz1[0] == `)`
                    cptParen += 1
                if pz1[0] == `(`
                    cptParen -= 1
            }

            if cptParen
                throw "syntax, missing '('"

            var expr: Array'rune
            while pz1 <= pze
            {
                expr.add(pz1[0])
                pz1 += 1
            }

            if n == 0
                preprocessed.add(expr.toSlice())
            else
            {
                var i = n - 1
                while i
                {
                    preprocessed.add(expr.toSlice())
                    i -= 1
                }
            }

            if m == -1
            {
                preprocessed.add(`+`)
            }
            else
            {
                if n == 0
                    m -= 1
                m -= n
                while m
                {
                    preprocessed.add(expr.toSlice())
                    preprocessed.add(`?`)
                    m -= 1
                }
            }
        }

        return pz
    }

    mtd preprocessUnicode(sl: const [..] rune)->const ^rune throw
    {
        let pz = @dataof(sl) + 2
        if pz[0] == `{`
        {
            if pz[1] == `L` and pz[3] == `}`
            {
                switch pz[2]
                {
                case `l`:
                    preprocessed.add(RuneUnicodeLetterLower)
                    return pz + 4
                case `u`:
                    preprocessed.add(RuneUnicodeLetterUpper)
                    return pz + 4
                }
            }
            elif pz[1] == `S` and pz[3] == `}`
            {
                switch pz[2]
                {
                case `m`:
                    preprocessed.add(RuneUnicodeSymbolMath)
                    return pz + 4
                }
            }
        }

        switch pz[0]
        {
        case `L`:
            preprocessed.add(RuneUnicodeLetter)
            return pz + 1
        case `N`:
            preprocessed.add(RuneUnicodeNumber)
            return pz + 1
        case `S`:
            preprocessed.add(RuneUnicodeSymbol)
            return pz + 1
        }

        return pz
    }

    mtd preprocessEscape(sl: const [..] rune)->const ^rune throw
    {
        var pz = @dataof(sl)
        let c = dref pz
        if c == `\\`
        {
            switch pz[1]
            {
            case `d`:
                try preprocessInter(@runes("[[:digit:]]"))
                return pz + 2
            case `w`:
                try preprocessInter(@runes("[[:word:]]"))
                return pz + 2
            case `s`:
                try preprocessInter(@runes("[[:space:]]"))
                return pz + 2
            case `l`:
                try preprocessInter(@runes("[[:lower:]]"))
                return pz + 2
            case `u`:
                try preprocessInter(@runes("[[:upper:]]"))
                return pz + 2
            case `a`:
                preprocessed.add(`\a`)
                return pz + 2
            case `f`:
                preprocessed.add(`\f`)
                return pz + 2
            case `n`:
                preprocessed.add(`\n`)
                return pz + 2
            case `r`:
                preprocessed.add(`\r`)
                return pz + 2
            case `t`:
                preprocessed.add(`\t`)
                return pz + 2
            case `v`:
                preprocessed.add(`\v`)
                return pz + 2
            case `p`:
                pz = try preprocessUnicode(sl)
                return pz
            }
        }

        preprocessed.add(c)
        return pz + 1
    }

    mtd preprocess(inStr: string) throw
    {
        var in32 = Unicode.fromUtf8(inStr)
        in32.add(0'rune)

        var pz = cast(const ^rune) in32.buffer
        let pzEnd = pz + in32.count

        var c = pz[0]
        if ignoreCase
            c = Unicode.toUpper(c)

        while c
        {
            switch c
            {
            case `[`:
                pz = try preprocessInter(@mkslice(pz, pzEnd - pz))
                break

            case `{`:
                pz = try preprocessEnum(@mkslice(pz, pzEnd - pz))
                break

            default:
                pz = try preprocessEscape(@mkslice(pz, pzEnd - pz))
                break
            }

            c = pz[0]
            if ignoreCase
                c = Unicode.toUpper(c)
        }
    }
}