#global namespace Parser
using Errors

const RuneAsciiDigit    = cast(rune) 0x8000_0000
const RuneAsciiLetter   = cast(rune) 0x8000_0001
const RuneAsciiLower    = cast(rune) 0x8000_0002
const RuneAsciiUpper    = cast(rune) 0x8000_0003
const RuneAsciiSpace    = cast(rune) 0x8000_0004
const RuneAscii         = cast(rune) 0x8000_0005
const RuneOpenParen     = cast(rune) 0x8000_0006
const RuneCloseParen    = cast(rune) 0x8000_0007

const RuneUnicodeLetter         = cast(rune) 0x8001_0006
const RuneUnicodeLetterLower    = cast(rune) 0x8001_0007
const RuneUnicodeLetterUpper    = cast(rune) 0x8001_0008
const RuneUnicodeNumber         = cast(rune) 0x8001_0009
const RuneUnicodeSymbol         = cast(rune) 0x8001_000a
const RuneUnicodeSymbolMath     = cast(rune) 0x8001_000b

struct NfaNode
{
    transitions:    HashTable'(rune, Array'(*NfaNode))
    group:          u32
    terminal:       bool
}

struct NfaTable
{
    level:  u32
    nodes:  Array'(*NfaNode)
}

struct DfaNode
{
    table:          Array'(*NfaNode)
    transitions:    HashTable'(rune, *DfaNode)
    group:          u32
    terminal:       bool
}

struct RegExpGroup
{
    rank:   u32
    value:  string
}

// Simple regular expression matcher.
// | *                | zero or more
// | ?                | zero or one
// | +                | one or more
// | .                | any rune
// | a\|b             | a or b
// | [abc]            | match any in list
// | [a-zA-Z]         | match any in range
// | [[:digit:]]      |
// | [[:xdigit:]]     |
// | [[:alpha:]]      |
// | [[:alnum:]]      |
// | [[:lower:]]      |
// | [[:upper:]]      |
// | [[:blank:]]      |
// | [[:space:]]      |
// | [[:word:]]       |
// | [[:ascii:]]      |
// | \d               | [[:digit:]]
// | \l               | [[:lower:]]
// | \u               | [[:upper:]]
// | \s               | [[:space:]]
// | \w               | [[:word:]]
// | \pL              | unicode letter
// | \pN              | unicode number
// | \pS              | unicode symbol
// | \p{Ll}           | unicode lower letter
// | \p{Lu}           | unicode upper letter
// | \p{Sm}           | unicode symbol math
#[Swag.Opaque]
public struct RegExp
{
    nfaNodes:       Array'(*NfaNode)
    dfaNodes:       Array'(*DfaNode)
    nfaStack:       Array'(*NfaTable)
    nfaTables:      Array'(*NfaTable)
    charSet:        HashSet'rune
    groups:         Array'RegExpGroup
    groupTransform: Array'u32
    openParen:      Array'u32
    orOpStack:      Array'rune
    orOpLevel:      Array'u32
    preprocessed:   Array'rune
    group:          u32
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

        groups.clear()
        charSet.clear()
        openParen.clear()
        orOpStack.clear()
        orOpLevel.clear()
        preprocessed.clear()
    }

    // Compile the expression
    public mtd compile(expr: string, ignoreCase = false) throw
    {
        clear()
        self.ignoreCase = #self.ignoreCase
        preprocess(expr)
        nfaBuild()
        dfaBuild()
    }

    // Compile and match the expression.
    public mtd match(expr, str: string, ignoreCase = false)->bool throw
    {
        compile(expr, #self.ignoreCase)
        return match(str)
    }

    // Returns true if 'str' matches the regexp.
    // Should have been compiled before.
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

        var lastGroup  = 0
        var startGroup = srcPtr

        while cpt < srcLength and !pstack1.isEmpty()
        {
            let (cr, eat) = Utf8.decodeRune(@mkslice(srcPtr, srcLength - cpt))
            let c = ignoreCase ? Unicode.toUpper(cr) : cr

            var ok = false
            while !pstack1.isEmpty()
            {
                let curState = pstack1.popBack()
                if dfaMatch(pstack2, c, curState)
                    ok = true
            }

            if ok
            {
                // Register group if changed
                if pstack2.count and pstack2.back().group != lastGroup
                {
                    if lastGroup
                    {
                        let strGroup = @mkstring(startGroup, srcPtr-startGroup)
                        groups.add({lastGroup, strGroup})
                    }

                    lastGroup = pstack2.back().group
                    startGroup = srcPtr
                }

                cpt, srcPtr += eat
            }

            swap(&pstack1, &pstack2)
        }

        if cpt != srcLength
            return false

        while !pstack1.isEmpty()
        {
            let curState = pstack1.popBack()

            let strGroup = @mkstring(startGroup, srcPtr-startGroup)
            groups.add({lastGroup, strGroup})

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

    // Returns the parsed string associated with a given captured group
    public mtd getCapture(rank: u32)->string
    {
        visit g: groups
        {
            if g.rank == rank
                return g.value
        }

        return ""
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
            let (cr, eat) = Utf8.decodeRune(@mkslice(srcPtr, srcLength - cpt))
            let c = ignoreCase ? Unicode.toUpper(cr) : cr

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
            throw SyntaxError{"nfaPop"}
        return nfaStack.popBack()
    }

    // Creates 2 new states, and one transition (by c) between them
    mtd nfaPush(c: rune)
    {
        let table = Memory.new'NfaTable()
        nfaTables.add(table)

        let s1 = Memory.new'NfaNode()
        let s2 = Memory.new'NfaNode()
        nfaNodes.add(s1)
        nfaNodes.add(s2)

        table.level = cast(u32) openParen.count
        s1.addTransition(c, s2)

        table.nodes.add(s1)
        table.nodes.add(s2)
        nfaStack.add(table)

        // Add the character to the list of used characters
        charSet.add(c)
    }

    mtd unionValidate() throw
    {
        if orOpStack.isEmpty()
            return
        if nfaStack.count < 2
            return

        // Check if the two tables have the same nested level
        let table2 = nfaPop()
        let table1 = nfaPop()
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
        operatorUnion()
    }

    mtd operatorUnion() throw
    {
        let table2 = nfaPop()
        let table1 = nfaPop()

        let ns1 = Memory.new'NfaNode()
        let ns2 = Memory.new'NfaNode()
        nfaNodes.add(ns1)
        nfaNodes.add(ns2)

        let sa1 = table1.nodes.front()
        let sa2 = table1.nodes.back()
        let sb1 = table2.nodes.front()
        let sb2 = table2.nodes.back()

        ns1.addTransition(0'rune, sa1)
        ns1.addTransition(0'rune, sb1)
        sa2.addTransition(0'rune, ns2)
        sb2.addTransition(0'rune, ns2)

        table1.nodes.add(table2.nodes.toSlice())
        table1.nodes.insertAt(0, ns1)
        table1.nodes.add(ns2)
        nfaStack.add(table1)
    }

    mtd operatorConcat(cpt: u64, canGroup = true) throw
    {
        var count = cpt
        while count >= 2
        {
            let table2 = nfaPop()
            let table1 = nfaPop()

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
            let table1 = nfaPop()
            table1.level = cast(u32) openParen.count

            if canGroup
            {
                group += 1
                visit n: table1.nodes
                {
                    if n.group == 0
                        n.group = group
                }
            }

            nfaStack.add(table1)
        }
    }          

    mtd operatorStar() throw
    {
        let table = nfaPop()
        let s1 = table.nodes.front()
        let s2 = table.nodes.back()
        s1.addTransition(0'rune, s2)
        s2.addTransition(0'rune, s1)
        nfaStack.add(table)
    }

    mtd operatorPlus() throw
    {
        let table = nfaPop()
        let s1 = table.nodes.front()
        let s2 = table.nodes.back()
        s2.addTransition(0'rune, s1)
        nfaStack.add(table)
    }

    mtd operatorQuestion() throw
    {
        let table = nfaPop()
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
            case `(`, RuneOpenParen:
                openParen.add(cast(u32) nfaStack.count)

            case RuneCloseParen:
                unionValidate()
                let op = openParen.popBack()
                operatorConcat(nfaStack.count - op, false)

            case `)`:
                unionValidate()
                if openParen.isEmpty()
                    throw SyntaxError{"unexpected right parenthesis"}
                let op = openParen.popBack()
                operatorConcat(nfaStack.count - op)

            case `*`:
                operatorStar()

            case `+`:
                operatorPlus()

            case `?`:
                operatorQuestion()

            case `|`:
                unionValidate()
                orOpStack.add(c)
                orOpLevel.add(cast(u32) openParen.count)

            case `.`:
                unionValidate()
                nfaPush(0'rune)

            case `\\`:
                if isReservedChar(pz[1])
                {
                    c = pz[1]
                    cpt, pz += 1
                }

                unionValidate()
                nfaPush(c)
                break

            default:
                unionValidate()
                nfaPush(c)
                break
            }

            cpt, pz += 1
        }

        // Remaining operators
        unionValidate()
        if !orOpStack.isEmpty()
            throw SyntaxError{}

        // Concat the remaining nodes
        operatorConcat(nfaStack.count)
    }

    func dfaEClosure(inH, outH: *Array'(*NfaNode))
    {
        outH.clear()

        var stack: Array'(*NfaNode)
        visit n: dref inH
            stack.add(n)

        while stack.count
        {
            let state = stack.popBack()
            outH.addOnce(state)

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

    mtd setTable(node: *DfaNode, tbl: *Array'(*NfaNode))
    {
        node.table =,move dref tbl

        // A DFA node is terminal if one of the NFA nodes used to construct it is a
        // terminal too
        visit n: node.table
        {
            if node.group == 0
                node.group = groupTransform[n.group]

            if n.terminal
                node.terminal = true
        }
    }

    func dfaMoveNode(out: *Array'(*NfaNode), c: rune, inH: Array'(*NfaNode))
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

    func equals(s1, s2: *Array'(*NfaNode))->bool
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
        let table = nfaPop()

        // Last state of NFA table is a terminal
        table.nodes.back().terminal = true

        // Group transformation
        var grp: Array'u32
        visit n: table.nodes
            grp.add(n.group)
        grp.sort()

        var curGroup, curGroupSwap: u32

        groupTransform.resize(cast(u64) group+1)
        visit n: grp
        {
            if n != curGroup
                curGroupSwap += 1
            curGroup = n
            groupTransform[n] = curGroupSwap
        }

        // Build the first state of the DFA table
        var nfaStart, dfaStart: Array'(*NfaNode)
        nfaStart.addOnce(table.nodes.front())
        dfaEClosure(&nfaStart, &dfaStart)

        var state = Memory.new'DfaNode()
        setTable(state, &dfaStart)
        dfaNodes.add(state)

        var moveRes, epsilonRes: Array'(*NfaNode)
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
                    if equals(&tstate.table, &epsilonRes)
                    {
                        found = tstate
                        break
                    }
                }

                if !found
                {
                    found = Memory.new'DfaNode()
                    setTable(found, &epsilonRes)
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

        preprocessed.add(RuneOpenParen)
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

                if Unicode.compare(subSlice, @runes("[:xdigit:]"), false, 10) == 0
                {
                    preprocessed.add(RuneOpenParen)
                    preprocessInter(@runes("[a-fA-F]"))
                    preprocessed.add(`|`)
                    preprocessed.add(RuneAsciiDigit)
                    preprocessed.add(RuneCloseParen)
                    pz += 10
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
                    preprocessed.add(RuneOpenParen)
                    preprocessed.add(RuneAsciiLetter)
                    preprocessed.add(`|`)
                    preprocessed.add(RuneAsciiDigit)
                    preprocessed.add(RuneCloseParen)
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
                    preprocessed.add(RuneOpenParen)
                    preprocessed.add(RuneAsciiLetter)
                    preprocessed.add(`|`)
                    preprocessed.add(RuneAsciiDigit)
                    preprocessed.add(`|`)
                    preprocessed.add(`_`)
                    preprocessed.add(RuneCloseParen)
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
                pz = preprocessEscape(@mkslice(pz, pz - pzStart))
            }
        }

        if pz != pzEnd
            pz += 1
        preprocessed.add(RuneCloseParen)

        return pz
    }

    func toI32(pz, pzEnd: const ^rune, val: *s32)->const ^rune throw
    {
        if !Unicode.isLatin1(pz[0]) or !Latin1.isDigit(cast(u8) pz[0])
            throw SyntaxError{}

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
            throw SyntaxError{"'{' without a previous expression"}

        var m = -1
        pz += 1

        // n value
        var n: s32 = 0
        pz = toI32(pz, pzEnd, &n)

        // m value
        if pz[0] == `,`
        {
            m = -1
            pz += 1
            if pz[0] != `}`
                pz = toI32(pz, pzEnd, &m)
        }
        else
        {
            m = n
        }

        if pz[0] != `}`
            throw SyntaxError{"enum, missing '}'"}
        if m != -1 and m < n
            throw SyntaxError{"enum m < n in {n,m}"}
        if m == 0 and n == 0
            throw SyntaxError{"enum m and n are zero in {n,m}"}

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
            if pz1[0] == `)` or pz1[0] == RuneCloseParen
                cptParen += 1
            while cptParen and pz1 != preprocessed.buffer
            {
                pz1 -= 1
                if pz1[0] == `)` or pz1[0] == RuneCloseParen
                    cptParen += 1
                if pz1[0] == `(` or pz1[0] == RuneOpenParen
                    cptParen -= 1
            }

            if cptParen
                throw SyntaxError{"missing '('"}

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
                preprocessInter(@runes("[[:digit:]]"))
                return pz + 2
            case `w`:
                preprocessInter(@runes("[[:word:]]"))
                return pz + 2
            case `s`:
                preprocessInter(@runes("[[:space:]]"))
                return pz + 2
            case `l`:
                preprocessInter(@runes("[[:lower:]]"))
                return pz + 2
            case `u`:
                preprocessInter(@runes("[[:upper:]]"))
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
                pz = preprocessUnicode(sl)
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

        var c = ignoreCase ? Unicode.toUpper(pz[0]) : pz[0]
        var open = false
        while c
        {
            switch c
            {
            case `[`:
                if open { preprocessed.add(RuneCloseParen); open = false; } 
                pz = preprocessInter(@mkslice(pz, pzEnd - pz))
                break

            case `{`:
                if open { preprocessed.add(RuneCloseParen); open = false; } 
                pz = preprocessEnum(@mkslice(pz, pzEnd - pz))

            default:
                if open and (pz[1] == `*` or pz[1] == `+` or pz[1] == `?`)
                {
                    if open { preprocessed.add(RuneCloseParen); open = false; } 
                    pz = preprocessEscape(@mkslice(pz, pzEnd - pz))
                }
                elif isReservedChar(c)
                {
                    if open { preprocessed.add(RuneCloseParen); open = false; } 
                    pz = preprocessEscape(@mkslice(pz, pzEnd - pz))
                }
                else
                {
                    if !open { preprocessed.add(RuneOpenParen); open = true; }
                    pz = preprocessEscape(@mkslice(pz, pzEnd - pz))
                }
            }

            c = ignoreCase ? Unicode.toUpper(pz[0]) : pz[0]
        }

        if open
            preprocessed.add(RuneCloseParen)
    }
}