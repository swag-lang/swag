/*
    :LICENCE
    This code is based on the Clipper1 library (http://www.angusj.com/delphi/clipper.php)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global namespace Clipper
using Core

const LoRange       = 0x3FFFFFFF
const HiRange       = 0x3FFFFFFFFFFFFFFF's64
const Unassigned    = -1
const Skip          = -2
const HorizontalVal = -1.0E+40

public alias CInt = s64
alias ClipperPath  = Array'IntPoint
alias ClipperPaths = Array'ClipperPath

public enum PolyType
{
    Subject
    Clip
}
public enum ClipType
{
    Intersection
    Union
    Difference
    Xor
}
public enum PolyFillType
{
    EvenOdd
    NonZero
    Positive
    Negative
}
public enum InitOptions
{
    ReverseSolution   = 1
    StrictlySimple    = 2
    PreserveCollinear = 4
}
public enum JoinType
{
    Square
    Round
    Miter
}

enum EdgeSide
{
    Left  = 1
    Right = 2
}
enum Direction
{
    RightToLeft
    LeftToRight
}

public struct IntPoint { x, y: CInt }

struct IntRect
{
    left:       CInt
    top:        CInt
    right:      CInt
    bottom:     CInt
}

struct IntersectNode
{
    edge1:     *Edge
    edge2:     *Edge
    pt:        IntPoint
}

struct LocalMinimum
{
    y:              CInt
    leftBound:      *Edge
    rightBound:     *Edge
}

impl LocalMinimum
{
    mtd opCmp(other: LocalMinimum)->s32
    {
        return other.y <=> .y
    }
}

struct Edge
{
    bot:           IntPoint
    curr:          IntPoint
    top:           IntPoint
    dx:            f64
    polyType:      PolyType = undefined
    side:          EdgeSide = undefined
    windDelta:     s32
    windCnt:       s32
    windCnt2:      s32
    outIdx:        s32
    next:          *Edge
    prev:          *Edge
    nextInLML:     *Edge
    nextInAEL:     *Edge
    prevInAEL:     *Edge
    nextInSEL:     *Edge
    prevInSEL:     *Edge
}

struct OutRec
{
    idx:           s32
    isHole:        bool
    isOpen:        bool
    firstLeft:     *OutRec
    polyNd:        *PolyNode
    pts:           *OutPt
    bottomPt:      *OutPt
}

struct OutPt
{
    idx:      s32
    pt:       IntPoint
    next:     *OutPt
    prev:     *OutPt
}

struct PolyNode
{
    index:        u32
    isOpen:       bool
    jointype:     JoinType = undefined
    contour:      ClipperPath
    childs:       Array'*PolyNode
    parent:       *PolyNode
}

impl PolyNode
{
    mtd addChild(child: *PolyNode)
    {
        let cnt = cast(u32) .childs.count
        .childs.add(child)
        child.parent = me
        child.index  = cnt
    }
}

struct Join
{
    outPt1:     *OutPt
    outPt2:     *OutPt
    offPt:      IntPoint
}

#[Swag.Opaque]
public struct Transform
{
    useFullRange:          bool
    preserveCollinear:     bool
    usingPolyTree:         bool
    strictSimple:          bool
    reverseOutput:         bool
    edgeList:              Array'{ edge: [*] Edge, rank: u64 }
    minimaList:            Array'LocalMinimum
    polyOuts:              Array'*OutRec
    joinList:              Array'*Join
    ghostJoinList:         Array'*Join
    intersectList:         Array'*IntersectNode
    scanBeamList:          Array'CInt
    maximaList:            Array'CInt
    clipFillType:          PolyFillType = undefined
    subjFillType:          PolyFillType = undefined
    clipType:              ClipType = undefined
    currentLM:             u64
    activeEdges:           *Edge
    sortedEdges:           *Edge
}

impl Transform
{
    public mtd opDrop()
    {
        foreach it in .edgeList do
            Memory.delete(it.item0, it.item1)
    }

    mtd execute(clipT: ClipType, solution: *ClipperPaths, subjFillT, clipFillT: PolyFillType)
    {
        .subjFillType  = subjFillT
        .clipFillType  = clipFillT
        .clipType      = clipT
        .usingPolyTree = false

        .executeInternal()
        .buildResult(solution)
        .disposeAllOutRecs()
    }

    mtd addPath(pg: *ClipperPath, polyType: PolyType)
    {
        var highI = cast(s64) pg.count - 1
        while highI > 0 and (pg.buffer[highI] == pg.buffer[0]) do
            highI -= 1
        while highI > 0 and (pg.buffer[highI] == pg.buffer[highI - 1]) do
            highI -= 1
        if highI < 2 do
            return

        var edges  = Memory.new'Edge(cast(u64) highI + 1)
        var isFlat = true
        .edgeList.add({edges, cast(u64) highI + 1})

        // 1. Basic (first) edge initialization ...
        edges[1].curr = pg.buffer[1]
        .rangeTest(pg.buffer[0])
        .rangeTest(pg.buffer[highI])
        initEdge(&edges[0], &edges[1], &edges[highI], pg.buffer[0])
        initEdge(&edges[highI], &edges[0], &edges[highI - 1], pg.buffer[highI])
        for #reverse i in 1 until highI
        {
            .rangeTest(pg.buffer[i])
            initEdge(&edges[i], &edges[i + 1], &edges[i - 1], pg.buffer[i])
        }

        var eStart = &edges[0]

        // 2. Remove duplicate vertices, and (when closed) collinear edges ...
        var e, eLoopStop = eStart
        for
        {
            if e.curr == e.next.curr
            {
                if e == e.next do
                    break
                if e == eStart do
                    eStart = e.next
                e         = removeEdge(e)
                eLoopStop = e
                continue
            }

            if e.prev == e.next do
                break

            if .slopesEqual(e.prev.curr, e.curr, e.next.curr) and
               (!.preserveCollinear or !pt2IsBetweenPt1AndPt3(e.prev.curr, e.curr, e.next.curr))
            {
                if e == eStart do
                    eStart = e.next
                e         = removeEdge(e)
                e         = e.prev
                eLoopStop = e
                continue
            }

            e = e.next
            if e == eLoopStop do
                break
        }

        if e.prev == e.next do
            return

        // 3. Do second stage of edge initialization ...
        e = eStart
        for
        {
            initEdge2(e, polyType)
            e = e.next
            if isFlat and e.curr.y != eStart.curr.y do
                isFlat = false
            if e == eStart do
                break
        }

        // 4. Finally, add edge bounds to LocalMinima list ...
        if isFlat do
            return

        if e.prev.bot == e.prev.top do
            e = e.next

        var leftBoundIsForward: bool
        var eMin:               *Edge
        for
        {
            e = findNextLocMin(e)
            if e == eMin do
                break
            elif !eMin do
                eMin = e

            var locMin: LocalMinimum
            locMin.y = e.bot.y
            if e.dx < e.prev.dx
            {
                locMin.leftBound   = e.prev
                locMin.rightBound  = e
                leftBoundIsForward = false
            }
            else
            {
                locMin.leftBound   = e
                locMin.rightBound  = e.prev
                leftBoundIsForward = true
            }

            if locMin.leftBound.next == locMin.rightBound do
                locMin.leftBound.windDelta = -1
            else do
                locMin.leftBound.windDelta = 1
            locMin.rightBound.windDelta = -locMin.leftBound.windDelta

            e = .processBound(locMin.leftBound, leftBoundIsForward)
            if e.outIdx == Skip do
                e = .processBound(e, leftBoundIsForward)

            var e2 = .processBound(locMin.rightBound, !leftBoundIsForward)
            if e2.outIdx == Skip do
                e2 = .processBound(e2, !leftBoundIsForward)

            if locMin.leftBound.outIdx == Skip do
                locMin.leftBound = null
            elif locMin.rightBound.outIdx == Skip do
                locMin.rightBound = null
            .minimaList.add(locMin)
            if !leftBoundIsForward do
                e = e2
        }
    }

    func initEdge(e, eNext, ePrev: *Edge, pt: IntPoint)
    {
        e.next   = eNext
        e.prev   = ePrev
        e.curr   = pt
        e.outIdx = Unassigned
    }

    func initEdge2(e: *Edge, pt: PolyType)
    {
        if e.curr.y >= e.next.curr.y
        {
            e.bot = e.curr
            e.top = e.next.curr
        }
        else
        {
            e.top = e.curr
            e.bot = e.next.curr
        }

        setDx(e)
        e.polyType = pt
    }

    func setDx(e: *Edge)
    {
        let dy = e.top.y - e.bot.y
        if dy == 0 do
            e.dx = HorizontalVal
        else do
            e.dx = cast(f64) (e.top.x - e.bot.x) / dy
    }

    func removeEdge(e: *Edge)->*Edge
    {
        e.prev.next = e.next
        e.next.prev = e.prev
        e.prev      = null
        return e.next
    }

    mtd addJoin(op1, op2: *OutPt, offPt: IntPoint)
    {
        let j = Memory.new'Join()
        j.outPt1 = op1
        j.outPt2 = op2
        j.offPt  = offPt
        .joinList.add(j)
    }

    #[Swag.Overload]
    mtd slopesEqual(e1: *Edge, e2: *Edge)->bool
    {
        if .useFullRange
        {
            let v1 = Math.Int128.mul(e1.top.y - e1.bot.y, e2.top.x - e2.bot.x)
            let v2 = Math.Int128.mul(e1.top.x - e1.bot.x, e2.top.y - e2.bot.y)
            return v1 == v2
        }

        return (e1.top.y - e1.bot.y) * (e2.top.x - e2.bot.x) == (e1.top.x - e1.bot.x) * (e2.top.y - e2.bot.y)
    }

    #[Swag.Overload]
    mtd slopesEqual(pt1, pt2, pt3: IntPoint)->bool
    {
        if .useFullRange
        {
            let v1 = Math.Int128.mul(pt1.y - pt2.y, pt2.x - pt3.x)
            let v2 = Math.Int128.mul(pt1.x - pt2.x, pt2.y - pt3.y)
            return v1 == v2
        }

        return (pt1.y - pt2.y) * (pt2.x - pt3.x) == (pt1.x - pt2.x) * (pt2.y - pt3.y)
    }

    #[Swag.Overload]
    mtd slopesEqual(pt1, pt2, pt3, pt4: IntPoint)->bool
    {
        if .useFullRange
        {
            let v1 = Math.Int128.mul(pt1.y - pt2.y, pt3.x - pt4.x)
            let v2 = Math.Int128.mul(pt1.x - pt2.x, pt3.y - pt4.y)
            return v1 == v2
        }

        return (pt1.y - pt2.y) * (pt3.x - pt4.x) == (pt1.x - pt2.x) * (pt3.y - pt4.y)
    }

    func pt2IsBetweenPt1AndPt3(pt1, pt2, pt3: IntPoint)->bool
    {
        if (pt1 == pt3) or (pt1 == pt2) or (pt3 == pt2) do
            return false
        if pt1.x != pt3.x do
            return (pt2.x > pt1.x) == (pt2.x < pt3.x)
        return (pt2.y > pt1.y) == (pt2.y < pt3.y)
    }

    mtd rangeTest(pt: IntPoint)
    {
        if pt.x > LoRange or pt.y > LoRange or -pt.x > LoRange or -pt.y > LoRange
        {
            .useFullRange = true
            if pt.x > HiRange or pt.y > HiRange or -pt.x > HiRange or -pt.y > HiRange do
                Debug.assert(false, "Coordinate outside allowed range")
        }
    }

    #[Swag.Inline]
    func isHorizontal(e: *Edge)->bool
    {
        return e.dx == HorizontalVal
    }

    #[Swag.Inline]
    func reverseHorizontal(e: *Edge)
    {
        swap(&e.top.x, &e.bot.x)
    }

    func findNextLocMin(edge: *Edge)->*Edge
    {
        var e = edge
        for
        {
            while e.bot != e.prev.bot or e.curr == e.top do
                e = e.next
            if !isHorizontal(e) and !isHorizontal(e.prev) do
                break
            while isHorizontal(e.prev) do
                e = e.prev
            let e2 = e
            while isHorizontal(e) do
                e = e.next
            if e.top.y == e.prev.bot.y do
                continue
            if e2.prev.bot.x < e.bot.x do
                e = e2
            break
        }

        return e
    }

    mtd processBound(edge: *Edge, nextIsForward: bool)->*Edge
    {
        var e      = edge
        var result = e

        if e.outIdx == Skip
        {
            if nextIsForward
            {
                while e.top.y == e.next.bot.y do
                    e = e.next
                while e != result and isHorizontal(e) do
                    e = e.prev
            }
            else
            {
                while e.top.y == e.prev.bot.y do
                    e = e.prev
                while e != result and isHorizontal(e) do
                    e = e.next
            }

            if e == result
            {
                if nextIsForward do
                    result = e.next
                else do
                    result = e.prev
            }
            else
            {
                if nextIsForward do
                    e = result.next
                else do
                    e = result.prev

                let locMin = LocalMinimum{y: e.bot.y, rightBound: e}
                e.windDelta = 0
                result      = .processBound(e, nextIsForward)
                .minimaList.add(locMin)
            }

            return result
        }

        var eStart: *Edge
        if isHorizontal(e)
        {
            if nextIsForward do
                eStart = e.prev
            else do
                eStart = e.next
            if isHorizontal(eStart)
            {
                if eStart.bot.x != e.bot.x and eStart.top.x != e.bot.x do
                    reverseHorizontal(e)
            }
            elif eStart.bot.x != e.bot.x do
                reverseHorizontal(e)
        }

        eStart = e
        if nextIsForward
        {
            while result.top.y == result.next.bot.y and result.next.outIdx != Skip do
                result = result.next
            if isHorizontal(result) and result.next.outIdx != Skip
            {
                var horz = result
                while isHorizontal(horz.prev) do
                    horz = horz.prev
                if horz.prev.top.x > result.next.top.x do
                    result = horz.prev
            }

            while e != result
            {
                e.nextInLML = e.next
                if isHorizontal(e) and e != eStart and e.bot.x != e.prev.top.x do
                    reverseHorizontal(e)
                e = e.next
            }

            if isHorizontal(e) and e != eStart and e.bot.x != e.prev.top.x do
                reverseHorizontal(e)
            result = result.next
        }
        else
        {
            while result.top.y == result.prev.bot.y and result.prev.outIdx != Skip do
                result = result.prev
            if isHorizontal(result) and result.prev.outIdx != Skip
            {
                var horz = result
                while isHorizontal(horz.next) do
                    horz = horz.next
                if horz.next.top.x == result.prev.top.x or horz.next.top.x > result.prev.top.x do
                    result = horz.next
            }

            while e != result
            {
                e.nextInLML = e.prev
                if isHorizontal(e) and e != eStart and e.bot.x != e.next.top.x do
                    reverseHorizontal(e)
                e = e.prev
            }

            if isHorizontal(e) and e != eStart and e.bot.x != e.next.top.x do
                reverseHorizontal(e)
            result = result.prev
        }

        return result
    }

    mtd popScanbeam(res: *CInt)->bool
    {
        if !.scanBeamList.count do
            return false

        let y = .scanBeamList.popBack()
        dref res = y
        while .scanBeamList.count and y == .scanBeamList.back() do
            .scanBeamList.count -= 1

        return true
    }

    mtd insertScanbeam(y: CInt)
    {
        .scanBeamList.add(y)
        .scanBeamList.sort()
    }

    mtd reset()
    {
        .currentLM = 0
        if .currentLM == .minimaList.count do
            return
        .minimaList.sort()

        .scanBeamList.clear()
        foreach lm in .minimaList
        {
            .insertScanbeam(lm.y)
            var e = lm.leftBound
            if e
            {
                e.curr   = e.bot
                e.side   = .Left
                e.outIdx = Unassigned
            }

            e = lm.rightBound
            if e
            {
                e.curr   = e.bot
                e.side   = .Right
                e.outIdx = Unassigned
            }
        }

        .activeEdges = null
        .currentLM   = 0
    }

    #[Swag.Inline]
    mtd localMinimaPending()->bool
    {
        return .currentLM != .minimaList.count
    }

    mtd deleteFromSEL(e: *Edge)
    {
        let selPrev = e.prevInSEL
        let selNext = e.nextInSEL
        if !selPrev and !selNext and e != .sortedEdges do
            return
        if selPrev do
            selPrev.nextInSEL = selNext
        else do
            .sortedEdges = selNext
        if selNext do
            selNext.prevInSEL = selPrev
        e.nextInSEL, e.prevInSEL = null
    }

    mtd popEdgeFromSEL(edge: **Edge)->bool
    {
        if !.sortedEdges do
            return false
        dref edge = .sortedEdges
        .deleteFromSEL(.sortedEdges)
        return true
    }

    mtd processHorizontals()
    {
        var horzEdge: *Edge
        while .popEdgeFromSEL(&horzEdge) do
            .processHorizontal(horzEdge)
    }

    func getHorzDirection(horzEdge: *Edge, dir: *Direction, left, right: *CInt)
    {
        if horzEdge.bot.x < horzEdge.top.x
        {
            dref left  = horzEdge.bot.x
            dref right = horzEdge.top.x
            dref dir   = .LeftToRight
        }
        else
        {
            dref left  = horzEdge.top.x
            dref right = horzEdge.bot.x
            dref dir   = .RightToLeft
        }
    }

    func getMaximaPair(e: *Edge)->#null *Edge
    {
        if (e.next.top == e.top) and !e.next.nextInLML do
            return e.next
        elif (e.prev.top == e.top) and !e.prev.nextInLML do
            return e.prev
        return null
    }

    func getNextInAEL(e: *Edge, dir: Direction)->*Edge
    {
        return dir == .LeftToRight ? e.nextInAEL : e.prevInAEL
    }

    mtd getLastOutPt(e: *Edge)->*OutPt
    {
        let outRec = .polyOuts[e.outIdx]
        if e.side == .Left do
            return outRec.pts
        else do
            return outRec.pts.prev
    }

    mtd addGhostJoin(op: *OutPt, offPt: IntPoint)
    {
        let j = Memory.new'Join()
        j.outPt1 = op
        j.outPt2 = null
        j.offPt  = offPt
        .ghostJoinList.add(j)
    }

    mtd processHorizontal(hE: *Edge)
    {
        var horzEdge = hE

        var dir: Direction = undefined
        var horzLeft, horzRight: CInt

        let isOpen = (horzEdge.windDelta == 0)
        getHorzDirection(horzEdge, &dir, &horzLeft, &horzRight)

        var eLastHorz = horzEdge
        while eLastHorz.nextInLML and isHorizontal(eLastHorz.nextInLML) do
            eLastHorz = eLastHorz.nextInLML

        var eMaxPair: *Edge
        if !eLastHorz.nextInLML do
            eMaxPair = getMaximaPair(eLastHorz)

        var maxIt, maxRit: u64
        if .maximaList.count > 0
        {
            if dir == .LeftToRight
            {
                maxIt = 0
                while maxIt != .maximaList.count and .maximaList.buffer[maxIt] <= horzEdge.bot.x do
                    maxIt += 1
                if maxIt != .maximaList.count and .maximaList.buffer[maxIt] >= eLastHorz.top.x do
                    maxIt = .maximaList.count
            }
            else
            {
                maxRit = .maximaList.count - #wrap 1
                while maxRit != Swag.U64.Max and .maximaList.buffer[maxRit] > horzEdge.bot.x do
                    maxRit = maxRit - #wrap 1
                if maxRit != Swag.U64.Max and .maximaList.buffer[maxRit] <= eLastHorz.top.x do
                    maxRit = Swag.U64.Max
            }
        }

        var op1: *OutPt
        for
        {
            let isLastHorz = (horzEdge == eLastHorz)
            var e          = getNextInAEL(horzEdge, dir)
            while e
            {
                if .maximaList.count > 0
                {
                    if dir == .LeftToRight
                    {
                        while maxIt != .maximaList.count and .maximaList.buffer[maxIt] < e.curr.x
                        {
                            if horzEdge.outIdx >= 0 and !isOpen do
                                .addOutPt(horzEdge, IntPoint{.maximaList.buffer[maxIt], horzEdge.bot.y})
                            maxIt += 1
                        }
                    }
                    else
                    {
                        while maxRit != Swag.U64.Max and .maximaList.buffer[maxRit] > e.curr.x
                        {
                            if horzEdge.outIdx >= 0 and !isOpen do
                                .addOutPt(horzEdge, IntPoint{.maximaList.buffer[maxRit], horzEdge.bot.y})
                            maxRit = maxRit - #wrap 1
                        }
                    }
                }

                if (dir == .LeftToRight and e.curr.x > horzRight) or (dir == .RightToLeft and e.curr.x < horzLeft) do
                    break
                if e.curr.x == horzEdge.top.x and horzEdge.nextInLML and e.dx < horzEdge.nextInLML.dx do
                    break

                if horzEdge.outIdx >= 0 and !isOpen
                {
                    op1 = .addOutPt(horzEdge, e.curr)
                    var eNextHorz = .sortedEdges
                    while eNextHorz
                    {
                        if eNextHorz.outIdx >= 0 and
                           horzSegmentsOverlap(horzEdge.bot.x, horzEdge.top.x, eNextHorz.bot.x, eNextHorz.top.x)
                        {
                            let op2 = .getLastOutPt(eNextHorz)
                            .addJoin(op2, op1, eNextHorz.top)
                        }

                        eNextHorz = eNextHorz.nextInSEL
                    }

                    .addGhostJoin(op1, horzEdge.bot)
                }

                if e == eMaxPair and isLastHorz and eMaxPair
                {
                    if horzEdge.outIdx >= 0 do
                        .addLocalMaxPoly(horzEdge, eMaxPair, horzEdge.top)
                    .deleteFromAEL(horzEdge)
                    .deleteFromAEL(eMaxPair)
                    return
                }

                if dir == .LeftToRight
                {
                    let pt = IntPoint{e.curr.x, horzEdge.curr.y}
                    .intersectEdges(horzEdge, e, pt)
                }
                else
                {
                    let pt = IntPoint{e.curr.x, horzEdge.curr.y}
                    .intersectEdges(e, horzEdge, pt)
                }

                let eNext = getNextInAEL(e, dir)
                .swapPositionsInAEL(horzEdge, e)
                e = eNext
            }

            if !horzEdge.nextInLML or !isHorizontal(horzEdge.nextInLML) do
                break
            .updateEdgeIntoAEL(&horzEdge)
            if horzEdge.outIdx >= 0 do
                .addOutPt(horzEdge, horzEdge.bot)
            getHorzDirection(horzEdge, &dir, &horzLeft, &horzRight)
        }

        if horzEdge.outIdx >= 0 and !op1
        {
            op1 = .getLastOutPt(horzEdge)
            var eNextHorz = .sortedEdges
            while eNextHorz
            {
                if eNextHorz.outIdx >= 0 and
                   horzSegmentsOverlap(horzEdge.bot.x, horzEdge.top.x, eNextHorz.bot.x, eNextHorz.top.x)
                {
                    let op2 = .getLastOutPt(eNextHorz)
                    .addJoin(op2, op1, eNextHorz.top)
                }

                eNextHorz = eNextHorz.nextInSEL
            }

            .addGhostJoin(op1, horzEdge.top)
        }

        if horzEdge.nextInLML
        {
            if horzEdge.outIdx >= 0
            {
                op1 = .addOutPt(horzEdge, horzEdge.top)
                .updateEdgeIntoAEL(&horzEdge)
                if horzEdge.windDelta == 0 do
                    return
                let ePrev = horzEdge.prevInAEL
                let eNext = horzEdge.nextInAEL
                if ePrev and ePrev.curr.x == horzEdge.bot.x and
                   ePrev.curr.y == horzEdge.bot.y and ePrev.windDelta != 0 and
                   ePrev.outIdx >= 0 and ePrev.curr.y > ePrev.top.y and .slopesEqual(horzEdge, ePrev)
                {
                    let op2 = .addOutPt(ePrev, horzEdge.bot)
                    .addJoin(op1, op2, horzEdge.top)
                }
                elif eNext and eNext.curr.x == horzEdge.bot.x and
                     eNext.curr.y == horzEdge.bot.y and eNext.windDelta != 0 and
                     eNext.outIdx >= 0 and eNext.curr.y > eNext.top.y and .slopesEqual(horzEdge, eNext)
                {
                    let op2 = .addOutPt(eNext, horzEdge.bot)
                    .addJoin(op1, op2, horzEdge.top)
                }
            }
            else do
                .updateEdgeIntoAEL(&horzEdge)
        }
        else
        {
            if horzEdge.outIdx >= 0 do
                .addOutPt(horzEdge, horzEdge.top)
            .deleteFromAEL(horzEdge)
        }
    }

    mtd updateEdgeIntoAEL(ed: **Edge)
    {
        var e = dref ed
        e.nextInLML.outIdx = e.outIdx
        let aelPrev = e.prevInAEL
        let aelNext = e.nextInAEL
        if aelPrev do
            aelPrev.nextInAEL = e.nextInLML
        else do
            .activeEdges = e.nextInLML
        if aelNext do
            aelNext.prevInAEL = e.nextInLML
        e.nextInLML.side      = e.side
        e.nextInLML.windDelta = e.windDelta
        e.nextInLML.windCnt   = e.windCnt
        e.nextInLML.windCnt2  = e.windCnt2
        e                     = e.nextInLML
        e.curr                = e.bot
        e.prevInAEL           = aelPrev
        e.nextInAEL           = aelNext
        if !isHorizontal(e) do
            .insertScanbeam(e.top.y)
        dref ed = e
    }

    mtd swapPositionsInAEL(edge1, edge2: *Edge)
    {
        if edge1.nextInAEL == edge1.prevInAEL or edge2.nextInAEL == edge2.prevInAEL do
            return

        if edge1.nextInAEL == edge2
        {
            let next = edge2.nextInAEL
            if next do
                next.prevInAEL = edge1
            let prev = edge1.prevInAEL
            if prev do
                prev.nextInAEL = edge2
            edge2.prevInAEL = prev
            edge2.nextInAEL = edge1
            edge1.prevInAEL = edge2
            edge1.nextInAEL = next
        }
        elif edge2.nextInAEL == edge1
        {
            let next = edge1.nextInAEL
            if next do
                next.prevInAEL = edge2
            let prev = edge2.prevInAEL
            if prev do
                prev.nextInAEL = edge1
            edge1.prevInAEL = prev
            edge1.nextInAEL = edge2
            edge2.prevInAEL = edge1
            edge2.nextInAEL = next
        }
        else
        {
            let next = edge1.nextInAEL
            let prev = edge1.prevInAEL
            edge1.nextInAEL = edge2.nextInAEL
            if edge1.nextInAEL do
                edge1.nextInAEL.prevInAEL = edge1
            edge1.prevInAEL = edge2.prevInAEL
            if edge1.prevInAEL do
                edge1.prevInAEL.nextInAEL = edge1
            edge2.nextInAEL = next
            if edge2.nextInAEL do
                edge2.nextInAEL.prevInAEL = edge2
            edge2.prevInAEL = prev
            if edge2.prevInAEL do
                edge2.prevInAEL.nextInAEL = edge2
        }

        if !edge1.prevInAEL do
            .activeEdges = edge1
        elif !edge2.prevInAEL do
            .activeEdges = edge2
    }

    mtd deleteFromAEL(e: *Edge)
    {
        let aelPrev = e.prevInAEL
        let aelNext = e.nextInAEL
        if !aelPrev and !aelNext and e != .activeEdges do
            return
        if aelPrev do
            aelPrev.nextInAEL = aelNext
        else do
            .activeEdges = aelNext
        if aelNext do
            aelNext.prevInAEL = aelPrev
        e.nextInAEL = null
        e.prevInAEL = null
    }

    mtd clearJoins()
    {
        foreach p in .joinList do
            Memory.delete(p)
        .joinList.clear()
    }

    mtd clearGhostJoins()
    {
        foreach p in .ghostJoinList do
            Memory.delete(p)
        .ghostJoinList.clear()
    }

    mtd executeInternal()
    {
        .reset()
        .maximaList.clear()
        .sortedEdges = null

        var botY, topY: CInt
        if !.popScanbeam(&botY) do
            return

        .insertLocalMinimaIntoAEL(botY)
        while .popScanbeam(&topY) or .localMinimaPending()
        {
            .processHorizontals()
            .clearGhostJoins()
            .processIntersections(topY)
            .processEdgesAtTopOfScanbeam(topY)
            botY = topY
            .insertLocalMinimaIntoAEL(botY)
        }

        // Fix orientations
        foreach outRec in .polyOuts
        {
            if !outRec.pts or outRec.isOpen do
                continue
            if (outRec.isHole ^ .reverseOutput) == (area(outRec.pts) > 0) do
                reversePolyPtLinks(outRec.pts)
        }

        if .joinList.count do
            .joinCommonEdges()

        foreach outRec in .polyOuts
        {
            if !outRec.pts do
                continue
            if outRec.isOpen do
                fixupOutPolyline(outRec)
            else do
                .fixupOutPolygon(outRec)
        }

        if .strictSimple do
            .doSimplePolygons()

        .clearJoins()
        .clearGhostJoins()
    }

    mtd doSimplePolygons()
    {
        foreach outrec in .polyOuts
        {
            var op = outrec.pts
            if !op or outrec.isOpen do
                continue

            for
            {
                var op2 = op.next
                while op2 != outrec.pts
                {
                    if op.pt == op2.pt and op2.next != op and op2.prev != op
                    {
                        let op3 = op.prev
                        let op4 = op2.prev
                        op.prev  = op4
                        op4.next = op
                        op2.prev = op3
                        op3.next = op2

                        outrec.pts = op
                        let outrec2 = .createOutRec()
                        outrec2.pts = op2
                        updateOutPtIdxs(outrec2)
                        if poly2ContainsPoly1(outrec2.pts, outrec.pts)
                        {
                            outrec2.isHole    = !outrec.isHole
                            outrec2.firstLeft = outrec
                            if .usingPolyTree do
                                .fixupFirstLefts2(outrec2, outrec)
                        }
                        elif poly2ContainsPoly1(outrec.pts, outrec2.pts)
                        {
                            outrec2.isHole    = outrec.isHole
                            outrec.isHole     = !outrec2.isHole
                            outrec2.firstLeft = outrec.firstLeft
                            outrec.firstLeft  = outrec2
                            if .usingPolyTree do
                                .fixupFirstLefts2(outrec, outrec2)
                        }
                        else
                        {
                            outrec2.isHole    = outrec.isHole
                            outrec2.firstLeft = outrec.firstLeft
                            if .usingPolyTree do
                                .fixupFirstLefts1(outrec, outrec2)
                        }

                        op2 = op
                    }

                    op2 = op2.next
                }

                op = op.next
                if op == outrec.pts do
                    break
            }
        }
    }

    func fixupOutPolyline(outrec: *OutRec)
    {
        var pp     = outrec.pts
        var lastPP = pp.prev
        while pp != lastPP
        {
            pp = pp.next
            if pp.pt == pp.prev.pt
            {
                if pp == lastPP do
                    lastPP = pp.prev
                let tmpPP = pp.prev
                tmpPP.next   = pp.next
                pp.next.prev = tmpPP
                Memory.delete(pp)
                pp = tmpPP
            }
        }

        if pp == pp.prev
        {
            disposeOutPts(pp)
            outrec.pts = null
        }
    }

    mtd fixupOutPolygon(outrec: *OutRec)
    {
        outrec.bottomPt = null
        var pp          = outrec.pts
        let preserveCol = .preserveCollinear or .strictSimple

        var lastOK: *OutPt
        for
        {
            if pp.prev == pp or pp.prev == pp.next
            {
                disposeOutPts(pp)
                outrec.pts = null
                return
            }

            if (pp.pt == pp.next.pt) or (pp.pt == pp.prev.pt) or
               (.slopesEqual(pp.prev.pt, pp.pt, pp.next.pt) and
               (!preserveCol or !pt2IsBetweenPt1AndPt3(pp.prev.pt, pp.pt, pp.next.pt)))
            {
                lastOK = null
                let tmp = pp
                pp.prev.next = pp.next
                pp.next.prev = pp.prev
                pp           = pp.prev
                Memory.delete(tmp)
            }
            elif pp == lastOK do
                break
            else
            {
                if !lastOK do
                    lastOK = pp
                pp = pp.next
            }
        }

        outrec.pts = pp
    }

    mtd getOutRec(idx: s32)->*OutRec
    {
        var outrec = .polyOuts[idx]
        while outrec != .polyOuts[outrec.idx] do
            outrec = .polyOuts[outrec.idx]
        return outrec
    }

    mtd joinPoints(j: *Join, outRec1, outRec2: *OutRec)->bool
    {
        var op1 = j.outPt1
        var op2 = j.outPt2

        let isHorizontal = (j.outPt1.pt.y == j.offPt.y)

        var op1b, op2b: *OutPt
        if isHorizontal and (j.offPt == j.outPt1.pt) and (j.offPt == j.outPt2.pt)
        {
            if outRec1 != outRec2 do
                return false
            op1b = j.outPt1.next
            while op1b != op1 and (op1b.pt == j.offPt) do
                op1b = op1b.next
            let reverse1 = (op1b.pt.y > j.offPt.y)
            op2b = j.outPt2.next
            while op2b != op2 and (op2b.pt == j.offPt) do
                op2b = op2b.next
            let reverse2 = (op2b.pt.y > j.offPt.y)
            if reverse1 == reverse2 do
                return false
            if reverse1
            {
                op1b      = dupOutPt(op1, false)
                op2b      = dupOutPt(op2, true)
                op1.prev  = op2
                op2.next  = op1
                op1b.next = op2b
                op2b.prev = op1b
                j.outPt1  = op1
                j.outPt2  = op1b
                return true
            }

            op1b      = dupOutPt(op1, true)
            op2b      = dupOutPt(op2, false)
            op1.next  = op2
            op2.prev  = op1
            op1b.prev = op2b
            op2b.next = op1b
            j.outPt1  = op1
            j.outPt2  = op1b
            return true
        }

        if isHorizontal
        {
            op1b = op1
            while op1.prev.pt.y == op1.pt.y and op1.prev != op1b and op1.prev != op2 do
                op1 = op1.prev
            while op1b.next.pt.y == op1b.pt.y and op1b.next != op1 and op1b.next != op2 do
                op1b = op1b.next
            if op1b.next == op1 or op1b.next == op2 do
                return false

            op2b = op2
            while op2.prev.pt.y == op2.pt.y and op2.prev != op2b and op2.prev != op1b do
                op2 = op2.prev
            while op2b.next.pt.y == op2b.pt.y and op2b.next != op2 and op2b.next != op1 do
                op2b = op2b.next
            if op2b.next == op2 or op2b.next == op1 do
                return false

            var left, right: CInt
            if !getOverlap(op1.pt.x, op1b.pt.x, op2.pt.x, op2b.pt.x, &left, &right) do
                return false

            var pt:              IntPoint
            var discardLeftSide: bool
            if op1.pt.x >= left and op1.pt.x <= right
            {
                pt              = op1.pt
                discardLeftSide = (op1.pt.x > op1b.pt.x)
            }
            elif op2.pt.x >= left and op2.pt.x <= right
            {
                pt              = op2.pt
                discardLeftSide = (op2.pt.x > op2b.pt.x)
            }
            elif op1b.pt.x >= left and op1b.pt.x <= right
            {
                pt              = op1b.pt
                discardLeftSide = op1b.pt.x > op1.pt.x
            }
            else
            {
                pt              = op2b.pt
                discardLeftSide = (op2b.pt.x > op2.pt.x)
            }

            j.outPt1 = op1
            j.outPt2 = op2
            return joinHorz(op1, op1b, op2, op2b, pt, discardLeftSide)
        }

        op1b = op1.next
        while (op1b.pt == op1.pt) and (op1b != op1) do
            op1b = op1b.next
        let reverse1 = ((op1b.pt.y > op1.pt.y) or !.slopesEqual(op1.pt, op1b.pt, j.offPt))
        if reverse1
        {
            op1b = op1.prev
            while (op1b.pt == op1.pt) and (op1b != op1) do
                op1b = op1b.prev
            if (op1b.pt.y > op1.pt.y) or !.slopesEqual(op1.pt, op1b.pt, j.offPt) do
                return false
        }

        op2b = op2.next
        while (op2b.pt == op2.pt) and (op2b != op2) do
            op2b = op2b.next
        let reverse2 = ((op2b.pt.y > op2.pt.y) or !.slopesEqual(op2.pt, op2b.pt, j.offPt))
        if reverse2
        {
            op2b = op2.prev
            while (op2b.pt == op2.pt) and (op2b != op2) do
                op2b = op2b.prev
            if (op2b.pt.y > op2.pt.y) or !.slopesEqual(op2.pt, op2b.pt, j.offPt) do
                return false
        }

        if (op1b == op1) or (op2b == op2) or (op1b == op2b) or ((outRec1 == outRec2) and (reverse1 == reverse2)) do
            return false

        if reverse1
        {
            op1b      = dupOutPt(op1, false)
            op2b      = dupOutPt(op2, true)
            op1.prev  = op2
            op2.next  = op1
            op1b.next = op2b
            op2b.prev = op1b
            j.outPt1  = op1
            j.outPt2  = op1b
            return true
        }

        op1b      = dupOutPt(op1, true)
        op2b      = dupOutPt(op2, false)
        op1.next  = op2
        op2.prev  = op1
        op1b.prev = op2b
        op2b.next = op1b
        j.outPt1  = op1
        j.outPt2  = op1b
        return true
    }

    func joinHorz(op1P, op1bP, op2P, op2bP: *OutPt, pt: IntPoint, discardLeft: bool)->bool
    {
        var op1  = op1P
        var op1b = op1bP
        var op2  = op2P
        var op2b = op2bP

        let dir1 = op1.pt.x > op1b.pt.x ? Direction.RightToLeft : Direction.LeftToRight
        let dir2 = op2.pt.x > op2b.pt.x ? Direction.RightToLeft : Direction.LeftToRight
        if dir1 == dir2 do
            return false

        if dir1 == .LeftToRight
        {
            while op1.next.pt.x <= pt.x and op1.next.pt.x >= op1.pt.x and op1.next.pt.y == pt.y do
                op1 = op1.next
            if discardLeft and (op1.pt.x != pt.x) do
                op1 = op1.next
            op1b = dupOutPt(op1, !discardLeft)
            if op1b.pt != pt
            {
                op1    = op1b
                op1.pt = pt
                op1b   = dupOutPt(op1, !discardLeft)
            }
        }
        else
        {
            while op1.next.pt.x >= pt.x and op1.next.pt.x <= op1.pt.x and op1.next.pt.y == pt.y do
                op1 = op1.next
            if !discardLeft and (op1.pt.x != pt.x) do
                op1 = op1.next
            op1b = dupOutPt(op1, discardLeft)
            if op1b.pt != pt
            {
                op1    = op1b
                op1.pt = pt
                op1b   = dupOutPt(op1, discardLeft)
            }
        }

        if dir2 == .LeftToRight
        {
            while op2.next.pt.x <= pt.x and op2.next.pt.x >= op2.pt.x and op2.next.pt.y == pt.y do
                op2 = op2.next
            if discardLeft and (op2.pt.x != pt.x) do
                op2 = op2.next
            op2b = dupOutPt(op2, !discardLeft)
            if op2b.pt != pt
            {
                op2    = op2b
                op2.pt = pt
                op2b   = dupOutPt(op2, !discardLeft)
            }
        }
        else
        {
            while op2.next.pt.x >= pt.x and op2.next.pt.x <= op2.pt.x and op2.next.pt.y == pt.y do
                op2 = op2.next
            if !discardLeft and (op2.pt.x != pt.x) do
                op2 = op2.next
            op2b = dupOutPt(op2, discardLeft)
            if op2b.pt != pt
            {
                op2    = op2b
                op2.pt = pt
                op2b   = dupOutPt(op2, discardLeft)
            }
        }

        if dir1 == .LeftToRight and discardLeft
        {
            op1.prev  = op2
            op2.next  = op1
            op1b.next = op2b
            op2b.prev = op1b
        }
        else
        {
            op1.next  = op2
            op2.prev  = op1
            op1b.prev = op2b
            op2b.next = op1b
        }

        return true
    }

    func getOverlap(a1, a2, b1, b2: CInt, left, right: *CInt)->bool
    {
        if a1 < a2
        {
            if b1 < b2
            {
                dref left  = Math.max(a1, b1)
                dref right = Math.min(a2, b2)
            }
            else
            {
                dref left  = Math.max(a1, b2)
                dref right = Math.min(a2, b1)
            }
        }
        else
        {
            if b1 < b2
            {
                dref left  = Math.max(a2, b1)
                dref right = Math.min(a1, b2)
            }
            else
            {
                dref left  = Math.max(a2, b2)
                dref right = Math.min(a1, b1)
            }
        }

        return left < right
    }

    func dupOutPt(outPt: *OutPt, insertAfter: bool)->*OutPt
    {
        let result = Memory.new'OutPt()
        result.pt  = outPt.pt
        result.idx = outPt.idx
        if insertAfter
        {
            result.next     = outPt.next
            result.prev     = outPt
            outPt.next.prev = result
            outPt.next      = result
        }
        else
        {
            result.prev     = outPt.prev
            result.next     = outPt
            outPt.prev.next = result
            outPt.prev      = result
        }

        return result
    }

    func updateOutPtIdxs(outrec: *OutRec)
    {
        var op = outrec.pts
        for
        {
            op.idx = outrec.idx
            op     = op.prev
            if op == outrec.pts do
                break
        }
    }

    func pointInPolygon(pt: IntPoint, outPoints: *OutPt)->s32
    {
        var op      = outPoints
        var result  = 0
        let startOp = op
        for
        {
            if op.next.pt.y == pt.y
            {
                if (op.next.pt.x == pt.x) or (op.pt.y == pt.y and ((op.next.pt.x > pt.x) == (op.pt.x < pt.x))) do
                    return -1
            }

            if (op.pt.y < pt.y) != (op.next.pt.y < pt.y)
            {
                if op.pt.x >= pt.x
                {
                    if op.next.pt.x > pt.x do
                        result = 1 - result
                    else
                    {
                        let d = cast(f64) (op.pt.x - pt.x) * (op.next.pt.y - pt.y) - cast(f64) (op.next.pt.x - pt.x) * (op.pt.y - pt.y)
                        if d == 0 do
                            return -1
                        if (d > 0) == (op.next.pt.y > op.pt.y) do
                            result = 1 - result
                    }
                }
                else
                {
                    if op.next.pt.x > pt.x
                    {
                        let d = cast(f64) (op.pt.x - pt.x) * (op.next.pt.y - pt.y) - cast(f64) (op.next.pt.x - pt.x) * (op.pt.y - pt.y)
                        if d == 0 do
                            return -1
                        if (d > 0) == (op.next.pt.y > op.pt.y) do
                            result = 1 - result
                    }
                }
            }

            op = op.next
            if startOp == op do
                break
        }

        return result
    }

    func poly2ContainsPoly1(outPt1, outPt2: *OutPt)->bool
    {
        var op = outPt1
        for
        {
            let res = pointInPolygon(op.pt, outPt2)
            if res >= 0 do
                return res > 0
            op = op.next
            if op == outPt1 do
                break
        }

        return true
    }

    func parseFirstLeft(fl: *OutRec)->#null *OutRec
    {
        var firstLeft = fl
        while firstLeft and !firstLeft.pts do
            firstLeft = firstLeft.firstLeft
        return firstLeft
    }

    mtd fixupFirstLefts2(innerOutRec, outerOutRec: *OutRec)
    {
        let orfl = outerOutRec.firstLeft
        foreach outRec in .polyOuts
        {
            if !outRec.pts or outRec == outerOutRec or outRec == innerOutRec do
                continue
            let firstLeft = parseFirstLeft(outRec.firstLeft)
            if firstLeft != orfl and firstLeft != innerOutRec and firstLeft != outerOutRec do
                continue
            if poly2ContainsPoly1(outRec.pts, innerOutRec.pts) do
                outRec.firstLeft = innerOutRec
            elif poly2ContainsPoly1(outRec.pts, outerOutRec.pts) do
                outRec.firstLeft = outerOutRec
            elif outRec.firstLeft == innerOutRec or outRec.firstLeft == outerOutRec do
                outRec.firstLeft = orfl
        }
    }

    mtd fixupFirstLefts1(oldOutRec, newOutRec: *OutRec)
    {
        foreach outRec in .polyOuts
        {
            let firstLeft = parseFirstLeft(outRec.firstLeft)
            if outRec.pts and firstLeft == oldOutRec
            {
                if poly2ContainsPoly1(outRec.pts, newOutRec.pts) do
                    outRec.firstLeft = newOutRec
            }
        }
    }

    mtd fixupFirstLefts3(oldOutRec, newOutRec: *OutRec)
    {
        foreach outRec in .polyOuts
        {
            let firstLeft = parseFirstLeft(outRec.firstLeft)
            if outRec.pts and firstLeft == oldOutRec do
                outRec.firstLeft = newOutRec
        }
    }

    mtd joinCommonEdges()
    {
        foreach join in .joinList
        {
            let outRec1 = .getOutRec(join.outPt1.idx)
            var outRec2 = .getOutRec(join.outPt2.idx)

            if !outRec1.pts or !outRec2.pts do
                continue
            if outRec1.isOpen or outRec2.isOpen do
                continue

            var holeStateRec: *OutRec
            if outRec1 == outRec2 do
                holeStateRec = outRec1
            elif outRec1RightOfOutRec2(outRec1, outRec2) do
                holeStateRec = outRec2
            elif outRec1RightOfOutRec2(outRec2, outRec1) do
                holeStateRec = outRec1
            else do
                holeStateRec = getLowermostRec(outRec1, outRec2)

            if !.joinPoints(join, outRec1, outRec2) do
                continue

            if (outRec1 == outRec2)
            {
                outRec1.pts      = join.outPt1
                outRec1.bottomPt = null
                outRec2          = .createOutRec()
                outRec2.pts      = join.outPt2
                updateOutPtIdxs(outRec2)

                if poly2ContainsPoly1(outRec2.pts, outRec1.pts)
                {
                    outRec2.isHole    = !outRec1.isHole
                    outRec2.firstLeft = outRec1
                    if .usingPolyTree do
                        .fixupFirstLefts2(outRec2, outRec1)
                    if (outRec2.isHole ^ .reverseOutput) == (area(outRec2.pts) > 0) do
                        reversePolyPtLinks(outRec2.pts)
                }
                elif poly2ContainsPoly1(outRec1.pts, outRec2.pts)
                {
                    outRec2.isHole    = outRec1.isHole
                    outRec1.isHole    = !outRec2.isHole
                    outRec2.firstLeft = outRec1.firstLeft
                    outRec1.firstLeft = outRec2
                    if .usingPolyTree do
                        .fixupFirstLefts2(outRec1, outRec2)
                    if (outRec1.isHole ^ .reverseOutput) == (area(outRec1.pts) > 0) do
                        reversePolyPtLinks(outRec1.pts)
                }
                else
                {
                    outRec2.isHole    = outRec1.isHole
                    outRec2.firstLeft = outRec1.firstLeft
                    if .usingPolyTree do
                        .fixupFirstLefts1(outRec1, outRec2)
                }
            }
            else
            {
                outRec2.pts      = null
                outRec2.bottomPt = null
                outRec2.idx      = outRec1.idx

                outRec1.isHole = holeStateRec.isHole
                if holeStateRec == outRec2 do
                    outRec1.firstLeft = outRec2.firstLeft
                outRec2.firstLeft = outRec1

                if .usingPolyTree do
                    .fixupFirstLefts3(outRec2, outRec1)
            }
        }
    }

    #[Swag.Inline]
    func isMaxima(e: *Edge, y: CInt)->bool
    {
        return e and e.top.y == y and !e.nextInLML
    }

    func getMaximaPairEx(e: *Edge)->#null *Edge
    {
        let result = getMaximaPair(e)
        if result and (result.outIdx == Skip or
           (result.nextInAEL == result.prevInAEL and !isHorizontal(result))) do
            return null
        return result
    }

    #[Swag.Inline]
    func isIntermediate(e: *Edge, y: CInt)->bool
    {
        return e.top.y == y and e.nextInLML
    }

    mtd doMaxima(e: *Edge)
    {
        let eMaxPair = getMaximaPairEx(e)
        if !eMaxPair
        {
            if e.outIdx >= 0 do
                .addOutPt(e, e.top)
            .deleteFromAEL(e)
            return
        }

        var eNext = e.nextInAEL
        while eNext and eNext != eMaxPair
        {
            .intersectEdges(e, eNext, e.top)
            .swapPositionsInAEL(e, eNext)
            eNext = e.nextInAEL
        }

        if e.outIdx == Unassigned and eMaxPair.outIdx == Unassigned
        {
            .deleteFromAEL(e)
            .deleteFromAEL(eMaxPair)
        }
        elif e.outIdx >= 0 and eMaxPair.outIdx >= 0
        {
            if e.outIdx >= 0 do
                .addLocalMaxPoly(e, eMaxPair, e.top)
            .deleteFromAEL(e)
            .deleteFromAEL(eMaxPair)
        }
        else
        {
            Debug.assert(false)
        }
    }

    mtd processEdgesAtTopOfScanbeam(topY: CInt)
    {
        var e = .activeEdges
        while e
        {
            // 1. process maxima, treating them as if they're 'bent' horizontal edges,
            //    but exclude maxima with horizontal edges. nb: e can't be a horizontal.
            var isMaximaEdge = isMaxima(e, topY)
            if isMaximaEdge
            {
                let eMaxPair = getMaximaPairEx(e)
                isMaximaEdge = !eMaxPair or !isHorizontal(eMaxPair)
            }

            if isMaximaEdge
            {
                if .strictSimple do
                    .maximaList.add(e.top.x)
                let ePrev = e.prevInAEL
                .doMaxima(e)
                if !ePrev do
                    e = .activeEdges
                else do
                    e = ePrev.nextInAEL
            }
            else
            {
                // 2. promote horizontal edges, otherwise update curr.x and curr.y ...
                if isIntermediate(e, topY) and isHorizontal(e.nextInLML)
                {
                    .updateEdgeIntoAEL(&e)
                    if e.outIdx >= 0 do
                        .addOutPt(e, e.bot)
                    .addEdgeToSEL(e)
                }
                else
                {
                    e.curr.x = topX(e, topY)
                    e.curr.y = topY
                }

                // When StrictlySimple and 'e' is being touched by another edge, then
                // make sure both edges have a vertex here ...
                if .strictSimple
                {
                    let ePrev = e.prevInAEL
                    if (e.outIdx >= 0) and (e.windDelta != 0) and ePrev and (ePrev.outIdx >= 0) and
                       (ePrev.curr.x == e.curr.x) and (ePrev.windDelta != 0)
                    {
                        let pt  = e.curr
                        let op  = .addOutPt(ePrev, pt)
                        let op2 = .addOutPt(e, pt)
                        .addJoin(op, op2, pt)
                    }
                }

                e = e.nextInAEL
            }
        }

        // 3. Process horizontals at the top of the scanbeam ...
        .maximaList.sort()
        .processHorizontals()
        .maximaList.clear()

        // 4. Promote intermediate vertices ...
        e = .activeEdges
        while e
        {
            if isIntermediate(e, topY)
            {
                var op: *OutPt
                if e.outIdx >= 0 do
                    op = .addOutPt(e, e.top)
                .updateEdgeIntoAEL(&e)

                let ePrev = e.prevInAEL
                let eNext = e.nextInAEL
                if ePrev and ePrev.curr.x == e.bot.x and
                   ePrev.curr.y == e.bot.y and op and
                   ePrev.outIdx >= 0 and ePrev.curr.y > ePrev.top.y and
                   .slopesEqual(e.curr, e.top, ePrev.curr, ePrev.top) and
                   (e.windDelta != 0) and (ePrev.windDelta != 0)
                {
                    let op2 = .addOutPt(ePrev, e.bot)
                    .addJoin(op, op2, e.top)
                }
                elif eNext and eNext.curr.x == e.bot.x and
                     eNext.curr.y == e.bot.y and op and
                     eNext.outIdx >= 0 and eNext.curr.y > eNext.top.y and
                     .slopesEqual(e.curr, e.top, eNext.curr, eNext.top) and
                     (e.windDelta != 0) and (eNext.windDelta != 0)
                {
                    let op2 = .addOutPt(eNext, e.bot)
                    .addJoin(op, op2, e.top)
                }
            }

            e = e.nextInAEL
        }
    }

    mtd processIntersections(topY: CInt)
    {
        if !.activeEdges do
            return

        .buildIntersectList(topY)
        let ilSize = .intersectList.count
        if ilSize == 0 do
            return
        if ilSize == 1 or .fixupIntersectionOrder() do
            .processIntersectList()
        .sortedEdges = null
    }

    mtd buildIntersectList(topY: CInt)
    {
        if !.activeEdges do
            return

        var e = .activeEdges
        .sortedEdges = e
        while e
        {
            e.prevInSEL = e.prevInAEL
            e.nextInSEL = e.nextInAEL
            e.curr.x    = topX(e, topY)
            e           = e.nextInAEL
        }

        var isModified = true
        while isModified
        {
            isModified = false
            e          = .sortedEdges
            while e.nextInSEL
            {
                let eNext = e.nextInSEL
                if e.curr.x > eNext.curr.x
                {
                    var pt: IntPoint
                    intersectPoint(e, eNext, &pt)
                    if pt.y < topY do
                        pt = IntPoint{topX(e, topY), topY}
                    let newNode = Memory.new'IntersectNode()
                    newNode.edge1 = e
                    newNode.edge2 = eNext
                    newNode.pt    = pt
                    .intersectList.add(newNode)
                    .swapPositionsInSEL(e, eNext)
                    isModified = true
                }
                else do
                    e = eNext
            }

            if !e.prevInSEL do
                break
            e.prevInSEL.nextInSEL = null
        }

        .sortedEdges = null
    }

    mtd copyAELToSEL()
    {
        var e = .activeEdges
        .sortedEdges = e
        while e
        {
            e.prevInSEL = e.prevInAEL
            e.nextInSEL = e.nextInAEL
            e           = e.nextInAEL
        }
    }

    func edgesAdjacent(inode: *IntersectNode)->bool
    {
        return (inode.edge1.nextInSEL == inode.edge2) or (inode.edge1.prevInSEL == inode.edge2)
    }

    mtd fixupIntersectionOrder()->bool
    {
        .copyAELToSEL()
        .intersectList.sort()
        for i in .intersectList
        {
            if !edgesAdjacent(.intersectList[i])
            {
                var j = i + 1
                while j < .intersectList.count and !edgesAdjacent(.intersectList[j]) do
                    j += 1
                if j == .intersectList.count do
                    return false
                swap(.intersectList.buffer + i, .intersectList.buffer + j)
            }

            .swapPositionsInSEL(.intersectList[i].edge1, .intersectList[i].edge2)
        }

        return true
    }

    mtd processIntersectList()
    {
        foreach iNode in .intersectList
        {
            .intersectEdges(iNode.edge1, iNode.edge2, iNode.pt)
            .swapPositionsInAEL(iNode.edge1, iNode.edge2)
            Memory.delete(iNode)
        }

        .intersectList.clear()
    }

    mtd swapPositionsInSEL(edge1, edge2: *Edge)
    {
        if !edge1.nextInSEL and !edge1.prevInSEL do
            return
        if !edge2.nextInSEL and !edge2.prevInSEL do
            return

        if edge1.nextInSEL == edge2
        {
            let next = edge2.nextInSEL
            if next do
                next.prevInSEL = edge1
            let prev = edge1.prevInSEL
            if prev do
                prev.nextInSEL = edge2
            edge2.prevInSEL = prev
            edge2.nextInSEL = edge1
            edge1.prevInSEL = edge2
            edge1.nextInSEL = next
        }
        elif edge2.nextInSEL == edge1
        {
            let next = edge1.nextInSEL
            if next do
                next.prevInSEL = edge2
            let prev = edge2.prevInSEL
            if prev do
                prev.nextInSEL = edge1
            edge1.prevInSEL = prev
            edge1.nextInSEL = edge2
            edge2.prevInSEL = edge1
            edge2.nextInSEL = next
        }
        else
        {
            let next = edge1.nextInSEL
            let prev = edge1.prevInSEL
            edge1.nextInSEL = edge2.nextInSEL
            if edge1.nextInSEL do
                edge1.nextInSEL.prevInSEL = edge1
            edge1.prevInSEL = edge2.prevInSEL
            if edge1.prevInSEL do
                edge1.prevInSEL.nextInSEL = edge1
            edge2.nextInSEL = next
            if edge2.nextInSEL do
                edge2.nextInSEL.prevInSEL = edge2
            edge2.prevInSEL = prev
            if edge2.prevInSEL do
                edge2.prevInSEL.nextInSEL = edge2
        }

        if !edge1.prevInSEL do
            .sortedEdges = edge1
        elif !edge2.prevInSEL do
            .sortedEdges = edge2
    }

    func intersectPoint(edge1, edge2: *Edge, ip: *IntPoint)
    {
        var b1, b2: f64
        if edge1.dx == edge2.dx
        {
            ip.y = edge1.curr.y
            ip.x = topX(edge1, ip.y)
            return
        }

        if edge1.dx == 0
        {
            ip.x = edge1.bot.x
            if isHorizontal(edge2) do
                ip.y = edge2.bot.y
            else
            {
                b2   = edge2.bot.y - (edge2.bot.x / edge2.dx)
                ip.y = cround(ip.x / edge2.dx + b2)
            }
        }
        elif edge2.dx == 0
        {
            ip.x = edge2.bot.x
            if isHorizontal(edge1) do
                ip.y = edge1.bot.y
            else
            {
                b1   = edge1.bot.y - (edge1.bot.x / edge1.dx)
                ip.y = cround(ip.x / edge1.dx + b1)
            }
        }
        else
        {
            b1 = edge1.bot.x - edge1.bot.y * edge1.dx
            b2 = edge2.bot.x - edge2.bot.y * edge2.dx
            let q = (b2 - b1) / (edge1.dx - edge2.dx)
            ip.y = cround(q)
            if Math.abs(edge1.dx) < Math.abs(edge2.dx) do
                ip.x = cround(edge1.dx * q + b1)
            else do
                ip.x = cround(edge2.dx * q + b2)
        }

        if ip.y < edge1.top.y or ip.y < edge2.top.y
        {
            if edge1.top.y > edge2.top.y do
                ip.y = edge1.top.y
            else do
                ip.y = edge2.top.y
            if Math.abs(edge1.dx) < Math.abs(edge2.dx) do
                ip.x = topX(edge1, ip.y)
            else do
                ip.x = topX(edge2, ip.y)
        }

        if (ip.y > edge1.curr.y)
        {
            ip.y = edge1.curr.y
            if Math.abs(edge1.dx) > Math.abs(edge2.dx) do
                ip.x = topX(edge2, ip.y)
            else do
                ip.x = topX(edge1, ip.y)
        }
    }

    mtd popLocalMinima(y: CInt, locMin: *const *LocalMinimum)->bool
    {
        if .currentLM == .minimaList.count or .minimaList[.currentLM].y != y do
            return false
        dref locMin = .minimaList[.currentLM]
        .currentLM += 1
        return true
    }

    #[Swag.Inline]
    func cround(val: f64)->CInt
    {
        return cast(CInt) Math.round(val)
    }

    func topX(edge: *Edge, currentY: CInt)->CInt
    {
        return currentY == edge.top.y ? edge.top.x : edge.bot.x + cround(edge.dx * (currentY - edge.bot.y))
    }

    func e2InsertsBeforeE1(e1, e2: *Edge)->bool
    {
        if e2.curr.x == e1.curr.x
        {
            if e2.top.y > e1.top.y do
                return e2.top.x < topX(e1, e2.top.y)
            else do
                return e1.top.x > topX(e2, e1.top.y)
        }
        else do
            return e2.curr.x < e1.curr.x
    }

    mtd insertEdgeIntoAEL(edge: *Edge, startE: #null *Edge)
    {
        var startEdge = startE
        if !.activeEdges
        {
            edge.prevInAEL = null
            edge.nextInAEL = null
            .activeEdges   = edge
        }
        elif !startEdge and e2InsertsBeforeE1(.activeEdges, edge)
        {
            edge.prevInAEL         = null
            edge.nextInAEL         = .activeEdges
            .activeEdges.prevInAEL = edge
            .activeEdges           = edge
        }
        else
        {
            if !startEdge do
                startEdge = .activeEdges
            while startEdge.nextInAEL and !e2InsertsBeforeE1(startEdge.nextInAEL, edge) do
                startEdge = startEdge.nextInAEL
            edge.nextInAEL = startEdge.nextInAEL
            if startEdge.nextInAEL do
                startEdge.nextInAEL.prevInAEL = edge
            edge.prevInAEL      = startEdge
            startEdge.nextInAEL = edge
        }
    }

    mtd isEvenOddFillType(edge: *Edge)->bool
    {
        if edge.polyType == .Subject do
            return .subjFillType == .EvenOdd
        return .clipFillType == .EvenOdd
    }

    mtd isEvenOddAltFillType(edge: *Edge)->bool
    {
        if edge.polyType == .Subject do
            return .clipFillType == .EvenOdd
        return .subjFillType == .EvenOdd
    }

    mtd setWindingCount(edge: *Edge)
    {
        var e = edge.prevInAEL

        while e and ((e.polyType != edge.polyType) or (e.windDelta == 0)) do
            e = e.prevInAEL

        if !e
        {
            if edge.windDelta == 0
            {
                let pft = edge.polyType == .Subject ? .subjFillType : .clipFillType
                edge.windCnt = (pft == .Negative ? -1 : 1)
            }
            else do
                edge.windCnt = edge.windDelta
            edge.windCnt2 = 0
            e             = .activeEdges
        }
        elif edge.windDelta == 0 and .clipType != .Union
        {
            edge.windCnt  = 1
            edge.windCnt2 = e.windCnt2
            e             = e.nextInAEL
        }
        elif .isEvenOddFillType(edge)
        {
            if edge.windDelta == 0
            {
                var inside = true
                var e2     = e.prevInAEL
                while e2
                {
                    if e2.polyType == e.polyType and e2.windDelta != 0 do
                        inside = !inside
                    e2 = e2.prevInAEL
                }
                edge.windCnt = inside ? 0 : 1
            }
            else do
                edge.windCnt = edge.windDelta
            edge.windCnt2 = e.windCnt2
            e             = e.nextInAEL
        }
        else
        {
            if e.windCnt * e.windDelta < 0
            {
                if Math.abs(e.windCnt) > 1
                {
                    if e.windDelta * edge.windDelta < 0 do
                        edge.windCnt = e.windCnt
                    else do
                        edge.windCnt = e.windCnt + edge.windDelta
                }
                else do
                    edge.windCnt = edge.windDelta == 0 ? 1 : edge.windDelta
            }
            else
            {
                if edge.windDelta == 0 do
                    edge.windCnt = (e.windCnt < 0 ? e.windCnt - 1 : e.windCnt + 1)
                elif e.windDelta * edge.windDelta < 0 do
                    edge.windCnt = e.windCnt
                else do
                    edge.windCnt = e.windCnt + edge.windDelta
            }

            edge.windCnt2 = e.windCnt2
            e             = e.nextInAEL
        }

        if .isEvenOddAltFillType(edge)
        {
            while e != edge
            {
                if e.windDelta != 0 do
                    edge.windCnt2 = edge.windCnt2 == 0 ? 1 : 0
                e = e.nextInAEL
            }
        }
        else
        {
            while e != edge
            {
                edge.windCnt2 += e.windDelta
                e = e.nextInAEL
            }
        }
    }

    mtd isContributing(edge: *Edge)->bool
    {
        var pft, pft2: PolyFillType = undefined
        if edge.polyType == .Subject
        {
            pft  = .subjFillType
            pft2 = .clipFillType
        }
        else
        {
            pft  = .clipFillType
            pft2 = .subjFillType
        }

        switch pft
        {
        case EvenOdd:
            if edge.windDelta == 0 and edge.windCnt != 1 do
                return false
        case NonZero:
            if Math.abs(edge.windCnt) != 1 do
                return false
        case Positive:
            if edge.windCnt != 1 do
                return false
        default:
            if edge.windCnt != -1 do
                return false
        }

        switch .clipType
        {
        case Intersection:
            switch pft2
            {
            case EvenOdd, NonZero:
                return edge.windCnt2 != 0
            case Positive:
                return edge.windCnt2 > 0
            default:
                return edge.windCnt2 < 0
            }

        case Union:
            switch pft2
            {
            case EvenOdd, NonZero:
                return edge.windCnt2 == 0
            case Positive:
                return edge.windCnt2 <= 0
            default:
                return edge.windCnt2 >= 0
            }

        case Difference:
            if edge.polyType == .Subject
            {
                switch pft2
                {
                case EvenOdd, NonZero:
                    return edge.windCnt2 == 0
                case Positive:
                    return edge.windCnt2 <= 0
                default:
                    return edge.windCnt2 >= 0
                }
            }
            else
            {
                switch pft2
                {
                case EvenOdd, NonZero:
                    return edge.windCnt2 != 0
                case Positive:
                    return edge.windCnt2 > 0
                default:
                    return edge.windCnt2 < 0
                }
            }

        case Xor:
            if edge.windDelta == 0
            {
                switch pft2
                {
                case EvenOdd, NonZero:
                    return edge.windCnt2 == 0
                case Positive:
                    return edge.windCnt2 <= 0
                default:
                    return edge.windCnt2 >= 0
                }
            }
        }

        return true
    }

    mtd createOutRec()->*OutRec
    {
        let result = Memory.new'OutRec()
        .polyOuts.add(result)
        result.idx = cast(s32) (.polyOuts.count - 1)
        return result
    }

    mtd setHoleState(e: *Edge, outrec: *OutRec)
    {
        var e2    = e.prevInAEL
        var eTmp: *Edge
        while e2
        {
            if e2.outIdx >= 0 and e2.windDelta != 0
            {
                if !eTmp do
                    eTmp = e2
                elif eTmp.outIdx == e2.outIdx do
                    eTmp = null
            }

            e2 = e2.prevInAEL
        }
        if !eTmp
        {
            outrec.firstLeft = null
            outrec.isHole    = false
        }
        else
        {
            outrec.firstLeft = .polyOuts[eTmp.outIdx]
            outrec.isHole    = !outrec.firstLeft.isHole
        }
    }

    #[Swag.Discardable]
    mtd addOutPt(e: *Edge, pt: IntPoint)->*OutPt
    {
        if e.outIdx < 0
        {
            let outRec = .createOutRec()
            outRec.isOpen = e.windDelta == 0
            let newOp = Memory.new'OutPt()
            outRec.pts = newOp
            newOp.idx  = outRec.idx
            newOp.pt   = pt
            newOp.next = newOp
            newOp.prev = newOp
            if !outRec.isOpen do
                .setHoleState(e, outRec)
            e.outIdx = outRec.idx
            return newOp
        }
        else
        {
            let outRec = .polyOuts[e.outIdx]
            let op     = outRec.pts

            let toFront = (e.side == .Left)
            if toFront and (pt == op.pt) do
                return op
            if !toFront and (pt == op.prev.pt) do
                return op.prev

            let newOp = Memory.new'OutPt()
            newOp.idx       = outRec.idx
            newOp.pt        = pt
            newOp.next      = op
            newOp.prev      = op.prev
            newOp.prev.next = newOp
            op.prev         = newOp
            if toFront do
                outRec.pts = newOp
            return newOp
        }
    }

    #[Swag.Discardable]
    mtd addLocalMinPoly(e1, e2: *Edge, pt: IntPoint)->*OutPt
    {
        var result: *OutPt
        var e, prevE: *Edge

        if isHorizontal(e2) or (e1.dx > e2.dx)
        {
            result    = .addOutPt(e1, pt)
            e2.outIdx = e1.outIdx
            e1.side   = .Left
            e2.side   = .Right
            e         = e1
            if e.prevInAEL == e2 do
                prevE = e2.prevInAEL
            else do
                prevE = e.prevInAEL
        }
        else
        {
            result    = .addOutPt(e2, pt)
            e1.outIdx = e2.outIdx
            e1.side   = .Right
            e2.side   = .Left
            e         = e2
            if e.prevInAEL == e1 do
                prevE = e1.prevInAEL
            else do
                prevE = e.prevInAEL
        }

        if prevE and prevE.outIdx >= 0 and prevE.top.y < pt.y and e.top.y < pt.y
        {
            let xPrev = topX(prevE, pt.y)
            let xE    = topX(e, pt.y)
            if xPrev == xE and
               (e.windDelta != 0) and
               (prevE.windDelta != 0) and
               .slopesEqual(IntPoint{xPrev, pt.y}, prevE.top, IntPoint{xE, pt.y}, e.top)
            {
                let outPt = .addOutPt(prevE, pt)
                .addJoin(result, outPt, e.top)
            }
        }

        return result
    }

    mtd addEdgeToSEL(edge: *Edge)
    {
        if !.sortedEdges
        {
            .sortedEdges   = edge
            edge.prevInSEL = null
            edge.nextInSEL = null
        }
        else
        {
            edge.nextInSEL         = .sortedEdges
            edge.prevInSEL         = null
            .sortedEdges.prevInSEL = edge
            .sortedEdges           = edge
        }
    }

    func horzSegmentsOverlap(s1a, s1b, s2a, s2b: CInt)->bool
    {
        var seg1a = s1a
        var seg1b = s1b
        var seg2a = s2a
        var seg2b = s2b
        if seg1a > seg1b do
            swap(&seg1a, &seg1b)
        if seg2a > seg2b do
            swap(&seg2a, &seg2b)
        return (seg1a < seg2b) and (seg2a < seg1b)
    }

    func outRec1RightOfOutRec2(o1, outRec2: *OutRec)->bool
    {
        var outRec1 = o1
        while outRec1
        {
            outRec1 = outRec1.firstLeft
            if outRec1 == outRec2 do
                return true
        }

        return false
    }

    func getDx(pt1, pt2: IntPoint)->f64
    {
        return (pt1.y == pt2.y) ? cast(f64) HorizontalVal : cast(f64) (pt2.x - pt1.x) / (pt2.y - pt1.y)
    }

    func area(op1: *OutPt)->f64
    {
        var op      = op1
        let startOp = op
        if !op do
            return 0

        var a = 0'f64
        for
        {
            a += cast(f64) (op.prev.pt.x + op.pt.x) * cast(f64) (op.prev.pt.y - op.pt.y)
            op = op.next
            if op == startOp do
                break
        }

        return a * 0.5
    }

    func firstIsBottomPt(btmPt1, btmPt2: *OutPt)->bool
    {
        var p = btmPt1.prev
        while (p.pt == btmPt1.pt) and (p != btmPt1) do
            p = p.prev
        let dx1p = Math.abs(getDx(btmPt1.pt, p.pt))
        p = btmPt1.next
        while (p.pt == btmPt1.pt) and (p != btmPt1) do
            p = p.next
        let dx1n = Math.abs(getDx(btmPt1.pt, p.pt))

        p = btmPt2.prev
        while (p.pt == btmPt2.pt) and (p != btmPt2) do
            p = p.prev
        let dx2p = Math.abs(getDx(btmPt2.pt, p.pt))
        p = btmPt2.next
        while (p.pt == btmPt2.pt) and (p != btmPt2) do
            p = p.next
        let dx2n = Math.abs(getDx(btmPt2.pt, p.pt))

        if Math.max(dx1p, dx1n) == Math.max(dx2p, dx2n) and
           Math.min(dx1p, dx1n) == Math.min(dx2p, dx2n) do
            return area(btmPt1) > 0
        return (dx1p >= dx2p and dx1p >= dx2n) or (dx1n >= dx2p and dx1n >= dx2n)
    }

    func getBottomPt(pp1: *OutPt)->*OutPt
    {
        var dups: *OutPt
        var pp    = pp1
        var p     = pp.next
        while p != pp
        {
            if p.pt.y > pp.pt.y
            {
                pp   = p
                dups = null
            }
            elif p.pt.y == pp.pt.y and p.pt.x <= pp.pt.x
            {
                if p.pt.x < pp.pt.x
                {
                    dups = null
                    pp   = p
                }
                else
                {
                    if p.next != pp and p.prev != pp do
                        dups = p
                }
            }

            p = p.next
        }

        if dups
        {
            while dups != p
            {
                if !firstIsBottomPt(p, dups) do
                    pp = dups
                dups = dups.next
                while dups.pt != pp.pt do
                    dups = dups.next
            }
        }

        return pp
    }

    func getLowermostRec(outRec1, outRec2: *OutRec)->*OutRec
    {
        if !outRec1.bottomPt do
            outRec1.bottomPt = getBottomPt(outRec1.pts)
        if !outRec2.bottomPt do
            outRec2.bottomPt = getBottomPt(outRec2.pts)
        let outPt1 = outRec1.bottomPt
        let outPt2 = outRec2.bottomPt
        if outPt1.pt.y > outPt2.pt.y do
            return outRec1
        if outPt1.pt.y < outPt2.pt.y do
            return outRec2
        if outPt1.pt.x < outPt2.pt.x do
            return outRec1
        if outPt1.pt.x > outPt2.pt.x do
            return outRec2
        if outPt1.next == outPt1 do
            return outRec2
        if outPt2.next == outPt2 do
            return outRec1
        if firstIsBottomPt(outPt1, outPt2) do
            return outRec1
        return outRec2
    }

    func reversePolyPtLinks(pp: *OutPt)
    {
        if !pp do
            return

        var pp1 = pp
        for
        {
            let pp2 = pp1.next
            pp1.next = pp1.prev
            pp1.prev = pp2
            pp1      = pp2
            if pp1 == pp do
                break
        }
    }

    mtd appendPolygon(e1, e2: *Edge)
    {
        let outRec1 = .polyOuts[e1.outIdx]
        let outRec2 = .polyOuts[e2.outIdx]

        var holeStateRec: *OutRec
        if outRec1RightOfOutRec2(outRec1, outRec2) do
            holeStateRec = outRec2
        elif outRec1RightOfOutRec2(outRec2, outRec1) do
            holeStateRec = outRec1
        else do
            holeStateRec = getLowermostRec(outRec1, outRec2)

        let p1_lft = outRec1.pts
        let p1_rt  = p1_lft.prev
        let p2_lft = outRec2.pts
        let p2_rt  = p2_lft.prev

        if e1.side == .Left
        {
            if e2.side == .Left
            {
                reversePolyPtLinks(p2_lft)
                p2_lft.next = p1_lft
                p1_lft.prev = p2_lft
                p1_rt.next  = p2_rt
                p2_rt.prev  = p1_rt
                outRec1.pts = p2_rt
            }
            else
            {
                p2_rt.next  = p1_lft
                p1_lft.prev = p2_rt
                p2_lft.prev = p1_rt
                p1_rt.next  = p2_lft
                outRec1.pts = p2_lft
            }
        }
        else
        {
            if e2.side == .Right
            {
                reversePolyPtLinks(p2_lft)
                p1_rt.next  = p2_rt
                p2_rt.prev  = p1_rt
                p2_lft.next = p1_lft
                p1_lft.prev = p2_lft
            }
            else
            {
                p1_rt.next  = p2_lft
                p2_lft.prev = p1_rt
                p1_lft.prev = p2_rt
                p2_rt.next  = p1_lft
            }
        }

        outRec1.bottomPt = null
        if holeStateRec == outRec2
        {
            if outRec2.firstLeft != outRec1 do
                outRec1.firstLeft = outRec2.firstLeft
            outRec1.isHole = outRec2.isHole
        }

        outRec2.pts       = null
        outRec2.bottomPt  = null
        outRec2.firstLeft = outRec1

        let okIdx       = e1.outIdx
        let obsoleteIdx = e2.outIdx

        e1.outIdx = Unassigned
        e2.outIdx = Unassigned

        var e = .activeEdges
        while e
        {
            if e.outIdx == obsoleteIdx
            {
                e.outIdx = okIdx
                e.side   = e1.side
                break
            }

            e = e.nextInAEL
        }

        outRec2.idx = outRec1.idx
    }

    mtd addLocalMaxPoly(e1, e2: *Edge, pt: IntPoint)
    {
        .addOutPt(e1, pt)
        if e2.windDelta == 0 do
            .addOutPt(e2, pt)

        if e1.outIdx == e2.outIdx
        {
            e1.outIdx = Unassigned
            e2.outIdx = Unassigned
        }
        elif e1.outIdx < e2.outIdx do
            .appendPolygon(e1, e2)
        else do
            .appendPolygon(e2, e1)
    }

    func swapSides(edge1, edge2: *Edge)
    {
        let side = edge1.side
        edge1.side = edge2.side
        edge2.side = side
    }

    func swapPolyIndexes(edge1, edge2: *Edge)
    {
        let outIdx = edge1.outIdx
        edge1.outIdx = edge2.outIdx
        edge2.outIdx = outIdx
    }

    mtd intersectEdges(e1, e2: *Edge, pt: IntPoint)
    {
        let e1Contributing = e1.outIdx >= 0
        let e2Contributing = e2.outIdx >= 0

        if e1.polyType == e2.polyType
        {
            if .isEvenOddFillType(e1)
            {
                let oldE1WindCnt = e1.windCnt
                e1.windCnt = e2.windCnt
                e2.windCnt = oldE1WindCnt
            }
            else
            {
                if e1.windCnt + e2.windDelta == 0 do
                    e1.windCnt = -e1.windCnt
                else do
                    e1.windCnt += e2.windDelta
                if e2.windCnt - e1.windDelta == 0 do
                    e2.windCnt = -e2.windCnt
                else do
                    e2.windCnt -= e1.windDelta
            }
        }
        else
        {
            if !.isEvenOddFillType(e2) do
                e1.windCnt2 += e2.windDelta
            else do
                e1.windCnt2 = (e1.windCnt2 == 0) ? 1 : 0
            if !.isEvenOddFillType(e1) do
                e2.windCnt2 -= e1.windDelta
            else do
                e2.windCnt2 = (e2.windCnt2 == 0) ? 1 : 0
        }

        var e1FillType, e2FillType, e1FillType2, e2FillType2: PolyFillType = undefined
        if e1.polyType == .Subject
        {
            e1FillType  = .subjFillType
            e1FillType2 = .clipFillType
        }
        else
        {
            e1FillType  = .clipFillType
            e1FillType2 = .subjFillType
        }

        if e2.polyType == .Subject
        {
            e2FillType  = .subjFillType
            e2FillType2 = .clipFillType
        }
        else
        {
            e2FillType  = .clipFillType
            e2FillType2 = .subjFillType
        }

        var e1Wc, e2Wc: CInt
        switch e1FillType
        {
        case Positive:
            e1Wc = e1.windCnt
        case Negative:
            e1Wc = -e1.windCnt
        default:
            e1Wc = Math.abs(e1.windCnt)
        }

        switch e2FillType
        {
        case Positive:
            e2Wc = e2.windCnt
        case Negative:
            e2Wc = -e2.windCnt
        default:
            e2Wc = Math.abs(e2.windCnt)
        }

        if e1Contributing and e2Contributing
        {
            if (e1Wc != 0 and e1Wc != 1) or (e2Wc != 0 and e2Wc != 1) or (e1.polyType != e2.polyType and .clipType != .Xor)
            {
                .addLocalMaxPoly(e1, e2, pt)
            }
            else
            {
                .addOutPt(e1, pt)
                .addOutPt(e2, pt)
                swapSides(e1, e2)
                swapPolyIndexes(e1, e2)
            }
        }
        elif e1Contributing
        {
            if e2Wc == 0 or e2Wc == 1
            {
                .addOutPt(e1, pt)
                swapSides(e1, e2)
                swapPolyIndexes(e1, e2)
            }
        }
        elif e2Contributing
        {
            if e1Wc == 0 or e1Wc == 1
            {
                .addOutPt(e2, pt)
                swapSides(e1, e2)
                swapPolyIndexes(e1, e2)
            }
        }
        elif (e1Wc == 0 or e1Wc == 1) and (e2Wc == 0 or e2Wc == 1)
        {
            var e1Wc2, e2Wc2: CInt
            switch e1FillType2
            {
            case Positive:
                e1Wc2 = e1.windCnt2
            case Negative:
                e1Wc2 = -e1.windCnt2
            default:
                e1Wc2 = Math.abs(e1.windCnt2)
            }

            switch e2FillType2
            {
            case Positive:
                e2Wc2 = e2.windCnt2
            case Negative:
                e2Wc2 = -e2.windCnt2
            default:
                e2Wc2 = Math.abs(e2.windCnt2)
            }

            if e1.polyType != e2.polyType
            {
                .addLocalMinPoly(e1, e2, pt)
            }
            elif e1Wc == 1 and e2Wc == 1
            {
                switch .clipType
                {
                case Intersection:
                    if e1Wc2 > 0 and e2Wc2 > 0 do
                        .addLocalMinPoly(e1, e2, pt)
                case Union:
                    if e1Wc2 <= 0 and e2Wc2 <= 0 do
                        .addLocalMinPoly(e1, e2, pt)
                case Difference:
                    if ((e1.polyType == .Clip) and (e1Wc2 > 0) and (e2Wc2 > 0)) or
                       ((e1.polyType == .Subject) and (e1Wc2 <= 0) and (e2Wc2 <= 0)) do
                        .addLocalMinPoly(e1, e2, pt)
                case Xor:
                    .addLocalMinPoly(e1, e2, pt)
                }
            }
            else do
                swapSides(e1, e2)
        }
    }

    mtd insertLocalMinimaIntoAEL(botY: CInt)
    {
        var lm: const *LocalMinimum
        while .popLocalMinima(botY, &lm)
        {
            let lb = lm.leftBound
            let rb = lm.rightBound

            var op1: *OutPt
            if !lb
            {
                .insertEdgeIntoAEL(rb, null)
                .setWindingCount(rb)
                if .isContributing(rb) do
                    op1 = .addOutPt(rb, rb.bot)
            }
            elif !rb
            {
                .insertEdgeIntoAEL(lb, null)
                .setWindingCount(lb)
                if .isContributing(lb) do
                    op1 = .addOutPt(lb, lb.bot)
                .insertScanbeam(lb.top.y)
            }
            else
            {
                .insertEdgeIntoAEL(lb, null)
                .insertEdgeIntoAEL(rb, lb)
                .setWindingCount(lb)
                rb.windCnt  = lb.windCnt
                rb.windCnt2 = lb.windCnt2
                if .isContributing(lb) do
                    op1 = .addLocalMinPoly(lb, rb, lb.bot)
                .insertScanbeam(lb.top.y)
            }

            if rb
            {
                if isHorizontal(rb)
                {
                    .addEdgeToSEL(rb)
                    if rb.nextInLML do
                        .insertScanbeam(rb.nextInLML.top.y)
                }
                else do
                    .insertScanbeam(rb.top.y)
            }

            if !lb or !rb do
                continue

            //if any output polygons share an edge, they'll need joining later ...
            if op1 and isHorizontal(rb) and .ghostJoinList.count > 0 and (rb.windDelta != 0)
            {
                for i in .ghostJoinList.count
                {
                    let jr = .ghostJoinList[i]
                    if horzSegmentsOverlap(jr.outPt1.pt.x, jr.offPt.x, rb.bot.x, rb.top.x) do
                        .addJoin(jr.outPt1, op1, jr.offPt)
                }
            }

            if lb.outIdx >= 0 and lb.prevInAEL and
               lb.prevInAEL.curr.x == lb.bot.x and
               lb.prevInAEL.outIdx >= 0 and
               .slopesEqual(lb.prevInAEL.bot, lb.prevInAEL.top, lb.curr, lb.top) and
               (lb.windDelta != 0) and (lb.prevInAEL.windDelta != 0)
            {
                let op2 = .addOutPt(lb.prevInAEL, lb.bot)
                .addJoin(op1, op2, lb.top)
            }

            if lb.nextInAEL != rb
            {
                if rb.outIdx >= 0 and
                   rb.prevInAEL.outIdx >= 0 and
                   .slopesEqual(rb.prevInAEL.curr, rb.prevInAEL.top, rb.curr, rb.top) and
                   (rb.windDelta != 0) and
                   (rb.prevInAEL.windDelta != 0)
                {
                    let op2 = .addOutPt(rb.prevInAEL, rb.bot)
                    .addJoin(op1, op2, rb.top)
                }

                var e = lb.nextInAEL
                if e
                {
                    while e != rb
                    {
                        .intersectEdges(rb, e, lb.curr)
                        e = e.nextInAEL
                    }
                }
            }
        }
    }

    func pointCount(pts: *OutPt)->s32
    {
        if !pts do
            return 0

        var result = 0
        var p      = pts
        for
        {
            result += 1
            p = p.next
            if p == pts do
                break
        }

        return result
    }

    mtd buildResult(polys: *ClipperPaths)
    {
        polys.reserve(.polyOuts.count)
        for i in .polyOuts
        {
            if !.polyOuts[i].pts do
                continue

            var p   = .polyOuts[i].pts.prev
            let cnt = pointCount(p)
            if cnt < 2 do
                continue

            var pg: ClipperPath
            pg.reserve(cast(u64) cnt)
            for cnt
            {
                pg.add(p.pt)
                p = p.prev
            }

            polys.add(moveref &pg)
        }
    }

    mtd disposeAllOutRecs()
    {
        for i in .polyOuts do
            .disposeOutRec(i)
        .polyOuts.clear()
    }

    func disposeOutPts(pts: *OutPt)
    {
        var pp = pts
        if pp == null do
            return

        pp.prev.next = null
        while pp
        {
            let tmpPp = pp
            pp = pp.next
            Memory.delete(tmpPp)
        }
    }

    mtd disposeOutRec(index: u64)
    {
        let outRec = .polyOuts[index]
        if outRec.pts do
            disposeOutPts(outRec.pts)
        Memory.delete(outRec)
        .polyOuts[index] = null
    }

    mtd getBounds()->IntRect
    {
        var result: retval

        var lm = 0'u64
        if lm == .minimaList.count do
            return result

        result.left   = .minimaList[lm].leftBound.bot.x
        result.top    = .minimaList[lm].leftBound.bot.y
        result.right  = .minimaList[lm].leftBound.bot.x
        result.bottom = .minimaList[lm].leftBound.bot.y

        while lm != .minimaList.count
        {
            result.bottom = Math.max(result.bottom, .minimaList[lm].leftBound.bot.y)
            var e = .minimaList[lm].leftBound
            for
            {
                let bottomE = e
                while e.nextInLML
                {
                    if e.bot.x < result.left do
                        result.left = e.bot.x
                    if e.bot.x > result.right do
                        result.right = e.bot.x
                    e = e.nextInLML
                }

                result.left  = Math.min(result.left, e.bot.x)
                result.right = Math.max(result.right, e.bot.x)
                result.left  = Math.min(result.left, e.top.x)
                result.right = Math.max(result.right, e.top.x)
                result.top   = Math.min(result.top, e.top.y)
                if bottomE == .minimaList[lm].leftBound do
                    e = .minimaList[lm].rightBound
                else do
                    break
            }

            lm += 1
        }

        return result
    }
}
