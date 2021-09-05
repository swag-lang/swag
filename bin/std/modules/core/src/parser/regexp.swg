#global namespace Parser

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
};

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
}

impl RegExp
{
    mtd opDrop()
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
    public mtd compile(str: string) throw
    {
        clear()
        try preprocess(str)
        try nfaBuild()
        try dfaBuild()
    }

    // Returns true if 'str' matches the regexp
    public mtd match(str: string)->bool
    {
        if dfaNodes.isEmpty()
            return false

        var stack1, stack2: Array'(*DfaNode)
        stack1.add(dfaNodes[0])
        pstack1 := &stack1
        pstack2 := &stack2

        cpt       := 0
        srcPtr    := @dataof(str)
        srcLength := @countof(str)

        while cpt < srcLength and !pstack1.isEmpty()
        {
            (c, eat) := Utf8.decodeRune(@mkslice(srcPtr, srcLength))

            ok := false
            while !pstack1.isEmpty()
            {
                curState := pstack1.popBack()
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
            curState := pstack1.popBack()
            if curState.terminal
                return true
        }

        return false
    }

    mtd dfaMatch(out: *Array'(*DfaNode), c: rune, curState: *DfaNode)->bool
    {
        ok := false

        // Is there a transition with the input character ?
        find := curState.transitions.find(c)
        if find
        {
            out.add(find.value)
            ok = true
        }

        // Is there a transition with the "any" operator ?
        find = curState.transitions.find(0)
        if find
        {
            out.add(find.value)
            ok = true
        }

        return ok
    }

    func addTransition(node: *NfaNode, c: rune, toNode: *NfaNode)
    {
        here := node.transitions.find(c)
        if !here
        {
            var to: Array'(*NfaNode)
            here = node.transitions.add(c, to)
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
        table := Memory.new'NfaTable()
        s1    := Memory.new'NfaNode()
        s2    := Memory.new'NfaNode()

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
        table2 := try nfaPop()
        table1 := try nfaPop()

        ns1 := Memory.new'NfaNode()
        ns2 := Memory.new'NfaNode()
        nfaNodes.add(ns1)
        nfaNodes.add(ns2)

        s1 := table1.nodes.front()
        s2 := table1.nodes.back()
        s3 := table2.nodes.front()
        s4 := table2.nodes.back()

        ns1.addTransition(0, s1)
        s2.addTransition(0, ns2)
        ns1.addTransition(0, s3)
        s4.addTransition(0, ns2)

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
        // @optim
        table2 := try nfaPop()
        table1 := try nfaPop()
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
        table := try nfaPop()
        s1 := table.nodes.front()
        s2 := table.nodes.back()
        s1.addTransition(0, s2)
        s2.addTransition(0, s1)
        nfaStack.add(table)
    }

    mtd operatorPlus() throw
    {
        table := try nfaPop()
        s1 := table.nodes.front()
        s2 := table.nodes.back()
        s2.addTransition(0, s1)
        nfaStack.add(table)
    }

    mtd operatorQuestion() throw
    {
        table := try nfaPop()
        s1 := table.nodes.front()
        s2 := table.nodes.back()
        s1.addTransition(0, s2)
        nfaStack.add(table)
    }

    func isReservedChar(c: rune)->bool
    {
        switch c
        {
        case "\\"'rune, "("'rune, ")"'rune, "*"'rune, "?"'rune,
             "+"'rune, "."'rune, "|"'rune, "["'rune, "]"'rune,
             "{"'rune, "}"'rune:
             return true
        }

        return false
    }

    mtd nfaBuild() throw
    {
        pz  := preprocessed.buffer
        cpt := 0

        while cpt < preprocessed.count
        {
            c := dref pz
            switch c
            {
            case "("'rune:
                openParen.add(cast(u32) nfaStack.count)

            case ")"'rune:
                try unionValidate()
                if openParen.isEmpty()
                    throw "syntax, unexpected right parenthesis"
                op := openParen.popBack()
                try operatorConcat(nfaStack.count - op)

            case "*"'rune:
                try operatorStar()

            case "+"'rune:
                try operatorPlus()

            case "?"'rune:
                try operatorQuestion()

            case "|"'rune:
                try unionValidate()
                orOpStack.add(c)
                orOpLevel.add(cast(u32) openParen.count)

            case "."'rune:
                try unionValidate()
                nfaPush(0)

            case "\\"'rune:
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

    mtd operatorConcat(cpt: uint) throw
    {
        count := cpt
        while count >= 2
        {
            table2 := try nfaPop()
            table1 := try nfaPop()

            // Update nested level
            table1.level = cast(u32) openParen.count
            table2.level = cast(u32) openParen.count

            s1 := table1.nodes.back()
            s2 := table2.nodes.front()
            s1.addTransition(0, s2)

            table1.nodes.add(table2.nodes.toSlice())
            nfaStack.add(table1)

            count -= 1
        }

        // Update nested level
        if count == 1
        {
            table1 := try nfaPop()
            table1.level = cast(u32) openParen.count
            nfaStack.add(table1)
        }
    }

    mtd dfaEClosure(in, out: *HashSet'(*NfaNode))
    {
        out.clear()

        var stack: Array'(*NfaNode)
        visit n: dref in
            stack.add(n)

        while stack.count
        {
            state := stack.popBack()
            out.add(state)

            find := state.transitions.find(0)
            if !find continue

            visit to: find.value
            {
                if !out.contains(to)
                    stack.add(to)
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
                node.terminal = true;
                break
            }
        }
    }

    func dfaMoveNode(out: *HashSet'(*NfaNode), c: rune, in: HashSet'(*NfaNode))
    {
        out.clear()
        visit state: in
        {
            // The real terminal character
            pfind := state.transitions.find(c)
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
        table := try nfaPop()

        // Last state of NFA table is a terminal
        table.nodes.back().terminal = true

        // Build the first state of the DFA table
        var nfaStart, dfaStart: HashSet'(*NfaNode)
        nfaStart.add(table.nodes.front())
        dfaEClosure(&nfaStart, &dfaStart)

        state := Memory.new'DfaNode()
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

    mtd preprocessInter(sl: const [..] rune)->const *rune throw
    {
        pz      := @dataof(sl)
        pzStart := pz
        pzEnd   := pz + @countof(sl)

        preprocessed.add("("'rune)
        pz += 1

        while pz != pzEnd and pz[0] != "]"'rune
        {
            // Predefined sets
            if pz[0] == "["'rune
            {
                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:digit:]"), false, 9) == 0
                {
                    discard try preprocessInter(@runes("[0-9]"));
                    pz += 9
                    if pz[0] != "]"'rune
                        preprocessed.add("|"'rune)
                    continue;
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:hexa:]"), false, 8) == 0
                {
                    discard try preprocessInter(@runes("[a-fA-F0-9]"));
                    pz += 8
                    if pz[0] != "]"'rune
                        preprocessed.add("|"'rune)
                    continue;
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:alpha:]"), false, 9) == 0
                {
                    discard try preprocessInter(@runes("[a-zA-Z]"));
                    pz += 9
                    if pz[0] != "]"'rune
                        preprocessed.add("|"'rune)
                    continue;
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:alnum:]"), false, 9) == 0
                {
                    discard try preprocessInter(@runes("[a-zA-Z0-9]"));
                    pz += 9
                    if pz[0] != "]"'rune
                        preprocessed.add("|"'rune)
                    continue;
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:lower:]"), false, 9) == 0
                {
                    discard try preprocessInter(@runes("[a-z]"));
                    pz += 9
                    if pz[0] != "]"'rune
                        preprocessed.add("|"'rune)
                    continue;
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:upper:]"), false, 9) == 0
                {
                    discard try preprocessInter(@runes("[A-Z]"));
                    pz += 9
                    if pz[0] != "]"'rune
                        preprocessed.add("|"'rune)
                    continue;
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:blank:]"), false, 9) == 0
                {
                    preprocessed.add(@runes(" |\t"))
                    pz += 9
                    if pz[0] != "]"'rune
                        preprocessed.add("|"'rune)
                    continue;
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:space:]"), false, 9) == 0
                {
                    preprocessed.add(@runes(" |\f|\n|\r|\t|\v"))
                    pz += 9
                    if pz[0] != "]"'rune
                        preprocessed.add("|"'rune)
                    continue;
                }

                if Unicode.compare(@mkslice(pz, pzEnd - pz), @runes("[:word:]"), false, 8) == 0
                {
                    discard try preprocessInter(@runes("[a-zA-Z0-9_]"));
                    pz += 8
                    if pz[0] != "]"'rune
                        preprocessed.add("|"'rune)
                    continue;
                }
            }

            if pz[1] == "-"'rune and pz[2] != "]"'rune
            {
                first := pz[0]
                end   := pz[2]
                if first > end
                    swap(&first, &end)
                while first < end
                {
                    preprocessed.add(first)
                    preprocessed.add("|"'rune)
                    first += 1
                }

                preprocessed.add(first)
                pz += 3
                if pz[0] != "]"'rune
                    preprocessed.add("|"'rune)
                continue
            }

            if isReservedChar(dref pz)
            {
                preprocessed.add("\\"'rune)
                preprocessed.add(dref pz)
                pz += 1
            }
            else
            {
                pz = try preprocessEscape(@mkslice(pz, pz - pzStart))
            }

            if pz[0] != "]"'rune
                preprocessed.add("|"'rune)
        }

        if pz != pzEnd
            pz += 1
        preprocessed.add(")"'rune)

        return pz
    }

    mtd preprocessEnum(sl: const [..] rune)->const *rune throw
    {
        pz := @dataof(sl)
        return pz
    }

    mtd preprocessEscape(sl: const [..] rune)->const *rune throw
    {
        pz := @dataof(sl)
        c := dref pz
        if c == "\\"'rune
        {
            switch pz[1]
            {
            case "d"'rune:
                discard try preprocessInter(@runes("[[:digit:]]"))
                return pz + 2
            case "w"'rune:
                discard try preprocessInter(@runes("[[:word:]]"))
                return pz + 2
            case "s"'rune:
                discard try preprocessInter(@runes("[[:space:]]"))
                return pz + 2
            case "l"'rune:
                discard try preprocessInter(@runes("[[:lower:]]"))
                return pz + 2
            case "u"'rune:
                discard try preprocessInter(@runes("[[:upper:]]"))
                return pz + 2
            case "a"'rune:
                preprocessed.add("\a"'rune)
                return pz + 2
            case "f"'rune:
                preprocessed.add("\f"'rune)
                return pz + 2
            case "n"'rune:
                preprocessed.add("\n"'rune)
                return pz + 2
            case "r"'rune:
                preprocessed.add("\r"'rune)
                return pz + 2
            case "t"'rune:
                preprocessed.add("\t"'rune)
                return pz + 2
            case "v"'rune:
                preprocessed.add("\v"'rune)
                return pz + 2
            }
        }

        preprocessed.add(c)
        return pz + 1
    }

    mtd preprocess(in: string) throw
    {
        in32 := Unicode.fromUtf8(in)
        in32.add(0)

        start := cast(const *rune) in32.buffer
        pz    := cast(const *rune) in32.buffer
        pzEnd := pz + in32.count
        c     := dref pz

        while c
        {
            switch c
            {
            case "["'rune:
                pz = try preprocessInter(@mkslice(pz, pzEnd - pz))
                break

            case "{"'rune:
                pz = try preprocessEnum(@mkslice(pz, pzEnd - pz))
                break

            default:
                pz = try preprocessEscape(@mkslice(pz, pzEnd - pz))
                break
            }

            c = dref pz
        }
    }
}