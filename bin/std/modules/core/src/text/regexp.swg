#global namespace Parser
using Errors

const RuneAsciiDigit  = cast(rune) 0x8000_0000
const RuneAsciiLetter = cast(rune) 0x8000_0001
const RuneAsciiLower  = cast(rune) 0x8000_0002
const RuneAsciiUpper  = cast(rune) 0x8000_0003
const RuneAsciiSpace  = cast(rune) 0x8000_0004
const RuneAscii       = cast(rune) 0x8000_0005
const RuneOpenParen   = cast(rune) 0x8000_0006
const RuneCloseParen  = cast(rune) 0x8000_0007
const RuneAsciiAlNum  = cast(rune) 0x8000_0008
const RuneAsciiWord   = cast(rune) 0x8000_0009
const RuneAsciiBlank  = cast(rune) 0x8000_000A
const RuneAsciiHexa   = cast(rune) 0x8000_000B
const RuneStart       = cast(rune) 0x8000_000C
const RuneEnd         = cast(rune) 0x8000_000D
const RuneWordSwap    = cast(rune) 0x8000_000E
const RuneGroup1      = cast(rune) 0x8000_0010
const RuneGroup2      = cast(rune) 0x8000_0020
const RuneGroup3      = cast(rune) 0x8000_0030
const RuneGroup4      = cast(rune) 0x8000_0040
const RuneGroup5      = cast(rune) 0x8000_0050
const RuneGroup6      = cast(rune) 0x8000_0060
const RuneGroup7      = cast(rune) 0x8000_0070
const RuneGroup8      = cast(rune) 0x8000_0080
const RuneGroup9      = cast(rune) 0x8000_0090
const RuneWhatever    = cast(rune) 0x8000_00A0

const RuneUnicodeLetter      = cast(rune) 0x8001_0006
const RuneUnicodeLetterLower = cast(rune) 0x8001_0007
const RuneUnicodeLetterUpper = cast(rune) 0x8001_0008
const RuneUnicodeNumber      = cast(rune) 0x8001_0009
const RuneUnicodeSymbol      = cast(rune) 0x8001_000a
const RuneUnicodeSymbolMath  = cast(rune) 0x8001_000b

const RuneInvert = cast(rune) 0x0400_0000

struct NfaTable
{
    level:     u32
    nodes:     Array'(*NfaNode)
}

struct NfaNode
{
    transitions:     HashTable'(rune, Array'(*NfaNode))
    group:           u32
    terminal:        bool
}

struct DfaNode
{
    table:           Array'(*NfaNode)
    transitions:     HashTable'(rune, *DfaNode)
    group:           u32
    terminal:        bool
}

struct RegExpGroup
{
    rank:      u32
    value:     string
}

struct DfaState
{
    node:           *DfaNode
    groupStart:     const ^u8
    parseIdx:       s32
    lastGroup:      s32
    groups:         Array'RegExpGroup
}

// Simple regular expression matcher.
// | *                | zero or more
// | ?                | zero or one
// | +                | one or more
// | .                | any rune
// | ^<expr>          | start of buffer
// | <expr>$          | end of buffer
// | a\|b             | a or b
// | [abc]            | match any in list
// | [a-zA-Z]         | match any in range
// | [[:digit:]]      | [0-9]
// | [[:xdigit:]]     | [a-fA-F0-9]
// | [[:alpha:]]      | [a-zA-Z]
// | [[:alnum:]]      | [a-zA-Z0-9]
// | [[:lower:]]      | [a-z]
// | [[:upper:]]      | [A-Z]
// | [[:blank:]]      | whitespace or tab
// | [[:space:]]      | and whitespace character
// | [[:word:]]       | [a-zA-Z0-9_]
// | [[:ascii:]]      | Any ascii character
// | [^<interval>]    | Inverse of <interval>
// | \d               | [[:digit:]]
// | \D               | [^[:digit:]]
// | \l               | [[:lower:]]
// | \L               | [^[:lower:]]
// | \u               | [[:upper:]]
// | \U               | [^[:upper:]]
// | \s               | [[:space:]]
// | \S               | [^[:space:]]
// | \w               | [[:word:]]
// | \W               | [^[:word:]]
// | \b               | start or end of word
// | \pL              | unicode letter
// | \pN              | unicode number
// | \pS              | unicode symbol
// | \p{Ll}           | unicode lower letter
// | \p{Lu}           | unicode upper letter
// | \p{Sm}           | unicode symbol math
// | \1 to \9         | backreference to a capture block
#[Swag.Opaque]
public struct RegExp
{
    nfaNodes:           ArrayPtr'(NfaNode)
    dfaNodes:           ArrayPtr'(DfaNode)
    nfaTables:          ArrayPtr'(NfaTable)
    nfaStack:           Array'(*NfaTable)
    charSet:            HashSet'rune
    groups:             Array'RegExpGroup
    groupTransform:     Array'u32
    openParen:          Array'u32
    orOpStack:          Array'rune
    orOpLevel:          Array'u32
    preprocessed:       Array'rune
    group:              u32
    hasStarted:         bool
    wasWord:            bool
    ignoreCase:         bool
    bufStart:           const ^u8
    bufLength:          u64
}

impl RegExp
{
    public mtd opDrop()
    {
        me.clear()
    }

    // You can get a capture group value by using the derefence syntax.
    // Returns 'null' if the capture group does not exist
    public mtd opIndex(idx: u64)->string
    {
        foreach v in me.groups
        {
            if v.rank == idx:
                return v.value
        }

        unreachable
    }

    mtd clearMatch()
    {
        me.hasStarted, me.wasWord = false
        me.group = 0
        me.groups.clear()
    }

    // Clear content
    public mtd clear()
    {
        me.nfaNodes.clear()
        me.dfaNodes.clear()
        me.nfaTables.clear()
        me.charSet.clear()
        me.openParen.clear()
        me.orOpStack.clear()
        me.orOpLevel.clear()
        me.preprocessed.clear()

        me.clearMatch()
    }

    // Creates and return an expression
    public func create(expr: string, ignoreCase = false)->RegExp throw
    {
        var result: retval
        result.compile(expr, ignoreCase)
        return result
    }

    // Compile the expression
    public mtd compile(expr: string, ignoreCase = false) throw
    {
        me.clear()
        me.ignoreCase = #me.ignoreCase
        me.preprocess(expr)
        me.nfaBuild()
        me.dfaBuild()
    }

    // Returns the parsed string associated with a given captured group
    public mtd getCapture(rank: u32)->string
    {
        foreach g in me.groups
        {
            if g.rank == rank:
                return g.value
        }

        return ""
    }

    // Returns all the captured strings, in order
    public mtd getCaptures()->Array'string
    {
        var result: retval
        foreach v in me.groups:
            result.add(v.value)
        return result
    }

    // Compile and match the expression.
    #[Swag.Overload]
    public mtd match(expr, str: string, ignoreCase = false)->bool throw
    {
        me.compile(expr, #me.ignoreCase)
        return me.match(str)
    }

    // Returns true if 'str' matches the regexp.
    // Should have been compiled before.
    #[Swag.Overload]
    public mtd match(str: string)->bool
    {
        me.clearMatch()
        let (ok, ?) = me.matchInternal(str, false)
        return ok
    }

    // Find all the occurences in the string
    #[Swag.Overload]
    public func grepAll(expr, str: string)->Array'string throw
    {
        var r = create(expr)
        return r.grepAll(str)
    }

    // Find all the occurences in the string
    #[Swag.Overload]
    public mtd grepAll(str: string)->Array'string
    {
        var result: retval

        if str == null:
            return result

        var res = str
        let end = @dataof(str) + @countof(str)
        me.clearMatch()

        for
        {
            let val = me.internalGrep(res)
            if !val:
                break
            result.add(val)
            let newStart = @dataof(val) + @countof(val)
            res = @mkstring(newStart, end - newStart)
        }

        return result
    }

    // Find the first occurence in the string
    #[Swag.Overload]
    public func grep(expr, str: string)->string throw
    {
        var r = create(expr)
        return r.grep(str)
    }

    // Find the first occurence in the string
    #[Swag.Overload]
    public mtd grep(str: string)->#null string
    {
        if str == null:
            return null
        me.clearMatch()
        return me.internalGrep(str)
    }

    // Try to match the start of the string, and returns the matched value or null.
    public mtd partialMatch(str: string)->#null string
    {
        if str == null:
            return null
        let beg  = @dataof(str)
        let last = beg + @countof(str)

        let res = me.matchInternal(@mkstring(beg, last - beg), true)
        if res.ok:
            return @mkstring(beg, res.buf - beg)
        return null
    }

    mtd internalGrep(str: string)->#null string
    {
        var beg  = @dataof(str)
        let last = beg + @countof(str)

        while beg != last
        {
            let res = me.matchInternal(@mkstring(beg, last - beg), true)
            if res.ok:
                return @mkstring(beg, res.buf - beg)
            beg += 1
        }

        return null
    }

    func setCapture(state: *DfaState, rank: s32, str: string)
    {
        foreach &v in state.groups
        {
            if v.rank == rank
            {
                v.value = str
                return
            }
        }

        state.groups.add({rank, str})
    }

    mtd matchInternal(str: string, forGrep: bool)->{ ok: bool, buf: const *u8 }
    {
        var result: retval

        if me.dfaNodes.isEmpty():
            return result

        me.bufStart  = @dataof(str)
        me.bufLength = @countof(str)

        var stack1, stack2: Array'DfaState
        var grepTerm: DfaState

        var st: DfaState
        st.node       = me.dfaNodes[0]
        st.groupStart = me.bufStart
        stack1.add(st)

        var pstack1 = &stack1
        var pstack2 = &stack2

        var mustEnd = false
        while !pstack1.isEmpty() and !mustEnd
        {
            while !pstack1.isEmpty()
            {
                var curState = pstack1.popBack()
                if curState.parseIdx >= me.bufLength
                {
                    pstack2.add(curState)
                    continue
                }

                let ptr = me.bufStart + curState.parseIdx
                let (cr, eat) = Utf8.decodeRune(@mkslice(ptr, me.bufLength - curState.parseIdx))
                let c = me.ignoreCase ? Unicode.toUpper(cr) : cr

                me.dfaMatch(pstack2, c, eat, &curState)
            }

            swap(&pstack1, &pstack2)

            var cptReachedEnd = 0
            foreach &v in dref pstack1
            {
                if v.parseIdx >= me.bufLength:
                    cptReachedEnd += 1

                if v.node.terminal and forGrep:
                    grepTerm = dref v
                elif v.node.terminal and v.parseIdx >= me.bufLength:
                    mustEnd = true
            }

            if cptReachedEnd == pstack1.count:
                mustEnd = true
        }

        // Get the latest terminal node for a grep
        if pstack1.isEmpty() and grepTerm.node:
            pstack1.add(grepTerm)

        for
        {
            while !pstack1.isEmpty()
            {
                var curState = pstack1.popBack()
                if curState.node.terminal and (forGrep or curState.parseIdx >= me.bufLength)
                {
                    me.closeGroup(&curState)
                    me.groups = #move curState.groups
                    return {true, me.bufStart + curState.parseIdx}
                }

                me.dfaMatch(pstack2, RuneEnd, 0, &curState)
            }

            if pstack2.isEmpty():
                break
            swap(&pstack1, &pstack2)
        }

        return result
    }

    mtd closeGroup(state: *DfaState)
    {
        let ptr = me.bufStart + state.parseIdx
        if state.lastGroup
        {
            let strGroup = @mkstring(state.groupStart, ptr - state.groupStart)
            setCapture(state, state.lastGroup, strGroup)
        }

        state.lastGroup  = state.node.group
        state.groupStart = ptr
    }

    mtd backward(out: *Array'DfaState, what: rune, state: *DfaState)
    {
        let grp = state.node.transitions.tryFind(what)
        if grp
        {
            me.closeGroup(state)

            var strGroup: string
            foreach g in state.groups
            {
                if g.rank == 1 + (what - RuneGroup1)
                {
                    strGroup = g.value
                    break
                }
            }

            if @countof(strGroup)
            {
                let ptr = me.bufStart + state.parseIdx
                if Utf8.startsWith(@mkslice(ptr, me.bufLength - state.parseIdx), strGroup, me.ignoreCase ? .UnicodeNoCase : .Unicode)
                {
                    var st = dref state
                    st.node = grp.value
                    st.parseIdx += cast(s32) @countof(strGroup)
                    out.add(st)
                }
            }
        }
    }

    mtd dfaMatch(out: *Array'DfaState, c: rune, eat: s32, curState: *DfaState)
    {
        var ok = false

        #[Swag.Mixin]
        func checkTr(c: rune, length: s32)
        {
            let #uniq1 = curState.node.transitions.tryFind(c)
            if #uniq1
            {
                var st = dref curState
                st.node = #uniq1.value
                if st.node.group != st.lastGroup:
                    me.closeGroup(&st)
                st.parseIdx += length
                out.add(st)
                ok = true
            }
        }

        // Word transition
        if !me.hasStarted
        {
            checkTr(RuneStart, 0)
            checkTr(RuneWordSwap, 0)
            me.hasStarted = true
            if ok:
                return
        }
        elif !me.wasWord and Unicode.isWord(c)
        {
            me.wasWord = true
            checkTr(RuneWordSwap, 0)
            if ok:
                return
        }
        elif me.wasWord and !Unicode.isWord(c)
        {
            me.wasWord = false
            checkTr(RuneWordSwap, 0)
            if ok:
                return
        }

        // Backreference
        for 9
        {
            me.backward(out, RuneGroup1 + @index, curState)
        }

        if Unicode.isAscii(c)
        {
            let c8 = cast(u8) c

            if Latin1.isDigit(c8):
                checkTr(RuneAsciiDigit, eat)
            else:
                checkTr(RuneAsciiDigit | RuneInvert, eat)

            if Latin1.isHexDigit(c8):
                checkTr(RuneAsciiHexa, eat)
            else:
                checkTr(RuneAsciiHexa | RuneInvert, eat)

            if Latin1.isLetter(c8):
                checkTr(RuneAsciiLetter, eat)
            else:
                checkTr(RuneAsciiLetter | RuneInvert, eat)

            if Latin1.isLower(c8):
                checkTr(RuneAsciiLower, eat)
            else:
                checkTr(RuneAsciiLower | RuneInvert, eat)

            if Latin1.isUpper(c8):
                checkTr(RuneAsciiUpper, eat)
            else:
                checkTr(RuneAsciiUpper | RuneInvert, eat)

            if Latin1.isLetterOrDigit(c8):
                checkTr(RuneAsciiAlNum, eat)
            else:
                checkTr(RuneAsciiAlNum | RuneInvert, eat)

            if Latin1.isLetterOrDigit(c8) or c8 == '_':
                checkTr(RuneAsciiWord, eat)
            else:
                checkTr(RuneAsciiWord | RuneInvert, eat)

            if Latin1.isSpace(c8):
                checkTr(RuneAsciiSpace, eat)
            else:
                checkTr(RuneAsciiSpace | RuneInvert, eat)

            if c8 == ' ' or c8 == '\t':
                checkTr(RuneAsciiBlank, eat)
            else:
                checkTr(RuneAsciiBlank | RuneInvert, eat)

            checkTr(RuneAscii, eat)
        }
        else
        {
            checkTr(RuneAscii | RuneInvert, eat)
        }

        if Unicode.isLetter(c):
            checkTr(RuneUnicodeLetter, eat)
        if Unicode.isLower(c):
            checkTr(RuneUnicodeLetterLower, eat)
        if Unicode.isUpper(c):
            checkTr(RuneUnicodeLetterUpper, eat)
        if Unicode.isNumber(c):
            checkTr(RuneUnicodeNumber, eat)
        if Unicode.isSymbol(c):
            checkTr(RuneUnicodeSymbol, eat)
        if Unicode.isSymbolMath(c):
            checkTr(RuneUnicodeSymbolMath, eat)

        checkTr(c, eat)
        checkTr(RuneWhatever, eat)
        checkTr(0'rune, eat)
    }

    func addTransition(node: *NfaNode, c: rune, toNode: *NfaNode)
    {
        var here = node.transitions.tryFind(c)
        if !here
        {
            var toT: Array'(*NfaNode)
            here = node.transitions.add(c, toT)
        }

        here.value.add(toNode)
    }

    mtd nfaPop()->*NfaTable throw
    {
        if me.nfaStack.isEmpty():
            throw SyntaxError{"nfaPop"}
        return me.nfaStack.popBack()
    }

    // Creates 2 new states, and one transition (by c) between them
    mtd nfaPush(c: rune)
    {
        let table = me.nfaTables.addNewPtr()
        let s1    = me.nfaNodes.addNewPtr()
        let s2    = me.nfaNodes.addNewPtr()

        table.level = cast(u32) me.openParen.count
        s1.addTransition(c, s2)

        table.nodes.add(s1)
        table.nodes.add(s2)
        me.nfaStack.add(table)

        // Add the character to the list of used characters
        me.charSet.add(c)
    }

    mtd unionValidate() throw
    {
        if me.orOpStack.isEmpty():
            return
        if me.nfaStack.count < 2:
            return

        // Check if the two tables have the same nested level
        let table2 = me.nfaPop()
        let table1 = me.nfaPop()
        me.nfaStack.add(table1)
        me.nfaStack.add(table2)
        if table1.level != table2.level:
            return

        // Operator must be at the same level
        if me.orOpLevel[me.orOpLevel.count - 1] != table1.level:
            return

        // Else validate union
        me.orOpStack.removeBack()
        me.orOpLevel.removeBack()
        me.operatorUnion()
    }

    mtd operatorUnion() throw
    {
        let table2 = me.nfaPop()
        let table1 = me.nfaPop()

        let ns1 = Memory.new'NfaNode()
        let ns2 = Memory.new'NfaNode()
        me.nfaNodes.add(ns1)
        me.nfaNodes.add(ns2)

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
        me.nfaStack.add(table1)
    }

    mtd operatorConcat(cpt: u64, canGroup = true) throw
    {
        var count = cpt
        while count >= 2
        {
            let table2 = me.nfaPop()
            let table1 = me.nfaPop()

            // Update nested level
            table1.level = cast(u32) me.openParen.count
            table2.level = cast(u32) me.openParen.count

            let s1 = table1.nodes.back()
            let s2 = table2.nodes.front()
            s1.addTransition(0'rune, s2)

            table1.nodes.add(table2.nodes.toSlice())
            me.nfaStack.add(table1)

            count -= 1
        }

        // Update nested level
        if count == 1
        {
            let table1 = me.nfaPop()
            table1.level = cast(u32) me.openParen.count

            if canGroup
            {
                me.group += 1
                foreach n in table1.nodes
                {
                    if n.group == 0:
                        n.group = me.group
                }
            }

            me.nfaStack.add(table1)
        }
    }

    mtd operatorStar() throw
    {
        let table = me.nfaPop()
        let s1    = table.nodes.front()
        let s2    = table.nodes.back()
        s1.addTransition(0'rune, s2)
        s2.addTransition(0'rune, s1)
        me.nfaStack.add(table)
    }

    mtd operatorPlus() throw
    {
        let table = me.nfaPop()
        let s1    = table.nodes.front()
        let s2    = table.nodes.back()
        s2.addTransition(0'rune, s1)
        me.nfaStack.add(table)
    }

    mtd operatorQuestion() throw
    {
        let table = me.nfaPop()
        let s1    = table.nodes.front()
        let s2    = table.nodes.back()
        s1.addTransition(0'rune, s2)
        me.nfaStack.add(table)
    }

    func isReservedChar(c: rune)->bool
    {
        switch c
        {
        case '\\', '(', ')', '*', '?', '+', '.', '|', '[', ']', '{', '}', '/':
            return true
        }

        return false
    }

    mtd nfaBuild() throw
    {
        var pz  = me.preprocessed.buffer
        var cpt = 0

        while cpt < me.preprocessed.count
        {
            var c = dref pz
            switch c
            {
            case '(', RuneOpenParen:
                me.openParen.add(cast(u32) me.nfaStack.count)

            case RuneCloseParen:
                me.unionValidate()
                let op = me.openParen.popBack()
                me.operatorConcat(me.nfaStack.count - op, false)

            case ')':
                me.unionValidate()
                if me.openParen.isEmpty():
                    throw SyntaxError{"unexpected right parenthesis"}
                let op = me.openParen.popBack()
                me.operatorConcat(me.nfaStack.count - op)

            case '*':
                me.operatorStar()

            case '+':
                me.operatorPlus()

            case '?':
                me.operatorQuestion()

            case '|':
                me.unionValidate()
                me.orOpStack.add(c)
                me.orOpLevel.add(cast(u32) me.openParen.count)

            case '.':
                me.unionValidate()
                me.nfaPush(RuneWhatever)

            case '\\':
                if isReservedChar(pz[1])
                {
                    c = pz[1]
                    cpt, pz += 1
                }

                me.unionValidate()
                me.nfaPush(c)

            default:
                me.unionValidate()
                me.nfaPush(c)
            }

            cpt, pz += 1
        }

        // Remaining operators
        me.unionValidate()
        if !me.orOpStack.isEmpty():
            throw SyntaxError{}

        // Concat the remaining nodes
        me.operatorConcat(me.nfaStack.count)
    }

    func dfaEClosure(inH, outH: *Array'(*NfaNode))
    {
        outH.clear()

        var stack: Array'(*NfaNode)
        foreach n in dref inH:
            stack.add(n)

        while stack.count
        {
            let state = stack.popBack()
            outH.addOnce(state)

            let find = state.transitions.tryFind(0'rune)
            if !find:
                continue

            foreach t in find.value
            {
                if !outH.contains(t):
                    stack.add(t)
            }
        }
    }

    mtd setTable(node: *DfaNode, tbl: *Array'(*NfaNode))
    {
        node.table = #move dref tbl
        foreach n in node.table
        {
            if node.group == 0:
                node.group = me.groupTransform[n.group]

            if n.terminal:
                node.terminal = true
        }
    }

    func dfaMoveNode(out: *Array'(*NfaNode), c: rune, inH: Array'(*NfaNode))
    {
        out.clear()
        foreach state in inH
        {
            // The real terminal character
            let pfind = state.transitions.tryFind(c)
            if pfind
            {
                foreach t in pfind.value:
                    out.add(t)
            }
        }
    }

    func equals(s1, s2: *Array'(*NfaNode))->bool
    {
        if s1.count != s2.count:
            return false
        foreach t1 in dref s1
        {
            if !s2.contains(t1):
                return false
        }

        return true
    }

    mtd dfaBuild() throw
    {
        // One table must remain
        let table = me.nfaPop()

        // Last state of NFA table is a terminal
        table.nodes.back().terminal = true

        // Group transformation
        var grp: Array'u32
        foreach n in table.nodes:
            grp.add(n.group)
        grp.sort()

        var curGroup, curGroupSwap: u32

        me.groupTransform.resize(cast(u64) me.group + 1)
        foreach n in grp
        {
            if n != curGroup:
                curGroupSwap += 1
            curGroup          = n
            me.groupTransform[n] = curGroupSwap
        }

        // Build the first state of the DFA table
        var nfaStart, dfaStart: Array'(*NfaNode)
        nfaStart.addOnce(table.nodes.front())
        dfaEClosure(&nfaStart, &dfaStart)

        var state = Memory.new'DfaNode()
        me.setTable(state, &dfaStart)
        me.dfaNodes.add(state)

        var moveRes, epsilonRes: Array'(*NfaNode)
        var stack: Array'(*DfaNode)
        stack.add(state)
        while stack.count
        {
            state = stack.popBack()
            foreach c in me.charSet
            {
                dfaMoveNode(&moveRes, c, state.table)
                if moveRes.count == 0:
                    continue
                dfaEClosure(&moveRes, &epsilonRes)

                var found: *DfaNode
                foreach tstate in me.dfaNodes
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
                    me.setTable(found, &epsilonRes)
                    stack.add(found)
                    me.dfaNodes.add(found)
                }

                state.transitions.add(c, found)
            }
        }
    }

    #[Swag.Discardable]
    mtd preprocessInter(sl: const [..] rune)->const ^rune throw
    {
        var pz      = @dataof(sl)
        let pzStart = pz
        let pzEnd   = pz + @countof(sl)

        me.preprocessed.add(RuneOpenParen)
        pz += 1

        // Negation
        var invert: rune
        if dref pz == '^':
            invert = RuneInvert

        var firstExp = true
        while pz != pzEnd and pz[0] != ']'
        {
            if !firstExp:
                me.preprocessed.add('|')
            firstExp = false

            let subSlice = @mkslice(pz, pzEnd - pz)

            // Predefined sets
            if pz[0] == '['
            {
                if Unicode.compare(subSlice, #runes("[:digit:]"), false, 9) == 0
                {
                    me.preprocessed.add(RuneAsciiDigit | invert)
                    pz += 9
                    continue
                }

                if Unicode.compare(subSlice, #runes("[:xdigit:]"), false, 10) == 0
                {
                    me.preprocessed.add(RuneAsciiHexa | invert)
                    pz += 10
                    continue
                }

                if Unicode.compare(subSlice, #runes("[:alpha:]"), false, 9) == 0
                {
                    me.preprocessed.add(RuneAsciiLetter | invert)
                    pz += 9
                    continue
                }

                if Unicode.compare(subSlice, #runes("[:alnum:]"), false, 9) == 0
                {
                    me.preprocessed.add(RuneAsciiAlNum | invert)
                    pz += 9
                    continue
                }

                if Unicode.compare(subSlice, #runes("[:lower:]"), false, 9) == 0
                {
                    me.preprocessed.add(RuneAsciiLower | invert)
                    pz += 9
                    continue
                }

                if Unicode.compare(subSlice, #runes("[:upper:]"), false, 9) == 0
                {
                    me.preprocessed.add(RuneAsciiUpper)
                    pz += 9
                    continue
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), #runes("[:blank:]"), false, 9) == 0
                {
                    me.preprocessed.add(RuneAsciiBlank | invert)
                    pz += 9
                    continue
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), #runes("[:space:]"), false, 9) == 0
                {
                    me.preprocessed.add(RuneAsciiSpace | invert)
                    pz += 9
                    continue
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), #runes("[:ascii:]"), false, 9) == 0
                {
                    me.preprocessed.add(RuneAscii | invert)
                    pz += 9
                    continue
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), #runes("[:word:]"), false, 8) == 0
                {
                    me.preprocessed.add(RuneAsciiWord | invert)
                    pz += 8
                    continue
                }
            }

            // Specified interval
            if @countof(subSlice) >= 3 and pz[1] == '-' and pz[2] != ']'
            {
                var first = pz[0]
                var end   = pz[2]
                if first > end:
                    swap(&first, &end)

                if first == 'A' and end == 'Z':
                    me.preprocessed.add(RuneAsciiUpper)
                elif first == 'a' and end == 'z':
                    me.preprocessed.add(RuneAsciiLower)
                elif first == '0' and end == '9':
                    me.preprocessed.add(RuneAsciiDigit)
                else
                {
                    while first < end
                    {
                        me.preprocessed.add(first)
                        me.preprocessed.add('|')
                        first += 1
                    }

                    me.preprocessed.add(first)
                }

                pz += 3
                continue
            }

            // This is not an interval
            if isReservedChar(dref pz)
            {
                me.preprocessed.add('\\')
                me.preprocessed.add(dref pz)
                pz += 1
            }
            else
            {
                pz = me.preprocessEscape(@mkslice(pz, pz - pzStart), true)
            }
        }

        if pz != pzEnd:
            pz += 1
        me.preprocessed.add(RuneCloseParen)

        return pz
    }

    func toI32(pz, pzEnd: const ^rune, val: *s32)->const ^rune throw
    {
        if !Unicode.isLatin1(pz[0]) or !Latin1.isDigit(cast(u8) pz[0]):
            throw SyntaxError{}

        var pzStart = pz
        var n       = 0
        while pzStart != pzEnd and Unicode.isLatin1(pzStart[0]) and Latin1.isDigit(cast(u8) pzStart[0])
        {
            n *= 10
            n += cast(u8) pzStart[0] - '0'
            pzStart += 1
        }

        dref val = n
        return pzStart
    }

    mtd preprocessEnum(sl: const [..] rune)->const ^rune throw
    {
        var pz    = @dataof(sl)
        let pzEnd = pz + @countof(sl)

        if me.preprocessed.count == 0:
            throw SyntaxError{"'{' without a previous expression"}

        var m = -1
        pz += 1

        // n value
        var n: s32 = 0
        pz = toI32(pz, pzEnd, &n)

        // m value
        if pz[0] == ','
        {
            m = -1
            pz += 1
            if pz[0] != '}':
                pz = toI32(pz, pzEnd, &m)
        }
        else
        {
            m = n
        }

        if pz[0] != '}':
            throw SyntaxError{"enum, missing '}'"}
        if m != -1 and m < n:
            throw SyntaxError{"enum m < n in {n,m}"}
        if m == 0 and n == 0:
            throw SyntaxError{"enum m and n are zero in {n,m}"}

        pz += 1

        if n == 0 and m == -1:
            me.preprocessed.add('*')
        elif n == 0 and m == 1:
            me.preprocessed.add('?')
        elif n == 1 and m == -1:
            me.preprocessed.add('+')
        elif n != 1 or m != 1 // {1,1}
        {
            // Isolate expression to duplicate
            let pze      = me.preprocessed.buffer + me.preprocessed.count - 1
            var pz1      = pze
            var cptParen = 0
            if pz1[0] == ')' or pz1[0] == RuneCloseParen:
                cptParen += 1
            while cptParen and pz1 != me.preprocessed.buffer
            {
                pz1 -= 1
                if pz1[0] == ')' or pz1[0] == RuneCloseParen:
                    cptParen += 1
                if pz1[0] == '(' or pz1[0] == RuneOpenParen:
                    cptParen -= 1
            }

            if cptParen:
                throw SyntaxError{"missing '('"}

            var expr: Array'rune
            while pz1 <= pze
            {
                expr.add(pz1[0])
                pz1 += 1
            }

            if n == 0:
                me.preprocessed.add(expr.toSlice())
            else
            {
                var i = n - 1
                while i
                {
                    me.preprocessed.add(expr.toSlice())
                    i -= 1
                }
            }

            if m == -1
            {
                me.preprocessed.add('+')
            }
            else
            {
                if n == 0:
                    m -= 1
                m -= n
                while m
                {
                    me.preprocessed.add(expr.toSlice())
                    me.preprocessed.add('?')
                    m -= 1
                }
            }
        }

        return pz
    }

    mtd preprocessUnicode(sl: const [..] rune)->const ^rune throw
    {
        let pz = @dataof(sl) + 2
        if pz[0] == '{'
        {
            if pz[1] == 'L' and pz[3] == '}'
            {
                switch pz[2]
                {
                case 'l':
                    me.preprocessed.add(RuneUnicodeLetterLower)
                    return pz + 4
                case 'u':
                    me.preprocessed.add(RuneUnicodeLetterUpper)
                    return pz + 4
                }
            }
            elif pz[1] == 'S' and pz[3] == '}'
            {
                switch pz[2]
                {
                case 'm':
                    me.preprocessed.add(RuneUnicodeSymbolMath)
                    return pz + 4
                }
            }
        }

        switch pz[0]
        {
        case 'L':
            me.preprocessed.add(RuneUnicodeLetter)
            return pz + 1
        case 'N':
            me.preprocessed.add(RuneUnicodeNumber)
            return pz + 1
        case 'S':
            me.preprocessed.add(RuneUnicodeSymbol)
            return pz + 1
        }

        return pz
    }

    mtd preprocessEscape(sl: const [..] rune, inInter = false)->const ^rune throw
    {
        var pz = @dataof(sl)
        let c  = dref pz
        if c == '\\'
        {
            switch pz[1]
            {
            case 'd':
                me.preprocessInter(#runes("[[:digit:]]"))
                return pz + 2
            case 'D':
                me.preprocessInter(#runes("[^[:digit:]]"))
                return pz + 2
            case 'w':
                me.preprocessInter(#runes("[[:word:]]"))
                return pz + 2
            case 'W':
                me.preprocessInter(#runes("[^[:word:]]"))
                return pz + 2
            case 's':
                me.preprocessInter(#runes("[[:space:]]"))
                return pz + 2
            case 'S':
                me.preprocessInter(#runes("[^[:space:]]"))
                return pz + 2
            case 'l':
                me.preprocessInter(#runes("[[:lower:]]"))
                return pz + 2
            case 'L':
                me.preprocessInter(#runes("[^[:lower:]]"))
                return pz + 2
            case 'u':
                me.preprocessInter(#runes("[[:upper:]]"))
                return pz + 2
            case 'U':
                me.preprocessInter(#runes("[^[:upper:]]"))
                return pz + 2
            case 'a':
                me.preprocessed.add('\a')
                return pz + 2
            case 'b':
                me.preprocessed.add(RuneWordSwap)
                return pz + 2
            case 'f':
                me.preprocessed.add('\f')
                return pz + 2
            case 'n':
                me.preprocessed.add('\n')
                return pz + 2
            case 'r':
                me.preprocessed.add('\r')
                return pz + 2
            case 't':
                me.preprocessed.add('\t')
                return pz + 2
            case 'v':
                me.preprocessed.add('\v')
                return pz + 2
            case 'p':
                pz = me.preprocessUnicode(sl)
                return pz
            case '1', '2', '3', '4', '5', '6', '7', '8', '9':
                me.preprocessed.add(RuneGroup1 + (pz[1] - '1'))
                return pz + 2
            }
        }

        if c == '^' and !inInter:
            me.preprocessed.add(RuneStart)
        elif c == '$' and !inInter:
            me.preprocessed.add(RuneEnd)
        else:
            me.preprocessed.add(c)

        return pz + 1
    }

    func addParenthesis(uni32: &Array'rune)
    {
        for var i = 1's32; i < uni32.count - 1; i += 1
        {
            if uni32[i] == '|'
            {
                uni32.insertAt(cast() i, ')''rune)
                uni32.insertAt(cast() (i + 2), '(''rune)

                var cpt0, cpt1, cpt2: s32
                var iminus = i - 1

                #scope(Loop)
                while iminus != -1
                {
                    switch uni32[iminus]
                    {
                    case ']':
                        cpt0 += 1
                    case ')':
                        cpt1 += 1
                    case '}':
                        cpt2 += 1
                    case '[':
                        if cpt0 == 0:
                            break to Loop
                        cpt0 -= 1
                    case '(':
                        if cpt1 == 0:
                            break to Loop
                        cpt1 -= 1
                    case '{':
                        if cpt2 == 0:
                            break to Loop
                        cpt2 -= 1
                    }

                    iminus -= 1
                }

                uni32.insertAt(cast() (iminus + 1), '(''rune)

                cpt0, cpt1, cpt2 = 0
                var iplus = i + 4

                #scope(Loop)
                while iplus != uni32.count
                {
                    switch uni32[iplus]
                    {
                    case '[':
                        cpt0 += 1
                    case '(':
                        cpt1 += 1
                    case '{':
                        cpt2 += 1
                    case ']':
                        if cpt0 == 0:
                            break to Loop
                        cpt0 -= 1
                    case ')':
                        if cpt1 == 0:
                            break to Loop
                        cpt1 -= 1
                    case '}':
                        if cpt2 == 0:
                            break to Loop
                        cpt2 -= 1
                    }

                    iplus += 1
                }

                uni32.insertAt(cast() iplus, ')''rune)
                i += 4
            }
        }
    }

    mtd preprocess(inStr: string) throw
    {
        var in32 = Unicode.fromUtf8(inStr)
        addParenthesis(&in32)
        in32.add(0'rune)

        var pz    = cast(const ^rune) in32.buffer
        let pzEnd = pz + in32.count

        var c = me.ignoreCase ? Unicode.toUpper(pz[0]) : pz[0]
        while c
        {
            switch c
            {
            case '[':
                pz = me.preprocessInter(@mkslice(pz, pzEnd - pz))
            case '{':
                pz = me.preprocessEnum(@mkslice(pz, pzEnd - pz))
            default:
                pz = me.preprocessEscape(@mkslice(pz, pzEnd - pz))
            }

            if !pz:
                break
            c = me.ignoreCase ? Unicode.toUpper(pz[0]) : pz[0]
        }
    }
}
