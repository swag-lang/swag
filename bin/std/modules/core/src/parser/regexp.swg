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
    m_Nfa:          Array'(*NfaNode)
    m_Dfa:          Array'(*DfaNode)
    m_NfaStack:     Array'(*NfaTable)
    m_CharSet:      HashSet'rune
    m_OpParen:      Array'u32
    m_OrOpStack:    Array'rune
    m_OrOpLevel:    Array'u32
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
        visit p: m_Nfa
            Memory.delete(p)
        m_Nfa.clear()

        visit p: m_Dfa
            Memory.delete(p)
        m_Dfa.clear()

        visit p: m_NfaStack
            Memory.delete(p)
        m_NfaStack.clear()

        m_CharSet.clear()
        m_OpParen.clear()
        m_OrOpStack.clear()
        m_OrOpLevel.clear()
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
        if m_Dfa.isEmpty()
            return false

        var stack1, stack2: Array'(*DfaNode)
        stack1.add(m_Dfa[0])
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
        if m_NfaStack.isEmpty()
            throw "nfaPop"
        return m_NfaStack.popBack()
    }

    mtd nfaPush(c: rune)
    {
        // Creates 2 new states, and one transition (by c) between them
        table        := Memory.new'NfaTable()
        table.level  = cast(u32) m_OpParen.count
        s1           := Memory.new'NfaNode()
        s2           := Memory.new'NfaNode()
        m_Nfa.add(s1)
        m_Nfa.add(s2)
        s1.addTransition(c, s2)

        table.nodes.add(s1)
        table.nodes.add(s2)
        m_NfaStack.add(table)

        // Add the character to the list of terminal characters.
        m_CharSet.add(c)
    }

    mtd operatorUnion() throw
    {
        table2 := try nfaPop()
        table1 := try nfaPop()

        ns1 := Memory.new'NfaNode()
        ns2 := Memory.new'NfaNode()
        m_Nfa.add(ns1)
        m_Nfa.add(ns2)

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
        m_NfaStack.add(table1)
        Memory.delete(table2)
    }

    mtd unionValidate() throw
    {
        if m_OrOpStack.isEmpty()
            return
        if m_NfaStack.count < 2
            return

        // Check if the two tables have the same nested level
        // @optim
        table2 := try nfaPop()
        table1 := try nfaPop()
        m_NfaStack.add(table1)
        m_NfaStack.add(table2)
        if table1.level != table2.level
            return

        // Operator must be at the same level
        if m_OrOpLevel[m_OrOpLevel.count - 1] != table1.level
            return

        // Else validate union
        m_OrOpStack.removeBack()
        m_OrOpLevel.removeBack()
        try operatorUnion()
    }

    mtd operatorStar() throw
    {
        table := try nfaPop()
        s1 := table.nodes.front()
        s2 := table.nodes.back()
        s1.addTransition(0, s2)
        s2.addTransition(0, s1)
        m_NfaStack.add(table)
    }

    mtd operatorPlus() throw
    {
        table := try nfaPop()
        s1 := table.nodes.front()
        s2 := table.nodes.back()
        s2.addTransition(0, s1)
        m_NfaStack.add(table)
    }

    mtd operatorQuestion() throw
    {
        table := try nfaPop()
        s1 := table.nodes.front()
        s2 := table.nodes.back()
        s1.addTransition(0, s2)
        m_NfaStack.add(table)
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
                m_OpParen.add(cast(u32) m_NfaStack.count)

            case ")"'rune:
                try unionValidate()
                if m_OpParen.isEmpty()
                    throw "syntax, unexpected right parenthesis"
                op := m_OpParen.popBack()
                try operatorConcat(m_NfaStack.count - op)

            case "*"'rune:
                try operatorStar()

            case "+"'rune:
                try operatorPlus()

            case "?"'rune:
                try operatorQuestion()

            case "|"'rune:
                try unionValidate()
                m_OrOpStack.add(c)
                m_OrOpLevel.add(cast(u32) m_OpParen.count)

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
        if !m_OrOpStack.isEmpty()
            throw "syntax"

        // Concat the remaining nodes
        try operatorConcat(m_NfaStack.count)
    }

    mtd operatorConcat(cpt: uint) throw
    {
        count := cpt
        while count >= 2
        {
            table2 := try nfaPop()
            table1 := try nfaPop()

            // Update nested level
            table1.level = cast(u32) m_OpParen.count
            table2.level = cast(u32) m_OpParen.count

            s1 := table1.nodes.back()
            s2 := table2.nodes.front()
            s1.addTransition(0, s2)

            table1.nodes.add(table2.nodes.toSlice())
            Memory.delete(table2)
            m_NfaStack.add(table1)

            count -= 1
        }

        // Update nested level
        if count == 1
        {
            table1 := try nfaPop()
            table1.level = cast(u32) m_OpParen.count
            m_NfaStack.add(table1)
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
        m_Dfa.add(state)

        var moveRes, epsilonRes: HashSet'(*NfaNode)
        var stack: Array'(*DfaNode)
        stack.add(state)
        while stack.count
        {
            state = stack.popBack()
            visit c: m_CharSet
            {
                dfaMoveNode(&moveRes, c, state.table)
                if moveRes.count == 0
                    continue
                dfaEClosure(&moveRes, &epsilonRes)

                var found: *DfaNode
                visit tstate: m_Dfa
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
                    m_Dfa.add(found)
                }

                state.transitions.add(c, found)
            }
        }

        Memory.delete(table)
    }

    mtd preprocessInter(pz: const *rune)->const *rune throw
    {
        return pz
    }

    mtd preprocessEnum(pz: const *rune)->const *rune throw
    {
        return pz
    }

    mtd preprocessEscape(pz: const *rune)->const *rune throw
    {
        c := dref pz
        if c == "\\"'rune
        {
            switch pz[1]
            {
            case "d"'rune:
                discard try preprocessInter(@dataof(@runes("[[:digit:]]")))
                return pz + 2
            case "w"'rune:
                discard try preprocessInter(@dataof(@runes("[[:word:]]")))
                return pz + 2
            case "s"'rune:
                discard try preprocessInter(@dataof(@runes("[[:space:]]")))
                return pz + 2
            case "l"'rune:
                discard try preprocessInter(@dataof(@runes("[[:lower:]]")))
                return pz + 2
            case "u"'rune:
                discard try preprocessInter(@dataof(@runes("[[:upper:]]")))
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

        pz := cast(const *rune) in32.buffer
        c  := dref pz
        while c
        {
            switch c
            {
            case "["'rune:
                pz = try preprocessInter(pz)
                break

            case "{"'rune:
                pz = try preprocessEnum(pz)
                break

            default:
                pz = try preprocessEscape(pz)
                break
            }

            c = dref pz
        }
    }
}