// The original code has been modified for swag
// http://www.angusj.com

/*******************************************************************************
*                                                                              *
* Author    :  Angus Johnson                                                   *
* Version   :  6.4.2                                                           *
* Date      :  27 February 2017                                                *
* Website   :  http://www.angusj.com                                           *
* Copyright :  Angus Johnson 2010-2017                                         *
*                                                                              *
* License:                                                                     *
* Use, modification & distribution is subject to Boost Software License Ver 1. *
* http://www.boost.org/LICENSE_1_0.txt                                         *
*                                                                              *
* Attributions:                                                                *
* The code in this library is an extension of Bala Vatti's clipping algorithm: *
* "A generic solution to polygon clipping"                                     *
* Communications of the ACM, Vol 35, Issue 7 (July 1992) pp 56-63.             *
* http://portal.acm.org/citation.cfm?id=129906                                 *
*                                                                              *
* Computer graphics and geometric modeling: implementation and algorithms      *
* By Max K. Agoston                                                            *
* Springer; 1 edition (January 4, 2005)                                        *
* http://books.google.com/books?q=vatti+clipping+agoston                       *
*                                                                              *
* See also:                                                                    *
* "Polygon Offsetting by Computing Winding Numbers"                            *
* Paper no. DETC2005-85513 pp. 565-575                                         *
* ASME 2005 International Design Engineering Technical Conferences             *
* and Computers and Information in Engineering Conference (IDETC/CIE2005)      *
* September 24-28, 2005 , Long Beach, California, USA                          *
* http://www.me.berkeley.edu/~mcmains/pubs/DAC05OffsetPolygon.pdf              *
*                                                                              *
*******************************************************************************/
#global namespace Clipper
using Core

struct IntPoint
{
    x, y: CInt
}

impl IntPoint
{
    method opEquals(other: Self)->bool
    {
        return x == other.x and y == other.y
    }
}

struct IntRect
{
    left:   CInt
    top:    CInt
    right:  CInt
    bottom: CInt
}

const LoRange = 0x3FFFFFFF
const HiRange = 0x3FFFFFFFFFFFFFFF's64
const Unassigned = -1
const Skip = -2
const HorizontalVal = -1.0E+40

alias CInt         = s64
alias ClipperPath  = Array'IntPoint
alias ClipperPaths = Array'ClipperPath

public enum PolyType        { Subject; Clip; }
public enum ClipType        { Intersection; Union; Difference; Xor; }
public enum PolyFillType    { EvenOdd; NonZero; Positive; Negative; }
public enum InitOptions     { ReverseSolution = 1; StrictlySimple = 2; PreserveCollinear = 4; }
public enum JoinType        { Square; Round; Miter; }

enum EdgeSide  { Left = 1; Right = 2; }
enum Direction { RightToLeft; LeftToRight; }

struct IntersectNode
{
    edge1:    *Edge
    edge2:    *Edge
    pt:       IntPoint
}

impl IntersectNode
{
    method opCmp(other: *IntersectNode)->s32
    {
        return pt.y <=> other.pt.y
    }
}

struct LocalMinimum
{
    y:            CInt
    leftBound:    *Edge
    rightBound:   *Edge
}

impl LocalMinimum
{
    method opCmp(other: LocalMinimum)->s32
    {
        return other.y <=> y
    }
}

struct Edge
{
    bot:          IntPoint
    curr:         IntPoint
    top:          IntPoint
    dx:           f64
    polyType:     PolyType = ?
    side:         EdgeSide = ?
    windDelta:    s32
    windCnt:      s32
    windCnt2:     s32
    outIdx:       s32
    next:         *Edge
    prev:         *Edge
    nextInLML:    *Edge
    nextInAEL:    *Edge
    prevInAEL:    *Edge
    nextInSEL:    *Edge
    prevInSEL:    *Edge
}

struct OutRec
{
    idx:        s32
    isHole:     bool
    isOpen:     bool
    firstLeft:  *OutRec
    polyNd:     *PolyNode
    pts:        *OutPt
    bottomPt:   *OutPt
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
    index:      u32
    isOpen:     bool
    jointype:   JoinType = ?
    contour:    ClipperPath
    childs:     Array'*PolyNode
    parent:     *PolyNode
}

impl PolyNode
{
    method addChild(child: *PolyNode)
    {
        cnt := cast(u32) childs.count
        childs.add(child)
        child.parent = self
        child.index = cnt
    }
}

struct Join
{
    outPt1: *OutPt
    outPt2: *OutPt
    offPt:  IntPoint
}

#[Swag.Opaque]
public struct Transform
{
    useFullRange:           bool
    preserveCollinear:      bool
    usingPolyTree:          bool
    strictSimple:           bool
    reverseOutput:          bool
    edgeList:               Array'*Edge
    minimaList:             Array'LocalMinimum
    polyOuts:               Array'*OutRec
    joinList:               Array'*Join
    ghostJoinList:          Array'*Join
    intersectList:          Array'*IntersectNode
    scanBeamList:           Array'CInt
    maximaList:             Array'CInt
    clipFillType:           PolyFillType = ?
    subjFillType:           PolyFillType = ?
    clipType:               ClipType = ?
    currentLM:              uint
    activeEdges:            *Edge
    sortedEdges:            *Edge
}

impl Transform
{
    method execute(clipT: ClipType, solution: *ClipperPaths, subjFillT, clipFillT: PolyFillType)
    {
        subjFillType  = subjFillT
        clipFillType  = clipFillT
        clipType      = clipT
        usingPolyTree = false

        defer disposeAllOutRecs()
        executeInternal()

        buildResult(solution)
    }

    method addPath(pg: *ClipperPath, polyType: PolyType)
    {
        highI := cast(int) pg.count - 1
        while highI > 0 and (pg.buffer[highI] == pg.buffer[0])
            highI -= 1
        while highI > 0 and (pg.buffer[highI] == pg.buffer[highI -1])
            highI -= 1
        if highI < 2 return

        edges  := Memory.new'Edge(cast(uint) highI + 1)
        IsFlat := true

        // 1. Basic (first) edge initialization ...
        edges[1].curr = pg.buffer[1]
        rangeTest(pg.buffer[0])
        rangeTest(pg.buffer[highI])
        initEdge(&edges[0], &edges[1], &edges[highI], pg.buffer[0])
        initEdge(&edges[highI], &edges[0], &edges[highI-1], pg.buffer[highI])
        loop i: highI-1 .. 1
        {
            rangeTest(pg.buffer[i])
            initEdge(&edges[i], &edges[i+1], &edges[i-1], pg.buffer[i])
        }

        eStart := &edges[0]

        //2. Remove duplicate vertices, and (when closed) collinear edges ...
        E, eLoopStop := eStart

        loop
        {
            //nb: allows matching start and end points when not Closed ...
            if E.curr == E.next.curr
            {
                if E == E.next break
                if E == eStart eStart = E.next
                E = removeEdge(E)
                eLoopStop = E
                continue
            }

            //only two vertices
            if E.prev == E.next
                break
            else if slopesEqual(E.prev.curr, E.curr, E.next.curr) and
                    (!preserveCollinear or !pt2IsBetweenPt1AndPt3(E.prev.curr, E.curr, E.next.curr))
            {
                if E == eStart eStart = E.next
                E = removeEdge(E)
                E = E.prev
                eLoopStop = E
                continue
            }

            E = E.next
            if E == eLoopStop
                break
        }

        if E.prev == E.next
            return

        //3. Do second stage of edge initialization ...
        E = eStart
        loop
        {
            initEdge2(E, polyType)
            E = E.next
            if IsFlat and E.curr.y != eStart.curr.y IsFlat = false
            if E == eStart break
        }

        //4. Finally, add edge bounds to LocalMinima list ...
        if IsFlat
            return

        edgeList.add(edges);
        var leftBoundIsForward: bool
        var EMin: *Edge

        //workaround to avoid an endless loop in the while loop below when
        //open paths have matching start and end points ...
        if E.prev.bot == E.prev.top E = E.next

        loop
        {
            E = findNextLocMin(E)
            if E == EMin break
            else if !EMin EMin = E

            //E and E.prev now share a local minima (left aligned if horizontal).
            //Compare their slopes to find which starts which bound ...
            var locMin: LocalMinimum
            locMin.y = E.bot.y
            if E.dx < E.prev.dx
            {
                locMin.leftBound = E.prev
                locMin.rightBound = E
                leftBoundIsForward = false
            }
            else
            {
                locMin.leftBound = E
                locMin.rightBound = E.prev
                leftBoundIsForward = true
            }

            if locMin.leftBound.next == locMin.rightBound
                locMin.leftBound.windDelta = -1
            else
                locMin.leftBound.windDelta = 1
            locMin.rightBound.windDelta = -locMin.leftBound.windDelta

            E = processBound(locMin.leftBound, leftBoundIsForward)
            if E.outIdx == Skip E = processBound(E, leftBoundIsForward)

            E2 := processBound(locMin.rightBound, !leftBoundIsForward)
            if E2.outIdx == Skip E2 = processBound(E2, !leftBoundIsForward)

            if locMin.leftBound.outIdx == Skip
                locMin.leftBound = null
            else if locMin.rightBound.outIdx == Skip
                locMin.rightBound = null
            minimaList.add(locMin)
            if !leftBoundIsForward E = E2
        }
    }

    func initEdge(e, eNext, ePrev: *Edge, pt: IntPoint)
    {
        Memory.clear(e, @sizeof(Edge))
        e.next = eNext
        e.prev = ePrev
        e.curr = pt
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
            e.top = e.curr;
            e.bot = e.next.curr
        }

        setDx(e)
        e.polyType = pt
    }

    func setDx(e: *Edge)
    {
        dy := (e.top.y - e.bot.y)
        if dy == 0 e.dx = HorizontalVal
        else e.dx = cast(f64) (e.top.x - e.bot.x) / dy
    }

    func removeEdge(e: *Edge)->*Edge
    {
        e.prev.next = e.next;
        e.next.prev = e.prev;
        result := e.next
        e.prev = null
        return result
    }

    method addJoin(op1, op2: *OutPt, offPt: IntPoint)
    {
        j := Memory.new'Join()
        j.outPt1 = op1
        j.outPt2 = op2
        j.offPt = offPt
        joinList.add(j)
    }

    method slopesEqual(e1: *Edge, e2: *Edge)->bool
    {
        if useFullRange
        {
            v1 := Math.Int128.mul(e1.top.y - e1.bot.y, e2.top.x - e2.bot.x)
            v2 := Math.Int128.mul(e1.top.x - e1.bot.x, e2.top.y - e2.bot.y)
            return v1 == v2
        }

        return (e1.top.y - e1.bot.y) * (e2.top.x - e2.bot.x) == (e1.top.x - e1.bot.x) * (e2.top.y - e2.bot.y)
    }

    method slopesEqual(pt1, pt2, pt3: IntPoint)->bool
    {
        if useFullRange
        {
            v1 := Math.Int128.mul(pt1.y - pt2.y, pt2.x - pt3.x)
            v2 := Math.Int128.mul(pt1.x - pt2.x, pt2.y - pt3.y)
            return v1 == v2
        }

        return (pt1.y - pt2.y) * (pt2.x - pt3.x) == (pt1.x - pt2.x) * (pt2.y - pt3.y)
    }

    method slopesEqual(pt1, pt2, pt3, pt4: IntPoint)->bool
    {
        if useFullRange
        {
            v1 := Math.Int128.mul(pt1.y - pt2.y, pt3.x - pt4.x)
            v2 := Math.Int128.mul(pt1.x - pt2.x, pt3.y - pt4.y)
            return v1 == v2
        }

        return (pt1.y - pt2.y) * (pt3.x - pt4.x) == (pt1.x - pt2.x) * (pt3.y - pt4.y)
    }

    func pt2IsBetweenPt1AndPt3(pt1, pt2, pt3: IntPoint)->bool
    {
        if (pt1 == pt3) or (pt1 == pt2) or (pt3 == pt2)
            return false
        if pt1.x != pt3.x
            return (pt2.x > pt1.x) == (pt2.x < pt3.x)
        return (pt2.y > pt1.y) == (pt2.y < pt3.y)
    }

    method rangeTest(pt: IntPoint)
    {
        if pt.x > LoRange or pt.y > LoRange or -pt.x > LoRange or -pt.y > LoRange
        {
            useFullRange = true
            if pt.x > HiRange or pt.y > HiRange or -pt.x > HiRange or -pt.y > HiRange
                Debug.assert(false, "Coordinate outside allowed range")
        }
    }

    func isHorizontal(e: *Edge)->bool
    {
        return e.dx == HorizontalVal
    }

    func findNextLocMin(edge: *Edge)->*Edge
    {
        E := edge
        loop
        {
            while E.bot != E.prev.bot or E.curr == E.top E = E.next
            if !isHorizontal(E) and !isHorizontal(E.prev) break
            while isHorizontal(E.prev) E = E.prev
            E2 := E;
            while isHorizontal(E) E = E.next
            if E.top.y == E.prev.bot.y continue
            if E2.prev.bot.x < E.bot.x E = E2
            break
        }

        return E
    }

    func reverseHorizontal(e: *Edge)
    {
        swap(&e.top.x, &e.bot.x)
    }

    method processBound(edge: *Edge, nextIsForward: bool)->*Edge
    {
        E := edge
        var Result: *Edge = E
        var Horz: *Edge

        if E.outIdx == Skip
        {
            if nextIsForward
            {
                while E.top.y == E.next.bot.y
                    E = E.next
                while E != Result and isHorizontal(E)
                    E = E.prev
            }
            else
            {
                while E.top.y == E.prev.bot.y
                    E = E.prev
                while E != Result and isHorizontal(E)
                    E = E.next
            }

            if E == Result
            {
                if nextIsForward
                    Result = E.next
                else
                    Result = E.prev
            }
            else
            {
                if nextIsForward
                    E = Result.next
                else
                    E = Result.prev
                var locMin: LocalMinimum
                locMin.y = E.bot.y
                locMin.leftBound = null
                locMin.rightBound = E
                E.windDelta = 0
                Result = processBound(E, nextIsForward)
                minimaList.add(locMin)
            }

            return Result
        }

        var EStart: *Edge
        if isHorizontal(E)
        {
            if nextIsForward
                EStart = E.prev
            else
                EStart = E.next
            if isHorizontal(EStart)
            {
                if EStart.bot.x != E.bot.x and EStart.top.x != E.bot.x
                    reverseHorizontal(E)
            }
            else if EStart.bot.x != E.bot.x
                reverseHorizontal(E)
        }

        EStart = E
        if nextIsForward
        {
            while Result.top.y == Result.next.bot.y and Result.next.outIdx != Skip
                Result = Result.next
            if isHorizontal(Result) and Result.next.outIdx != Skip
            {
                Horz = Result
                while isHorizontal(Horz.prev) Horz = Horz.prev
                if Horz.prev.top.x > Result.next.top.x Result = Horz.prev
            }

            while E != Result
            {
                E.nextInLML = E.next
                if isHorizontal(E) and E != EStart and E.bot.x != E.prev.top.x
                    reverseHorizontal(E)
                E = E.next
            }

            if isHorizontal(E) and E != EStart and E.bot.x != E.prev.top.x
                reverseHorizontal(E)
            Result = Result.next
        }
        else
        {
            while Result.top.y == Result.prev.bot.y and Result.prev.outIdx != Skip
                Result = Result.prev
            if isHorizontal(Result) and Result.prev.outIdx != Skip
            {
                Horz = Result
                while isHorizontal(Horz.next) Horz = Horz.next
                if Horz.next.top.x == Result.prev.top.x or Horz.next.top.x > Result.prev.top.x
                    Result = Horz.next
            }

            while E != Result
            {
                E.nextInLML = E.prev
                if isHorizontal(E) and E != EStart and E.bot.x != E.next.top.x
                    reverseHorizontal(E);
                E = E.prev
            }

            if isHorizontal(E) and E != EStart and E.bot.x != E.next.top.x
                reverseHorizontal(E)
            Result = Result.prev
        }

        return Result
    }

    method popScanbeam(res: *CInt)->bool
    {
        if !scanBeamList.count
            return false

        y := scanBeamList.popBack()
        dref res = y
        while scanBeamList.count and y == scanBeamList.back()
            scanBeamList.count -= 1

        return true
    }

    method insertScanbeam(y: CInt)
    {
        scanBeamList.add(y)
        scanBeamList.sort()
    }

    method reset()
    {
        currentLM = 0
        if currentLM == minimaList.count return
        minimaList.sort()

        scanBeamList.clear()
        visit lm: minimaList
        {
            insertScanbeam(lm.y)
            e := lm.leftBound
            if e
            {
                e.curr = e.bot
                e.side = .Left
                e.outIdx = Unassigned
            }

            e = lm.rightBound
            if e
            {
                e.curr = e.bot
                e.side = .Right
                e.outIdx = Unassigned
            }
        }

        activeEdges = null
        currentLM = 0
    }

    method localMinimaPending()->bool
    {
        return currentLM != minimaList.count
    }

    method deleteFromSEL(e: *Edge)
    {
        SelPrev := e.prevInSEL
        SelNext := e.nextInSEL
        if !SelPrev and !SelNext and (e != sortedEdges) return
        if SelPrev
            SelPrev.nextInSEL = SelNext
        else
            sortedEdges = SelNext
        if SelNext
            SelNext.prevInSEL = SelPrev
        e.nextInSEL = null
        e.prevInSEL = null
    }

    method popEdgeFromSEL(edge: **Edge)->bool
    {
        if !sortedEdges return false
        dref edge = sortedEdges
        deleteFromSEL(sortedEdges)
        return true
    }

    method processHorizontals()
    {
        var horzEdge: *Edge
        while popEdgeFromSEL(&horzEdge)
            processHorizontal(horzEdge)
    }

    func getHorzDirection(HorzEdge: *Edge, Dir: *Direction, left, right: *CInt)
    {
        if HorzEdge.bot.x < HorzEdge.top.x
        {
            dref left = HorzEdge.bot.x
            dref right = HorzEdge.top.x
            dref Dir = .LeftToRight
        }
        else
        {
            dref left = HorzEdge.top.x
            dref right = HorzEdge.bot.x
            dref Dir = .RightToLeft
        }
    }

    func getMaximaPair(e: *Edge)->*Edge
    {
        if (e.next.top == e.top) and !e.next.nextInLML
            return e.next
        else if (e.prev.top == e.top) and !e.prev.nextInLML
            return e.prev
        else
            return null
    }

    func getNextInAEL(e: *Edge, dir: Direction)->*Edge
    {
        return dir == .LeftToRight ? e.nextInAEL : e.prevInAEL
    }

    method getLastOutPt(e: *Edge)->*OutPt
    {
        outRec := polyOuts[e.outIdx]
        if e.side == .Left
            return outRec.pts
        else
            return outRec.pts.prev
    }

    method addGhostJoin(op: *OutPt, offPt: IntPoint)
    {
        j := Memory.new'Join()
        j.outPt1 = op
        j.outPt2 = null
        j.offPt = offPt
        ghostJoinList.add(j)
    }

    method processHorizontal(hE: *Edge)
    {
        horzEdge := hE

        var dir: Direction = ?
        var horzLeft, horzRight: CInt

        isOpen := (horzEdge.windDelta == 0)
        getHorzDirection(horzEdge, &dir, &horzLeft, &horzRight)

        eLastHorz := horzEdge
        var eMaxPair: *Edge
        while eLastHorz.nextInLML and isHorizontal(eLastHorz.nextInLML)
            eLastHorz = eLastHorz.nextInLML
        if !eLastHorz.nextInLML
            eMaxPair = getMaximaPair(eLastHorz)

        var maxIt, maxRit: uint
        if maximaList.count > 0
        {
            if dir == .LeftToRight
            {
                maxIt = 0
                while maxIt != maximaList.count and maximaList.buffer[maxIt] <= horzEdge.bot.x
                    maxIt += 1
                if maxIt != maximaList.count and maximaList.buffer[maxIt] >= eLastHorz.top.x
                    maxIt = maximaList.count
            }
            else
            {
                maxRit = maximaList.count -,safe 1
                while maxRit != Swag.UInt.Max and maximaList.buffer[maxRit] > horzEdge.bot.x
                    maxRit = maxRit -,safe 1
                if maxRit != Swag.UInt.Max and maximaList.buffer[maxRit] <= eLastHorz.top.x
                    maxRit = Swag.UInt.Max
            }
        }

        var op1: *OutPt
        loop
        {
            IsLastHorz := (horzEdge == eLastHorz)
            e := getNextInAEL(horzEdge, dir)
            while e
            {
                if maximaList.count > 0
                {
                    if dir == .LeftToRight
                    {
                        while maxIt != maximaList.count and maximaList.buffer[maxIt] < e.curr.x
                        {
                            if horzEdge.outIdx >= 0 and !isOpen
                                addOutPt(horzEdge, IntPoint{maximaList.buffer[maxIt], horzEdge.bot.y})
                            maxIt += 1
                        }
                    }
                    else
                    {
                        while maxRit != Swag.UInt.Max and maximaList.buffer[maxRit] > e.curr.x
                        {
                            if horzEdge.outIdx >= 0 and !isOpen
                                addOutPt(horzEdge, IntPoint{maximaList.buffer[maxRit], horzEdge.bot.y})
                            maxRit = maxRit -,safe 1
                        }
                    }
                }

                if (dir == .LeftToRight and e.curr.x > horzRight) or (dir == .RightToLeft and e.curr.x < horzLeft)
                    break
                if e.curr.x == horzEdge.top.x and horzEdge.nextInLML and e.dx < horzEdge.nextInLML.dx
                    break

                if horzEdge.outIdx >= 0 and !isOpen
                {
                    op1 = addOutPt(horzEdge, e.curr)
                    eNextHorz := sortedEdges
                    while eNextHorz
                    {
                        if eNextHorz.outIdx >= 0 and
                           horzSegmentsOverlap(horzEdge.bot.x, horzEdge.top.x, eNextHorz.bot.x, eNextHorz.top.x)
                        {
                            op2 := getLastOutPt(eNextHorz)
                            addJoin(op2, op1, eNextHorz.top)
                        }

                        eNextHorz = eNextHorz.nextInSEL
                    }

                    addGhostJoin(op1, horzEdge.bot)
                }

                if e == eMaxPair and IsLastHorz
                {
                    if horzEdge.outIdx >= 0
                        addLocalMaxPoly(horzEdge, eMaxPair, horzEdge.top)
                    deleteFromAEL(horzEdge)
                    deleteFromAEL(eMaxPair)
                    return
                }

                if dir == .LeftToRight
                {
                    pt := IntPoint{e.curr.x, horzEdge.curr.y}
                    intersectEdges(horzEdge, e, pt)
                }
                else
                {
                    pt := IntPoint{e.curr.x, horzEdge.curr.y}
                    intersectEdges(e, horzEdge, pt)
                }

                eNext := getNextInAEL(e, dir)
                swapPositionsInAEL(horzEdge, e)
                e = eNext
            }

            if !horzEdge.nextInLML or !isHorizontal(horzEdge.nextInLML) break
            updateEdgeIntoAEL(&horzEdge)
            if horzEdge.outIdx >= 0 addOutPt(horzEdge, horzEdge.bot)
            getHorzDirection(horzEdge, &dir, &horzLeft, &horzRight)
        }

        if horzEdge.outIdx >= 0 and !op1
        {
            op1 = getLastOutPt(horzEdge)
            eNextHorz := sortedEdges
            while eNextHorz
            {
                if eNextHorz.outIdx >= 0 and
                   horzSegmentsOverlap(horzEdge.bot.x, horzEdge.top.x, eNextHorz.bot.x, eNextHorz.top.x)
                {
                    op2 := getLastOutPt(eNextHorz)
                    addJoin(op2, op1, eNextHorz.top)
                }

                eNextHorz = eNextHorz.nextInSEL
            }

            addGhostJoin(op1, horzEdge.top)
        }

        if horzEdge.nextInLML
        {
            if horzEdge.outIdx >= 0
            {
                op1 = addOutPt(horzEdge, horzEdge.top)
                updateEdgeIntoAEL(&horzEdge)
                if horzEdge.windDelta == 0 return
                ePrev := horzEdge.prevInAEL
                eNext := horzEdge.nextInAEL
                if ePrev and ePrev.curr.x == horzEdge.bot.x and
                   ePrev.curr.y == horzEdge.bot.y and ePrev.windDelta != 0 and
                   ePrev.outIdx >= 0 and ePrev.curr.y > ePrev.top.y and slopesEqual(horzEdge, ePrev)
                {
                    op2 := addOutPt(ePrev, horzEdge.bot)
                    addJoin(op1, op2, horzEdge.top)
                }
                else if eNext and eNext.curr.x == horzEdge.bot.x and
                        eNext.curr.y == horzEdge.bot.y and eNext.windDelta != 0 and
                        eNext.outIdx >= 0 and eNext.curr.y > eNext.top.y and slopesEqual(horzEdge, eNext)
                {
                    op2 := addOutPt(eNext, horzEdge.bot)
                    addJoin(op1, op2, horzEdge.top)
                }
            }
            else
                updateEdgeIntoAEL(&horzEdge)
        }
        else
        {
            if horzEdge.outIdx >= 0 addOutPt(horzEdge, horzEdge.top)
            deleteFromAEL(horzEdge)
        }
    }

    method updateEdgeIntoAEL(ed: **Edge)
    {
        e := dref ed
        e.nextInLML.outIdx = e.outIdx
        AelPrev := e.prevInAEL
        AelNext := e.nextInAEL
        if AelPrev AelPrev.nextInAEL = e.nextInLML
        else activeEdges = e.nextInLML
        if AelNext AelNext.prevInAEL = e.nextInLML
        e.nextInLML.side = e.side
        e.nextInLML.windDelta = e.windDelta
        e.nextInLML.windCnt = e.windCnt
        e.nextInLML.windCnt2 = e.windCnt2
        e = e.nextInLML
        e.curr = e.bot
        e.prevInAEL = AelPrev
        e.nextInAEL = AelNext
        if !isHorizontal(e) insertScanbeam(e.top.y)
        dref ed = e
    }

    method swapPositionsInAEL(edge1, edge2: *Edge)
    {
        if edge1.nextInAEL == edge1.prevInAEL or edge2.nextInAEL == edge2.prevInAEL
            return

        if edge1.nextInAEL == edge2
        {
            next := edge2.nextInAEL;
            if next next.prevInAEL = edge1
            prev := edge1.prevInAEL
            if prev prev.nextInAEL = edge2
            edge2.prevInAEL = prev
            edge2.nextInAEL = edge1
            edge1.prevInAEL = edge2
            edge1.nextInAEL = next
        }
        else if edge2.nextInAEL == edge1
        {
            next := edge1.nextInAEL;
            if next next.prevInAEL = edge2
            prev := edge2.prevInAEL
            if prev prev.nextInAEL = edge1
            edge1.prevInAEL = prev
            edge1.nextInAEL = edge2
            edge2.prevInAEL = edge1
            edge2.nextInAEL = next
        }
        else
        {
            next := edge1.nextInAEL
            prev := edge1.prevInAEL
            edge1.nextInAEL = edge2.nextInAEL
            if edge1.nextInAEL edge1.nextInAEL.prevInAEL = edge1
            edge1.prevInAEL = edge2.prevInAEL
            if edge1.prevInAEL edge1.prevInAEL.nextInAEL = edge1
            edge2.nextInAEL = next
            if edge2.nextInAEL edge2.nextInAEL.prevInAEL = edge2
            edge2.prevInAEL = prev
            if edge2.prevInAEL edge2.prevInAEL.nextInAEL = edge2
        }

        if !edge1.prevInAEL activeEdges = edge1
        else if !edge2.prevInAEL activeEdges = edge2
    }

    method deleteFromAEL(e: *Edge)
    {
        AelPrev := e.prevInAEL
        AelNext := e.nextInAEL
        if !AelPrev and !AelNext and (e != activeEdges)
            return
        if AelPrev
            AelPrev.nextInAEL = AelNext
        else
            activeEdges = AelNext
        if AelNext
            AelNext.prevInAEL = AelPrev
        e.nextInAEL = null
        e.prevInAEL = null
    }

    method clearJoins()
    {
        visit p: joinList
            Memory.delete(p)
        joinList.clear()
    }

    method clearGhostJoins()
    {
        visit p: ghostJoinList
            Memory.delete(p)
        ghostJoinList.clear()
    }

    method executeInternal()
    {
        reset()
        maximaList.clear()
        sortedEdges = null

        var botY, topY: CInt
        if !popScanbeam(&botY) return

        insertLocalMinimaIntoAEL(botY)
        while popScanbeam(&topY) or localMinimaPending()
        {
            processHorizontals()
            clearGhostJoins()
            processIntersections(topY)
            processEdgesAtTopOfScanbeam(topY)
            botY = topY
            insertLocalMinimaIntoAEL(botY)
        }

        //fix orientations ...
        visit outRec: polyOuts
        {
            if !outRec.pts or outRec.isOpen continue
            if (outRec.isHole ^ reverseOutput) == (area(outRec.pts) > 0)
                reversePolyPtLinks(outRec.pts)
        }

        if joinList.count joinCommonEdges()

        //unfortunately fixupOutPolygon() must be done after joinCommonEdges()
        visit outRec: polyOuts
        {
            if !outRec.pts continue
            if outRec.isOpen
                fixupOutPolyline(outRec)
            else
                fixupOutPolygon(outRec)
        }

        if strictSimple doSimplePolygons()

        clearJoins()
        clearGhostJoins()
    }

    method doSimplePolygons()
    {
        var i: uint
        while i < polyOuts.count
        {
            outrec := polyOuts[i]
            i += 1
            op := outrec.pts
            if !op or outrec.isOpen continue

            loop
            {
                op2 := op.next
                while op2 != outrec.pts
                {
                    if op.pt == op2.pt and op2.next != op and op2.prev != op
                    {
                        op3 := op.prev
                        op4 := op2.prev
                        op.prev = op4
                        op4.next = op
                        op2.prev = op3
                        op3.next = op2

                        outrec.pts = op
                        outrec2 := createOutRec()
                        outrec2.pts = op2
                        updateOutPtIdxs(outrec2)
                        if poly2ContainsPoly1(outrec2.pts, outrec.pts)
                        {
                            outrec2.isHole = !outrec.isHole
                            outrec2.firstLeft = outrec
                            if usingPolyTree fixupFirstLefts2(outrec2, outrec)
                        }
                        else if poly2ContainsPoly1(outrec.pts, outrec2.pts)
                        {
                            outrec2.isHole = outrec.isHole
                            outrec.isHole = !outrec2.isHole
                            outrec2.firstLeft = outrec.firstLeft
                            outrec.firstLeft = outrec2
                            if usingPolyTree fixupFirstLefts2(outrec, outrec2)
                        }
                        else
                        {
                            outrec2.isHole = outrec.isHole
                            outrec2.firstLeft = outrec.firstLeft
                            if usingPolyTree fixupFirstLefts1(outrec, outrec2)
                        }

                        op2 = op
                    }

                    op2 = op2.next
                }

                op = op.next
                if op == outrec.pts break
            }
        }
    }

    func fixupOutPolyline(outrec: *OutRec)
    {
        pp := outrec.pts
        lastPP := pp.prev
        while pp != lastPP
        {
            pp = pp.next
            if pp.pt == pp.prev.pt
            {
                if pp == lastPP lastPP = pp.prev
                tmpPP := pp.prev
                tmpPP.next = pp.next
                pp.next.prev = tmpPP
                Memory.delete(pp)
                pp = tmpPP
            }
        }

        if pp == pp.prev
        {
            disposeOutPts(pp)
            outrec.pts = null
            return
        }
    }

    method fixupOutPolygon(outrec: *OutRec)
    {
        var lastOK: *OutPt
        outrec.bottomPt = null
        pp := outrec.pts
        preserveCol := preserveCollinear or strictSimple

        loop
        {
            if pp.prev == pp or pp.prev == pp.next
            {
                disposeOutPts(pp)
                outrec.pts = null
                return
            }

            if (pp.pt == pp.next.pt) or (pp.pt == pp.prev.pt) or
                (slopesEqual(pp.prev.pt, pp.pt, pp.next.pt) and
                (!preserveCol or !pt2IsBetweenPt1AndPt3(pp.prev.pt, pp.pt, pp.next.pt)))
            {
                lastOK = null
                tmp := pp
                pp.prev.next = pp.next
                pp.next.prev = pp.prev
                pp = pp.prev
                Memory.delete(tmp)
            }
            else if pp == lastOK
                break
            else
            {
                if !lastOK lastOK = pp
                pp = pp.next
            }
        }

        outrec.pts = pp
    }

    method getOutRec(idx: s32)->*OutRec
    {
        outrec := polyOuts[idx]
        while outrec != polyOuts[outrec.idx]
            outrec = polyOuts[outrec.idx]
        return outrec
    }

    method joinPoints(j: *Join, outRec1, outRec2: *OutRec)->bool
    {
        op1 := j.outPt1
        op2 := j.outPt2
        var op1b, op2b: *OutPt

        isHorizontal := (j.outPt1.pt.y == j.offPt.y)

        if isHorizontal and (j.offPt == j.outPt1.pt) and (j.offPt == j.outPt2.pt)
        {
            if outRec1 != outRec2 return false
            op1b = j.outPt1.next
            while op1b != op1 and (op1b.pt == j.offPt)
                op1b = op1b.next
            reverse1 := (op1b.pt.y > j.offPt.y)
            op2b = j.outPt2.next
            while op2b != op2 and (op2b.pt == j.offPt)
                op2b = op2b.next
            reverse2 := (op2b.pt.y > j.offPt.y)
            if reverse1 == reverse2 return false
            if reverse1
            {
                op1b = dupOutPt(op1, false)
                op2b = dupOutPt(op2, true)
                op1.prev = op2
                op2.next = op1
                op1b.next = op2b
                op2b.prev = op1b
                j.outPt1 = op1
                j.outPt2 = op1b
                return true
            }

            op1b = dupOutPt(op1, true)
            op2b = dupOutPt(op2, false)
            op1.next = op2
            op2.prev = op1
            op1b.prev = op2b
            op2b.next = op1b
            j.outPt1 = op1
            j.outPt2 = op1b
            return true
        }

        if isHorizontal
        {
            op1b = op1
            while op1.prev.pt.y == op1.pt.y and op1.prev != op1b and op1.prev != op2
                op1 = op1.prev
            while op1b.next.pt.y == op1b.pt.y and op1b.next != op1 and op1b.next != op2
                op1b = op1b.next
            if op1b.next == op1 or op1b.next == op2 return false

            op2b = op2
            while op2.prev.pt.y == op2.pt.y and op2.prev != op2b and op2.prev != op1b
                op2 = op2.prev
            while op2b.next.pt.y == op2b.pt.y and op2b.next != op2 and op2b.next != op1
                op2b = op2b.next
            if op2b.next == op2 or op2b.next == op1 return false

            var left, right: CInt
            if !getOverlap(op1.pt.x, op1b.pt.x, op2.pt.x, op2b.pt.x, &left, &right)
                return false

            var pt: IntPoint
            var DiscardLeftSide: bool
            if op1.pt.x >= left and op1.pt.x <= right
            {
                pt = op1.pt
                DiscardLeftSide = (op1.pt.x > op1b.pt.x)
            }
            else if op2.pt.x >= left and op2.pt.x <= right
            {
                pt = op2.pt
                DiscardLeftSide = (op2.pt.x > op2b.pt.x)
            }
            else if op1b.pt.x >= left and op1b.pt.x <= right
            {
                pt = op1b.pt
                DiscardLeftSide = op1b.pt.x > op1.pt.x
            }
            else
            {
                pt = op2b.pt
                DiscardLeftSide = (op2b.pt.x > op2.pt.x)
            }

            j.outPt1 = op1
            j.outPt2 = op2
            return joinHorz(op1, op1b, op2, op2b, pt, DiscardLeftSide)
        }

        op1b = op1.next
        while (op1b.pt == op1.pt) and (op1b != op1)
            op1b = op1b.next
        Reverse1 := ((op1b.pt.y > op1.pt.y) or !slopesEqual(op1.pt, op1b.pt, j.offPt))
        if Reverse1
        {
            op1b = op1.prev
            while (op1b.pt == op1.pt) and (op1b != op1)
                op1b = op1b.prev
            if (op1b.pt.y > op1.pt.y) or !slopesEqual(op1.pt, op1b.pt, j.offPt)
                return false
        }

        op2b = op2.next
        while (op2b.pt == op2.pt) and (op2b != op2)
            op2b = op2b.next
        Reverse2 := ((op2b.pt.y > op2.pt.y) or !slopesEqual(op2.pt, op2b.pt, j.offPt))
        if Reverse2
        {
            op2b = op2.prev
            while (op2b.pt == op2.pt) and (op2b != op2)
                op2b = op2b.prev
            if (op2b.pt.y > op2.pt.y) or !slopesEqual(op2.pt, op2b.pt, j.offPt)
                return false
        }

        if (op1b == op1) or (op2b == op2) or (op1b == op2b) or ((outRec1 == outRec2) and (Reverse1 == Reverse2))
            return false

        if (Reverse1)
        {
            op1b = dupOutPt(op1, false)
            op2b = dupOutPt(op2, true)
            op1.prev = op2
            op2.next = op1
            op1b.next = op2b
            op2b.prev = op1b
            j.outPt1 = op1
            j.outPt2 = op1b
            return true
        }

        op1b = dupOutPt(op1, true)
        op2b = dupOutPt(op2, false)
        op1.next = op2
        op2.prev = op1
        op1b.prev = op2b
        op2b.next = op1b
        j.outPt1 = op1
        j.outPt2 = op1b
        return true
    }

    func joinHorz(op1P, op1bP, op2P, op2bP: *OutPt, pt: IntPoint, discardLeft: bool)->bool
    {
        op1 := op1P
        op1b:= op1bP
        op2 := op2P
        op2b:= op2bP

        var Dir1: Direction = (op1.pt.x > op1b.pt.x ? .RightToLeft : .LeftToRight)
        var Dir2: Direction = (op2.pt.x > op2b.pt.x ? .RightToLeft : .LeftToRight)
        if Dir1 == Dir2 return false

        if Dir1 == .LeftToRight
        {
            while op1.next.pt.x <= pt.x and op1.next.pt.x >= op1.pt.x and op1.next.pt.y == pt.y
                op1 = op1.next
            if discardLeft and (op1.pt.x != pt.x)
                op1 = op1.next
            op1b = dupOutPt(op1, !discardLeft)
            if op1b.pt != pt
            {
                op1 = op1b
                op1.pt = pt
                op1b = dupOutPt(op1, !discardLeft)
            }
        }
        else
        {
            while op1.next.pt.x >= pt.x and op1.next.pt.x <= op1.pt.x and op1.next.pt.y == pt.y
                op1 = op1.next
            if !discardLeft and (op1.pt.x != pt.x)
                op1 = op1.next
            op1b = dupOutPt(op1, discardLeft)
            if op1b.pt != pt
            {
                op1 = op1b
                op1.pt = pt
                op1b = dupOutPt(op1, discardLeft)
            }
        }

        if Dir2 == .LeftToRight
        {
            while op2.next.pt.x <= pt.x and op2.next.pt.x >= op2.pt.x and op2.next.pt.y == pt.y
                op2 = op2.next
            if discardLeft and (op2.pt.x != pt.x)
                op2 = op2.next
            op2b = dupOutPt(op2, !discardLeft)
            if op2b.pt != pt
            {
                op2 = op2b
                op2.pt = pt
                op2b = dupOutPt(op2, !discardLeft)
            }
        }
        else
        {
            while op2.next.pt.x >= pt.x and op2.next.pt.x <= op2.pt.x and op2.next.pt.y == pt.y
                op2 = op2.next
            if !discardLeft and (op2.pt.x != pt.x)
                op2 = op2.next
            op2b = dupOutPt(op2, discardLeft)
            if op2b.pt != pt
            {
                op2 = op2b
                op2.pt = pt
                op2b = dupOutPt(op2, discardLeft)
            }
        }

        if Dir1 == .LeftToRight and discardLeft
        {
            op1.prev = op2
            op2.next = op1
            op1b.next = op2b
            op2b.prev = op1b
        }
        else
        {
            op1.next = op2
            op2.prev = op1
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
                dref left = Math.max(a1, b1)
                dref right = Math.min(a2, b2)
            }
            else
            {
                dref left = Math.max(a1, b2)
                dref right = Math.min(a2, b1)
            }
        }
        else
        {
            if b1 < b2
            {
                dref left = Math.max(a2, b1)
                dref right = Math.min(a1, b2)
            }
            else
            {
                dref left = Math.max(a2, b2)
                dref right = Math.min(a1, b1)
            }
        }

        return left < right
    }

    func dupOutPt(outPt: *OutPt, InsertAfter: bool)->*OutPt
    {
        result := Memory.new'OutPt()
        result.pt = outPt.pt
        result.idx = outPt.idx
        if InsertAfter
        {
            result.next = outPt.next
            result.prev = outPt
            outPt.next.prev = result
            outPt.next = result
        }
        else
        {
            result.prev = outPt.prev
            result.next = outPt
            outPt.prev.next = result
            outPt.prev = result
        }

        return result
    }

    func updateOutPtIdxs(outrec: *OutRec)
    {
        op := outrec.pts
        loop
        {
            op.idx = outrec.idx
            op = op.prev
            if op == outrec.pts break
        }
    }

    func pointInPolygon(pt: IntPoint, outPoints: *OutPt)->s32
    {
        op := outPoints
        result := 0
        startOp := op
        loop
        {
            if op.next.pt.y == pt.y
            {
                if (op.next.pt.x == pt.x) or (op.pt.y == pt.y and ((op.next.pt.x > pt.x) == (op.pt.x < pt.x)))
                    return -1
            }

            if (op.pt.y < pt.y) != (op.next.pt.y < pt.y)
            {
                if op.pt.x >= pt.x
                {
                    if op.next.pt.x > pt.x
                        result = 1 - result
                    else
                    {
                        d := cast(f64) (op.pt.x - pt.x) * (op.next.pt.y - pt.y) - cast(f64) (op.next.pt.x - pt.x) * (op.pt.y - pt.y)
                        if d == 0 return -1
                        if (d > 0) == (op.next.pt.y > op.pt.y) result = 1 - result
                    }
                }
                else
                {
                    if op.next.pt.x > pt.x
                    {
                        d := cast(f64) (op.pt.x - pt.x) * (op.next.pt.y - pt.y) - cast(f64) (op.next.pt.x - pt.x) * (op.pt.y - pt.y)
                        if d == 0 return -1
                        if (d > 0) == (op.next.pt.y > op.pt.y) result = 1 - result
                    }
                }
            }

            op = op.next
            if startOp == op break
        }

        return result
    }

    func poly2ContainsPoly1(outPt1, outPt2: *OutPt)->bool
    {
        op := outPt1
        loop
        {
            res := pointInPolygon(op.pt, outPt2)
            if res >= 0 return res > 0
            op = op.next
            if op == outPt1 break
        }

        return true
    }

    func parseFirstLeft(FL: *OutRec)->*OutRec
    {
        firstLeft := FL
        while firstLeft and !firstLeft.pts
            firstLeft = firstLeft.firstLeft
        return firstLeft
    }

    method fixupFirstLefts2(innerOutRec, outerOutRec: *OutRec)
    {
        orfl := outerOutRec.firstLeft
        visit outRec: polyOuts
        {
            if !outRec.pts or outRec == outerOutRec or outRec == innerOutRec
                continue
            firstLeft := parseFirstLeft(outRec.firstLeft)
            if firstLeft != orfl and firstLeft != innerOutRec and firstLeft != outerOutRec
                continue
            if poly2ContainsPoly1(outRec.pts, innerOutRec.pts)
                outRec.firstLeft = innerOutRec
            else if poly2ContainsPoly1(outRec.pts, outerOutRec.pts)
                outRec.firstLeft = outerOutRec
            else if outRec.firstLeft == innerOutRec or outRec.firstLeft == outerOutRec
                outRec.firstLeft = orfl
        }
    }

    method fixupFirstLefts1(oldOutRec, newOutRec: *OutRec)
    {
        visit outRec: polyOuts
        {
            firstLeft := parseFirstLeft(outRec.firstLeft)
            if outRec.pts and firstLeft == oldOutRec
            {
                if poly2ContainsPoly1(outRec.pts, newOutRec.pts)
                    outRec.firstLeft = newOutRec
            }
        }
    }

    method fixupFirstLefts3(oldOutRec, newOutRec: *OutRec)
    {
        visit outRec: polyOuts
        {
            firstLeft := parseFirstLeft(outRec.firstLeft)
            if outRec.pts and firstLeft == oldOutRec
                outRec.firstLeft = newOutRec
        }
    }

    method joinCommonEdges()
    {
        visit join: joinList
        {
            outRec1 := getOutRec(join.outPt1.idx)
            outRec2 := getOutRec(join.outPt2.idx)

            if !outRec1.pts or !outRec2.pts continue
            if outRec1.isOpen or outRec2.isOpen continue

            var holeStateRec: *OutRec
            if outRec1 == outRec2 holeStateRec = outRec1
            else if outRec1RightOfOutRec2(outRec1, outRec2) holeStateRec = outRec2
            else if outRec1RightOfOutRec2(outRec2, outRec1) holeStateRec = outRec1
            else holeStateRec = getLowermostRec(outRec1, outRec2)

            if !joinPoints(join, outRec1, outRec2) continue

            if (outRec1 == outRec2)
            {
                outRec1.pts = join.outPt1
                outRec1.bottomPt = null
                outRec2 = createOutRec()
                outRec2.pts = join.outPt2
                updateOutPtIdxs(outRec2)

                if poly2ContainsPoly1(outRec2.pts, outRec1.pts)
                {
                    outRec2.isHole = !outRec1.isHole
                    outRec2.firstLeft = outRec1
                    if usingPolyTree fixupFirstLefts2(outRec2, outRec1)
                    if (outRec2.isHole ^ reverseOutput) == (area(outRec2.pts) > 0)
                    reversePolyPtLinks(outRec2.pts)

                }
                else if poly2ContainsPoly1(outRec1.pts, outRec2.pts)
                {
                    outRec2.isHole = outRec1.isHole
                    outRec1.isHole = !outRec2.isHole
                    outRec2.firstLeft = outRec1.firstLeft
                    outRec1.firstLeft = outRec2

                    if usingPolyTree fixupFirstLefts2(outRec1, outRec2)

                    if (outRec1.isHole ^ reverseOutput) == (area(outRec1.pts) > 0)
                    reversePolyPtLinks(outRec1.pts)
                }
                else
                {
                    outRec2.isHole = outRec1.isHole
                    outRec2.firstLeft = outRec1.firstLeft
                    if usingPolyTree fixupFirstLefts1(outRec1, outRec2)
                }

            }
            else
            {
                outRec2.pts = null
                outRec2.bottomPt = null
                outRec2.idx = outRec1.idx

                outRec1.isHole = holeStateRec.isHole
                if holeStateRec == outRec2
                    outRec1.firstLeft = outRec2.firstLeft
                outRec2.firstLeft = outRec1

                if usingPolyTree fixupFirstLefts3(outRec2, outRec1)
            }
        }
    }

    func isMaxima(e: *Edge, y: CInt)->bool
    {
        return e and e.top.y == y and !e.nextInLML
    }

    func getMaximaPairEx(e: *Edge)->*Edge
    {
        result := getMaximaPair(e)
        if result and (result.outIdx == Skip or
           (result.nextInAEL == result.prevInAEL and !isHorizontal(result)))
            return null
        return result
    }

    func isIntermediate(e: *Edge, y: CInt)->bool
    {
        return e.top.y == y and e.nextInLML
    }

    method doMaxima(e: *Edge)
    {
        eMaxPair := getMaximaPairEx(e)
        if !eMaxPair
        {
            if e.outIdx >= 0
                addOutPt(e, e.top)
            deleteFromAEL(e)
            return;
        }

        eNext := e.nextInAEL
        while eNext and eNext != eMaxPair
        {
            intersectEdges(e, eNext, e.top)
            swapPositionsInAEL(e, eNext)
            eNext = e.nextInAEL
        }

        if e.outIdx == Unassigned and eMaxPair.outIdx == Unassigned
        {
            deleteFromAEL(e)
            deleteFromAEL(eMaxPair)
        }
        else if e.outIdx >= 0 and eMaxPair.outIdx >= 0
        {
            if e.outIdx >= 0 addLocalMaxPoly(e, eMaxPair, e.top)
            deleteFromAEL(e)
            deleteFromAEL(eMaxPair)
        }
        else
        {
            Debug.assert(false)
        }
    }

    method processEdgesAtTopOfScanbeam(topY: CInt)
    {
        e := activeEdges
        while e
        {
            //1. process maxima, treating them as if they're 'bent' horizontal edges,
            //   but exclude maxima with horizontal edges. nb: e can't be a horizontal.
            isMaximaEdge := isMaxima(e, topY)
            if isMaximaEdge
            {
                eMaxPair := getMaximaPairEx(e)
                isMaximaEdge = !eMaxPair or !isHorizontal(eMaxPair)
            }

            if isMaximaEdge
            {
                if strictSimple maximaList.add(e.top.x)
                ePrev := e.prevInAEL
                doMaxima(e)
                if !ePrev e = activeEdges
                else e = ePrev.nextInAEL
            }
            else
            {
                //2. promote horizontal edges, otherwise update curr.x and curr.y ...
                if isIntermediate(e, topY) and isHorizontal(e.nextInLML)
                {
                    updateEdgeIntoAEL(&e)
                    if e.outIdx >= 0
                        addOutPt(e, e.bot)
                    addEdgeToSEL(e)
                }
                else
                {
                    e.curr.x = topX(e, topY)
                    e.curr.y = topY
                }

                //When StrictlySimple and 'e' is being touched by another edge, then
                //make sure both edges have a vertex here ...
                if strictSimple
                {
                    ePrev := e.prevInAEL
                    if  (e.outIdx >= 0) and (e.windDelta != 0) and ePrev and (ePrev.outIdx >= 0) and
                        (ePrev.curr.x == e.curr.x) and (ePrev.windDelta != 0)
                    {
                        pt := e.curr
                        op := addOutPt(ePrev, pt)
                        op2 := addOutPt(e, pt)
                        addJoin(op, op2, pt)
                    }
                }

                e = e.nextInAEL
            }
        }

        //3. Process horizontals at the top of the scanbeam ...
        maximaList.sort()
        processHorizontals()
        maximaList.clear()

        //4. Promote intermediate vertices ...
        e = activeEdges
        while e
        {
            if isIntermediate(e, topY)
            {
                var op: *OutPt
                if e.outIdx >= 0
                    op = addOutPt(e, e.top)
                updateEdgeIntoAEL(&e)

                ePrev := e.prevInAEL
                eNext := e.nextInAEL
                if ePrev and ePrev.curr.x == e.bot.x and
                    ePrev.curr.y == e.bot.y and op and
                    ePrev.outIdx >= 0 and ePrev.curr.y > ePrev.top.y and
                    slopesEqual(e.curr, e.top, ePrev.curr, ePrev.top) and
                    (e.windDelta != 0) and (ePrev.windDelta != 0)
                {
                    op2 := addOutPt(ePrev, e.bot)
                    addJoin(op, op2, e.top)
                }
                else if eNext and eNext.curr.x == e.bot.x and
                    eNext.curr.y == e.bot.y and op and
                    eNext.outIdx >= 0 and eNext.curr.y > eNext.top.y and
                    slopesEqual(e.curr, e.top, eNext.curr, eNext.top) and
                    (e.windDelta != 0) and (eNext.windDelta != 0)
                {
                    op2 := addOutPt(eNext, e.bot)
                    addJoin(op, op2, e.top)
                }
            }

            e = e.nextInAEL
        }
    }

    method processIntersections(topY: CInt)
    {
        if !activeEdges return
        buildIntersectList(topY)
        IlSize := intersectList.count
        if IlSize == 0 return

        if IlSize == 1 or fixupIntersectionOrder()
            processIntersectList()

        sortedEdges = null
    }

    method buildIntersectList(topY: CInt)
    {
        if !activeEdges return

        e := activeEdges
        sortedEdges = e
        while e
        {
            e.prevInSEL = e.prevInAEL
            e.nextInSEL = e.nextInAEL
            e.curr.x = topX(e, topY)
            e = e.nextInAEL
        }

        var isModified: bool
        loop
        {
            isModified = false
            e = sortedEdges
            while e.nextInSEL
            {
                eNext := e.nextInSEL
                var pt: IntPoint
                if e.curr.x > eNext.curr.x
                {
                    intersectPoint(e, eNext, &pt)
                    if pt.y < topY pt = IntPoint{topX(e, topY), topY}
                    newNode := Memory.new'IntersectNode()
                    newNode.edge1 = e;
                    newNode.edge2 = eNext
                    newNode.pt = pt
                    intersectList.add(newNode)
                    swapPositionsInSEL(e, eNext)
                    isModified = true
                }
                else
                    e = eNext
            }

            if !e.prevInSEL break
            e.prevInSEL.nextInSEL = null
            if !isModified break
        }

        sortedEdges = null
    }

    method copyAELToSEL()
    {
        e := activeEdges
        sortedEdges = e
        while e
        {
            e.prevInSEL = e.prevInAEL
            e.nextInSEL = e.nextInAEL
            e = e.nextInAEL
        }
    }

    func edgesAdjacent(inode: *IntersectNode)->bool
    {
        return (inode.edge1.nextInSEL == inode.edge2) or (inode.edge1.prevInSEL == inode.edge2)
    }

    method fixupIntersectionOrder()->bool
    {
        copyAELToSEL()
        intersectList.sort()
        loop i: intersectList.count
        {
            if !edgesAdjacent(intersectList[i])
            {
                j := i + 1
                while j < intersectList.count and !edgesAdjacent(intersectList[j]) j += 1
                if j == intersectList.count return false
                swap(intersectList.buffer + i, intersectList.buffer + j)
            }

            swapPositionsInSEL(intersectList[i].edge1, intersectList[i].edge2)
        }

        return true
    }

    method processIntersectList()
    {
        visit iNode: intersectList
        {
            intersectEdges(iNode.edge1, iNode.edge2, iNode.pt)
            swapPositionsInAEL(iNode.edge1, iNode.edge2)
            Memory.delete(iNode)
        }

        intersectList.clear()
    }

    method swapPositionsInSEL(edge1, edge2: *Edge)
    {
        if !edge1.nextInSEL and !edge1.prevInSEL return
        if !edge2.nextInSEL and !edge2.prevInSEL return

        if edge1.nextInSEL == edge2
        {
            next := edge2.nextInSEL
            if next next.prevInSEL = edge1
            prev := edge1.prevInSEL
            if prev prev.nextInSEL = edge2
            edge2.prevInSEL = prev
            edge2.nextInSEL = edge1
            edge1.prevInSEL = edge2
            edge1.nextInSEL = next
        }
        else if edge2.nextInSEL == edge1
        {
            next := edge1.nextInSEL
            if next next.prevInSEL = edge2
            prev := edge2.prevInSEL
            if prev prev.nextInSEL = edge1
            edge1.prevInSEL = prev
            edge1.nextInSEL = edge2
            edge2.prevInSEL = edge1
            edge2.nextInSEL = next
        }
        else
        {
            next := edge1.nextInSEL
            prev := edge1.prevInSEL
            edge1.nextInSEL = edge2.nextInSEL
            if edge1.nextInSEL edge1.nextInSEL.prevInSEL = edge1
            edge1.prevInSEL = edge2.prevInSEL
            if edge1.prevInSEL edge1.prevInSEL.nextInSEL = edge1
            edge2.nextInSEL = next
            if edge2.nextInSEL edge2.nextInSEL.prevInSEL = edge2
            edge2.prevInSEL = prev
            if edge2.prevInSEL edge2.prevInSEL.nextInSEL = edge2
        }

        if !edge1.prevInSEL sortedEdges = edge1
        else if !edge2.prevInSEL sortedEdges = edge2
    }

    method intersectPoint(edge1, edge2: *Edge, ip: *IntPoint)
    {
        var b1, b2: f64
        if edge1.dx == edge2.dx
        {
            ip.y = edge1.curr.y
            ip.x = topX(edge1, ip.y)
            return
        }
        else if edge1.dx == 0
        {
            ip.x = edge1.bot.x
            if isHorizontal(edge2)
                ip.y = edge2.bot.y
            else
            {
                b2 = edge2.bot.y - (edge2.bot.x / edge2.dx)
                ip.y = round(ip.x / edge2.dx + b2)
            }
        }
        else if edge2.dx == 0
        {
            ip.x = edge2.bot.x
            if isHorizontal(edge1)
                ip.y = edge1.bot.y
            else
            {
                b1 = edge1.bot.y - (edge1.bot.x / edge1.dx)
                ip.y = round(ip.x / edge1.dx + b1)
            }
        }
        else
        {
            b1 = edge1.bot.x - edge1.bot.y * edge1.dx
            b2 = edge2.bot.x - edge2.bot.y * edge2.dx
            q := (b2-b1) / (edge1.dx - edge2.dx)
            ip.y = round(q)
            if Math.abs(edge1.dx) < Math.abs(edge2.dx)
                ip.x = round(edge1.dx * q + b1)
            else
                ip.x = round(edge2.dx * q + b2)
        }

        if ip.y < edge1.top.y or ip.y < edge2.top.y
        {
            if edge1.top.y > edge2.top.y
                ip.y = edge1.top.y
            else
                ip.y = edge2.top.y
            if Math.abs(edge1.dx) < Math.abs(edge2.dx)
                ip.x = topX(edge1, ip.y)
            else
                ip.x = topX(edge2, ip.y)
        }

        if (ip.y > edge1.curr.y)
        {
            ip.y = edge1.curr.y
            if Math.abs(edge1.dx) > Math.abs(edge2.dx)
                ip.x = topX(edge2, ip.y)
            else
                ip.x = topX(edge1, ip.y)
        }
    }

    method popLocalMinima(y: CInt, locMin: **LocalMinimum)->bool
    {
        if currentLM == minimaList.count or minimaList[currentLM].y != y return false
        dref locMin = minimaList[currentLM]
        currentLM += 1
        return true
    }

    func round(val: f64)->CInt
    {
        if val < 0 return cast(CInt) (val - 0.5)
        return cast(CInt) (val + 0.5)
    }

    func topX(edge: *Edge, currentY: CInt)->CInt
    {
        return currentY == edge.top.y ? edge.top.x : edge.bot.x + round(edge.dx * (currentY - edge.bot.y))
    }

    func e2InsertsBeforeE1(e1, e2: *Edge)->bool
    {
        if e2.curr.x == e1.curr.x
        {
            if e2.top.y > e1.top.y
                return e2.top.x < topX(e1, e2.top.y)
            else
                return e1.top.x > topX(e2, e1.top.y)
        }
        else
            return e2.curr.x < e1.curr.x
    }

    method insertEdgeIntoAEL(edge, startE: *Edge)
    {
        startEdge := startE
        if !activeEdges
        {
            edge.prevInAEL = null
            edge.nextInAEL = null
            activeEdges = edge
        }
        else if !startEdge and e2InsertsBeforeE1(activeEdges, edge)
        {
            edge.prevInAEL = null
            edge.nextInAEL = activeEdges
            activeEdges.prevInAEL = edge
            activeEdges = edge
        }
        else
        {
            if !startEdge startEdge = activeEdges
            while startEdge.nextInAEL and !e2InsertsBeforeE1(startEdge.nextInAEL, edge)
                startEdge = startEdge.nextInAEL
            edge.nextInAEL = startEdge.nextInAEL
            if startEdge.nextInAEL startEdge.nextInAEL.prevInAEL = edge
            edge.prevInAEL = startEdge
            startEdge.nextInAEL = edge
        }
    }

    method isEvenOddFillType(edge: *Edge)->bool
    {
        if edge.polyType == .Subject
            return subjFillType == .EvenOdd
        return clipFillType == .EvenOdd
    }

    method isEvenOddAltFillType(edge: *Edge)->bool
    {
        if edge.polyType == .Subject
            return clipFillType == .EvenOdd
        return subjFillType == .EvenOdd
    }

    method setWindingCount(edge: *Edge)
    {
        e := edge.prevInAEL

        while e and ((e.polyType != edge.polyType) or (e.windDelta == 0))
            e = e.prevInAEL

        if !e
        {
            if edge.windDelta == 0
            {
                pft := edge.polyType == .Subject ? subjFillType : clipFillType
                edge.windCnt = (pft == .Negative ? -1 : 1)
            }
            else
                edge.windCnt = edge.windDelta
            edge.windCnt2 = 0
            e = activeEdges
        }
        else if edge.windDelta == 0 and clipType != .Union
        {
            edge.windCnt = 1
            edge.windCnt2 = e.windCnt2
            e = e.nextInAEL
        }
        else if isEvenOddFillType(edge)
        {
            if edge.windDelta == 0
            {
                Inside := true
                e2 := e.prevInAEL
                while e2
                {
                    if e2.polyType == e.polyType and e2.windDelta != 0
                        Inside = !Inside
                    e2 = e2.prevInAEL
                }
                edge.windCnt = (Inside ? 0 : 1)
            }
            else
                edge.windCnt = edge.windDelta
            edge.windCnt2 = e.windCnt2
            e = e.nextInAEL
        }
        else
        {
            if e.windCnt * e.windDelta < 0
            {
                if Math.abs(e.windCnt) > 1
                {
                    if e.windDelta * edge.windDelta < 0
                        edge.windCnt = e.windCnt
                    else
                        edge.windCnt = e.windCnt + edge.windDelta
                }
                else
                    edge.windCnt = edge.windDelta == 0 ? 1 : edge.windDelta
            }
            else
            {
                if edge.windDelta == 0
                    edge.windCnt = (e.windCnt < 0 ? e.windCnt - 1 : e.windCnt + 1)
                else if e.windDelta * edge.windDelta < 0
                    edge.windCnt = e.windCnt
                else
                    edge.windCnt = e.windCnt + edge.windDelta
            }

            edge.windCnt2 = e.windCnt2
            e = e.nextInAEL
        }

        if isEvenOddAltFillType(edge)
        {
            while e != edge
            {
                if e.windDelta != 0
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

    method isContributing(edge: *Edge)->bool
    {
        var pft, pft2: PolyFillType = ?
        if edge.polyType == .Subject
        {
            pft = subjFillType
            pft2 = clipFillType
        }
        else
        {
            pft = clipFillType
            pft2 = subjFillType
        }

        switch pft
        {
        case EvenOdd:
            if edge.windDelta == 0 and edge.windCnt != 1
                return false
        case NonZero:
            if Math.abs(edge.windCnt) != 1
                return false
        case Positive:
            if edge.windCnt != 1
                return false
        default:
            if edge.windCnt != -1
                return false
        }

        switch clipType
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
                switch(pft2)
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

    method createOutRec()->*OutRec
    {
        result := Memory.new'OutRec()
        polyOuts.add(result)
        result.idx = cast(s32) (polyOuts.count - 1)
        return result
    }

    method setHoleState(e: *Edge, outrec: *OutRec)
    {
        e2 := e.prevInAEL
        var eTmp: *Edge
        while e2
        {
            if e2.outIdx >= 0 and e2.windDelta != 0
            {
                if !eTmp eTmp = e2
                else if eTmp.outIdx == e2.outIdx eTmp = null
            }

            e2 = e2.prevInAEL
        }
        if !eTmp
        {
            outrec.firstLeft = null
            outrec.isHole = false
        }
        else
        {
            outrec.firstLeft = polyOuts[eTmp.outIdx]
            outrec.isHole = !outrec.firstLeft.isHole
        }
    }

    #[Swag.Discardable]
    method addOutPt(e: *Edge, pt: IntPoint)->*OutPt
    {
        if e.outIdx < 0
        {
            outRec := createOutRec()
            outRec.isOpen = e.windDelta == 0
            newOp := Memory.new'OutPt();
            outRec.pts = newOp
            newOp.idx = outRec.idx
            newOp.pt = pt
            newOp.next = newOp
            newOp.prev = newOp
            if !outRec.isOpen
                setHoleState(e, outRec)
            e.outIdx = outRec.idx
            return newOp
        }
        else
        {
            outRec := polyOuts[e.outIdx]
            op := outRec.pts

            ToFront := (e.side == .Left)
            if ToFront and (pt == op.pt) return op
            if !ToFront and (pt == op.prev.pt) return op.prev

            newOp := Memory.new'OutPt()
            newOp.idx = outRec.idx
            newOp.pt = pt
            newOp.next = op
            newOp.prev = op.prev
            newOp.prev.next = newOp;
            op.prev = newOp
            if ToFront outRec.pts = newOp
            return newOp
        }
    }

    #[Swag.Discardable]
    method addLocalMinPoly(e1, e2: *Edge, pt: IntPoint)->*OutPt
    {
        var result: *OutPt
        var e, prevE: *Edge

        if isHorizontal(e2) or (e1.dx > e2.dx)
        {
            result = addOutPt(e1, pt)
            e2.outIdx = e1.outIdx
            e1.side = .Left
            e2.side = .Right
            e = e1
            if e.prevInAEL == e2
                prevE = e2.prevInAEL
            else
                prevE = e.prevInAEL
        }
        else
        {
            result = addOutPt(e2, pt)
            e1.outIdx = e2.outIdx
            e1.side = .Right
            e2.side = .Left
            e = e2
            if e.prevInAEL == e1
                prevE = e1.prevInAEL
            else
                prevE = e.prevInAEL
        }

        if prevE and prevE.outIdx >= 0 and prevE.top.y < pt.y and e.top.y < pt.y
        {
            xPrev := topX(prevE, pt.y)
            xE := topX(e, pt.y)
            if  xPrev == xE and
                (e.windDelta != 0) and
                (prevE.windDelta != 0) and
                slopesEqual(IntPoint{xPrev, pt.y}, prevE.top, IntPoint{xE, pt.y}, e.top)
            {
                outPt := addOutPt(prevE, pt)
                addJoin(result, outPt, e.top)
            }
        }

        return result
    }

    method addEdgeToSEL(edge: *Edge)
    {
        if !sortedEdges
        {
            sortedEdges = edge
            edge.prevInSEL = null
            edge.nextInSEL = null
        }
        else
        {
            edge.nextInSEL = sortedEdges
            edge.prevInSEL = null
            sortedEdges.prevInSEL = edge
            sortedEdges = edge
        }
    }

    func horzSegmentsOverlap(s1a, s1b, s2a, s2b: CInt)->bool
    {
        seg1a := s1a
        seg1b := s1b
        seg2a := s2a
        seg2b := s2b
        if seg1a > seg1b swap(&seg1a, &seg1b)
        if seg2a > seg2b swap(&seg2a, &seg2b)
        return (seg1a < seg2b) and (seg2a < seg1b)
    }

    func outRec1RightOfOutRec2(o1, outRec2: *OutRec)->bool
    {
        outRec1 := o1
        loop
        {
            outRec1 = outRec1.firstLeft
            if outRec1 == outRec2 return true
            if !outRec1 break
        }
        return false
    }

    func getDx(pt1, pt2: IntPoint)->f64
    {
        return (pt1.y == pt2.y) ? HorizontalVal : cast(f64) (pt2.x - pt1.x) / (pt2.y - pt1.y)
    }

    func area(op1: *OutPt)->f64
    {
        op := op1
        startOp := op
        if !op return 0

        a := 0'f64
        loop
        {
            a += cast(f64) (op.prev.pt.x + op.pt.x) * cast(f64) (op.prev.pt.y - op.pt.y)
            op = op.next
            if op == startOp break
        }

        return a * 0.5
    }

    func firstIsBottomPt(btmPt1, btmPt2: *OutPt)->bool
    {
        p := btmPt1.prev
        while (p.pt == btmPt1.pt) and (p != btmPt1) p = p.prev
        dx1p := Math.abs(getDx(btmPt1.pt, p.pt))
        p = btmPt1.next
        while (p.pt == btmPt1.pt) and (p != btmPt1) p = p.next
        dx1n := Math.abs(getDx(btmPt1.pt, p.pt))

        p = btmPt2.prev
        while (p.pt == btmPt2.pt) and (p != btmPt2) p = p.prev
        dx2p := Math.abs(getDx(btmPt2.pt, p.pt))
        p = btmPt2.next
        while (p.pt == btmPt2.pt) and (p != btmPt2) p = p.next
        dx2n := Math.abs(getDx(btmPt2.pt, p.pt))

        if Math.max(dx1p, dx1n) == Math.max(dx2p, dx2n) and
           Math.min(dx1p, dx1n) == Math.min(dx2p, dx2n)
            return area(btmPt1) > 0
        else
            return (dx1p >= dx2p and dx1p >= dx2n) or (dx1n >= dx2p and dx1n >= dx2n)
    }

    func getBottomPt(pp1: *OutPt)->*OutPt
    {
        var dups: *OutPt
        pp := pp1
        p := pp.next
        while p != pp
        {
            if p.pt.y > pp.pt.y
            {
                pp = p
                dups = null
            }
            else if p.pt.y == pp.pt.y and p.pt.x <= pp.pt.x
            {
                if p.pt.x < pp.pt.x
                {
                    dups = null
                    pp = p
                }
                else
                {
                    if p.next != pp and p.prev != pp dups = p
                }
            }
            p = p.next
        }

        if dups
        {
            while dups != p
            {
                if !firstIsBottomPt(p, dups) pp = dups
                dups = dups.next
                while dups.pt != pp.pt
                    dups = dups.next
            }
        }

        return pp
    }

    func getLowermostRec(outRec1, outRec2: *OutRec)->*OutRec
    {
        if !outRec1.bottomPt
            outRec1.bottomPt = getBottomPt(outRec1.pts)
        if !outRec2.bottomPt
            outRec2.bottomPt = getBottomPt(outRec2.pts)
        outPt1 := outRec1.bottomPt
        outPt2 := outRec2.bottomPt
        if outPt1.pt.y > outPt2.pt.y return outRec1
        if outPt1.pt.y < outPt2.pt.y return outRec2
        if outPt1.pt.x < outPt2.pt.x return outRec1
        if outPt1.pt.x > outPt2.pt.x return outRec2
        if outPt1.next == outPt1 return outRec2
        if outPt2.next == outPt2 return outRec1
        if firstIsBottomPt(outPt1, outPt2) return outRec1
        return outRec2
    }

    func reversePolyPtLinks(pp: *OutPt)
    {
        if !pp return
        var pp1, pp2: *OutPt
        pp1 = pp
        loop
        {
            pp2 = pp1.next
            pp1.next = pp1.prev
            pp1.prev = pp2
            pp1 = pp2
            if pp1 == pp break
        }
    }

    method appendPolygon(e1, e2: *Edge)
    {
        //get the start and ends of both output polygons ...
        outRec1 := polyOuts[e1.outIdx]
        outRec2 := polyOuts[e2.outIdx]

        var holeStateRec: *OutRec
        if outRec1RightOfOutRec2(outRec1, outRec2)
            holeStateRec = outRec2
        else if outRec1RightOfOutRec2(outRec2, outRec1)
            holeStateRec = outRec1
        else
            holeStateRec = getLowermostRec(outRec1, outRec2)

        //get the start and ends of both output polygons and
        //join e2 poly onto e1 poly and delete pointers to e2 ...

        p1_lft := outRec1.pts
        p1_rt := p1_lft.prev
        p2_lft := outRec2.pts
        p2_rt := p2_lft.prev

        //join e2 poly onto e1 poly and delete pointers to e2 ...
        if e1.side == .Left
        {
            if e2.side == .Left
            {
                reversePolyPtLinks(p2_lft)
                p2_lft.next = p1_lft
                p1_lft.prev = p2_lft
                p1_rt.next = p2_rt
                p2_rt.prev = p1_rt
                outRec1.pts = p2_rt
            }
            else
            {
                p2_rt.next = p1_lft
                p1_lft.prev = p2_rt
                p2_lft.prev = p1_rt
                p1_rt.next = p2_lft
                outRec1.pts = p2_lft
            }
        }
        else
        {
            if e2.side == .Right
            {
                reversePolyPtLinks(p2_lft)
                p1_rt.next = p2_rt
                p2_rt.prev = p1_rt
                p2_lft.next = p1_lft
                p1_lft.prev = p2_lft
            }
            else
            {
                p1_rt.next = p2_lft
                p2_lft.prev = p1_rt
                p1_lft.prev = p2_rt
                p2_rt.next = p1_lft
            }
        }

        outRec1.bottomPt = null
        if holeStateRec == outRec2
        {
            if outRec2.firstLeft != outRec1
                outRec1.firstLeft = outRec2.firstLeft
            outRec1.isHole = outRec2.isHole
        }

        outRec2.pts = null
        outRec2.bottomPt = null
        outRec2.firstLeft = outRec1

        OKIdx := e1.outIdx
        ObsoleteIdx := e2.outIdx

        e1.outIdx = Unassigned
        e2.outIdx = Unassigned

        e := activeEdges
        while e
        {
            if e.outIdx == ObsoleteIdx
            {
                e.outIdx = OKIdx
                e.side = e1.side
                break
            }

            e = e.nextInAEL
        }

        outRec2.idx = outRec1.idx
    }

    method addLocalMaxPoly(e1, e2: *Edge, pt: IntPoint)
    {
        addOutPt(e1, pt)
        if e2.windDelta == 0 addOutPt(e2, pt)

        if e1.outIdx == e2.outIdx
        {
            e1.outIdx = Unassigned
            e2.outIdx = Unassigned
        }
        else if e1.outIdx < e2.outIdx
            appendPolygon(e1, e2)
        else
            appendPolygon(e2, e1)
    }

    func swapSides(edge1, edge2: *Edge)
    {
        side := edge1.side
        edge1.side = edge2.side
        edge2.side = side
    }

    func swapPolyIndexes(edge1, edge2: *Edge)
    {
        outIdx := edge1.outIdx
        edge1.outIdx = edge2.outIdx
        edge2.outIdx = outIdx
    }

    method intersectEdges(e1, e2: *Edge, pt: IntPoint)
    {
        e1Contributing := e1.outIdx >= 0
        e2Contributing := e2.outIdx >= 0

        //update winding counts...
        //assumes that e1 will be to the right of e2 ABOVE the intersection
        if e1.polyType == e2.polyType
        {
            if isEvenOddFillType(e1)
            {
                oldE1WindCnt := e1.windCnt
                e1.windCnt = e2.windCnt
                e2.windCnt = oldE1WindCnt
            }
            else
            {
                if e1.windCnt + e2.windDelta == 0
                    e1.windCnt = -e1.windCnt
                else
                    e1.windCnt += e2.windDelta
                if e2.windCnt - e1.windDelta == 0
                    e2.windCnt = -e2.windCnt
                else
                    e2.windCnt -= e1.windDelta
            }
        }
        else
        {
            if !isEvenOddFillType(e2)
                e1.windCnt2 += e2.windDelta
            else
                e1.windCnt2 = (e1.windCnt2 == 0) ? 1 : 0
            if !isEvenOddFillType(e1)
                e2.windCnt2 -= e1.windDelta
            else
                e2.windCnt2 = (e2.windCnt2 == 0) ? 1 : 0
        }

        var e1FillType, e2FillType, e1FillType2, e2FillType2: PolyFillType = ?
        if e1.polyType == .Subject
        {
            e1FillType = subjFillType
            e1FillType2 = clipFillType
        }
        else
        {
            e1FillType = clipFillType
            e1FillType2 = subjFillType
        }

        if e2.polyType == .Subject
        {
            e2FillType = subjFillType
            e2FillType2 = clipFillType
        }
        else
        {
            e2FillType = clipFillType
            e2FillType2 = subjFillType
        }

        var e1Wc, e2Wc: CInt
        switch e1FillType
        {
        case Positive:   e1Wc = e1.windCnt
        case Negative:   e1Wc = -e1.windCnt
        default:            e1Wc = Math.abs(e1.windCnt)
        }

        switch e2FillType
        {
        case Positive:   e2Wc = e2.windCnt
        case Negative:   e2Wc = -e2.windCnt
        default:            e2Wc = Math.abs(e2.windCnt)
        }

        if e1Contributing and e2Contributing
        {
            if (e1Wc != 0 and e1Wc != 1) or (e2Wc != 0 and e2Wc != 1) or (e1.polyType != e2.polyType and clipType != .Xor)
            {
                addLocalMaxPoly(e1, e2, pt)
            }
            else
            {
                addOutPt(e1, pt)
                addOutPt(e2, pt)
                swapSides(e1, e2)
                swapPolyIndexes(e1, e2)
            }
        }
        else if e1Contributing
        {
            if e2Wc == 0 or e2Wc == 1
            {
                addOutPt(e1, pt)
                swapSides(e1, e2)
                swapPolyIndexes(e1, e2)
            }
        }
        else if e2Contributing
        {
            if e1Wc == 0 or e1Wc == 1
            {
                addOutPt(e2, pt);
                swapSides(e1, e2)
                swapPolyIndexes(e1, e2)
            }
        }
        else if (e1Wc == 0 or e1Wc == 1) and (e2Wc == 0 or e2Wc == 1)
        {
            var e1Wc2, e2Wc2: CInt
            switch e1FillType2
            {
            case Positive:   e1Wc2 = e1.windCnt2
            case Negative:   e1Wc2 = -e1.windCnt2
            default:            e1Wc2 = Math.abs(e1.windCnt2)
            }

            switch e2FillType2
            {
            case Positive:   e2Wc2 = e2.windCnt2
            case Negative:   e2Wc2 = -e2.windCnt2
            default:            e2Wc2 = Math.abs(e2.windCnt2)
            }

            if e1.polyType != e2.polyType
            {
                addLocalMinPoly(e1, e2, pt);
            }
            else if e1Wc == 1 and e2Wc == 1
            {
                switch clipType
                {
                case Intersection:
                    if e1Wc2 > 0 and e2Wc2 > 0
                        addLocalMinPoly(e1, e2, pt)
                case Union:
                    if e1Wc2 <= 0 and e2Wc2 <= 0
                        addLocalMinPoly(e1, e2, pt)
                case Difference:
                    if ((e1.polyType == .Clip) and (e1Wc2 > 0) and (e2Wc2 > 0)) or
                       ((e1.polyType == .Subject) and (e1Wc2 <= 0) and (e2Wc2 <= 0))
                        addLocalMinPoly(e1, e2, pt)
                case Xor:
                    addLocalMinPoly(e1, e2, pt)
                }
            }
            else
                swapSides(e1, e2)
        }
    }

    method insertLocalMinimaIntoAEL(botY: CInt)
    {
        var lm: *LocalMinimum
        while popLocalMinima(botY, &lm)
        {
            lb := lm.leftBound
            rb := lm.rightBound

            var Op1: *OutPt
            if !lb
            {
                //nb: don't insert LB into either AEL or SEL
                insertEdgeIntoAEL(rb, null)
                setWindingCount(rb)
                if isContributing(rb)
                    Op1 = addOutPt(rb, rb.bot)
            }
            else if (!rb)
            {
                insertEdgeIntoAEL(lb, null)
                setWindingCount(lb)
                if isContributing(lb)
                    Op1 = addOutPt(lb, lb.bot)
                insertScanbeam(lb.top.y)
            }
            else
            {
                insertEdgeIntoAEL(lb, null)
                insertEdgeIntoAEL(rb, lb)
                setWindingCount(lb)
                rb.windCnt = lb.windCnt
                rb.windCnt2 = lb.windCnt2
                if isContributing(lb)
                    Op1 = addLocalMinPoly(lb, rb, lb.bot)
                insertScanbeam(lb.top.y)
            }

            if rb
            {
                if isHorizontal(rb)
                {
                    addEdgeToSEL(rb)
                    if rb.nextInLML
                        insertScanbeam(rb.nextInLML.top.y)
                }
                else
                    insertScanbeam(rb.top.y)
            }

            if !lb or !rb continue

            //if any output polygons share an edge, they'll need joining later ...
            if Op1 and isHorizontal(rb) and ghostJoinList.count > 0 and (rb.windDelta != 0)
            {
                loop i: ghostJoinList.count
                {
                    jr := ghostJoinList[i]
                    if horzSegmentsOverlap(jr.outPt1.pt.x, jr.offPt.x, rb.bot.x, rb.top.x)
                        addJoin(jr.outPt1, Op1, jr.offPt)
                }
            }

            if  lb.outIdx >= 0 and lb.prevInAEL and
                lb.prevInAEL.curr.x == lb.bot.x and
                lb.prevInAEL.outIdx >= 0 and
                slopesEqual(lb.prevInAEL.bot, lb.prevInAEL.top, lb.curr, lb.top) and
                (lb.windDelta != 0) and (lb.prevInAEL.windDelta != 0)
            {
                Op2 := addOutPt(lb.prevInAEL, lb.bot)
                addJoin(Op1, Op2, lb.top)
            }

            if lb.nextInAEL != rb
            {
                if  rb.outIdx >= 0 and
                    rb.prevInAEL.outIdx >= 0 and
                    slopesEqual(rb.prevInAEL.curr, rb.prevInAEL.top, rb.curr, rb.top) and
                    (rb.windDelta != 0) and
                    (rb.prevInAEL.windDelta != 0)
                {
                    Op2 := addOutPt(rb.prevInAEL, rb.bot)
                    addJoin(Op1, Op2, rb.top)
                }

                e := lb.nextInAEL
                if e
                {
                    while e != rb
                    {
                        intersectEdges(rb, e, lb.curr)
                        e = e.nextInAEL
                    }
                }
            }
        }
    }

    func pointCount(pts: *OutPt)->s32
    {
        if !pts return 0
        result := 0
        p := pts
        loop
        {
            result += 1
            p = p.next
            if p == pts break
        }

        return result
    }

    method buildResult(polys: *ClipperPaths)
    {
        polys.reserve(polyOuts.count)
        loop i: polyOuts
        {
            if !polyOuts[i].pts continue

            var pg: ClipperPath

            p := polyOuts[i].pts.prev
            cnt := pointCount(p)
            if cnt < 2 continue

            pg.reserve(cast(uint) cnt)
            loop cnt
            {
                pg.add(p.pt)
                p = p.prev
            }

            polys.emplace(&pg)
        }
    }

    method disposeAllOutRecs()
    {
        loop i: polyOuts disposeOutRec(i)
        polyOuts.clear()
    }

    func disposeOutPts(pts: *OutPt)
    {
        pp := pts
        if pp == null return
        pp.prev.next = null
        while pp
        {
            tmpPp := pp
            pp = pp.next
            Memory.delete(tmpPp)
        }
    }

    method disposeOutRec(index: uint)
    {
        outRec := polyOuts[index]
        if outRec.pts disposeOutPts(outRec.pts)
        Memory.delete(outRec)
        polyOuts[index] = null
    }

    method getBounds()->IntRect
    {
        var result: retval

        lm := 0'uint
        if lm == minimaList.count
            return result

        result.left = minimaList[lm].leftBound.bot.x
        result.top = minimaList[lm].leftBound.bot.y
        result.right = minimaList[lm].leftBound.bot.x
        result.bottom = minimaList[lm].leftBound.bot.y

        while lm != minimaList.count
        {
            result.bottom = Math.max(result.bottom, minimaList[lm].leftBound.bot.y)
            e := minimaList[lm].leftBound
            loop
            {
                bottomE := e
                while e.nextInLML
                {
                    if (e.bot.x < result.left) result.left = e.bot.x
                    if (e.bot.x > result.right) result.right = e.bot.x
                    e = e.nextInLML
                }

                result.left = Math.min(result.left, e.bot.x)
                result.right = Math.max(result.right, e.bot.x)
                result.left = Math.min(result.left, e.top.x)
                result.right = Math.max(result.right, e.top.x)
                result.top = Math.min(result.top, e.top.y)
                if bottomE == minimaList[lm].leftBound
                    e = minimaList[lm].rightBound
                else
                    break
            }

            lm += 1
        }

        return result
    }
}
