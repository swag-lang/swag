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
using Core

struct IntPoint
{
    X, Y: CInt
}

impl IntPoint
{
    method opEquals(other: Self)->bool
    {
        return X == other.X and Y == other.Y
    }
}

const loRange = 0x7FFF
const hiRange = 0x7FFF
const Unassigned = -1
const Skip = -2
const HORIZONTAL = -1.0E+40

alias CInt  = s32
alias ClipperPath  = Array'IntPoint
alias ClipperPaths  = Array'ClipperPath

public enum PolyType        { ptSubject; ptClip; }
public enum ClipType        { ctIntersection; ctUnion; ctDifference; ctXor; }
public enum PolyFillType    { pftEvenOdd; pftNonZero; pftPositive; pftNegative; }
public enum InitOptions     { ioReverseSolution = 1; ioStrictlySimple = 2; ioPreserveCollinear = 4; }
public enum JoinType        { jtSquare; jtRound; jtMiter; }
public enum EndType         { etClosedPolygon; etClosedLine; etOpenButt; etOpenSquare; etOpenRound; }

enum EdgeSide { esLeft = 1; esRight = 2; }
enum Direction { dRightToLeft; dLeftToRight; }

struct IntersectNode
{
    Edge1:    *TEdge
    Edge2:    *TEdge
    Pt:       IntPoint
}

impl IntersectNode
{
    method opCmp(other: *IntersectNode)->s32
    {
        return Pt.Y <=> other.Pt.Y
    }
}

struct LocalMinimum
{
    Y:            CInt
    LeftBound:    *TEdge
    RightBound:   *TEdge
}

impl LocalMinimum
{
    method opCmp(other: LocalMinimum)->s32
    {
        return Y <=> other.Y
    }
}

struct TEdge
{
    Bot:          IntPoint
    Curr:         IntPoint
    Top:          IntPoint
    Dx:           f64
    PolyTyp:      PolyType = ?
    Side:         EdgeSide = ?
    WindDelta:    s32
    WindCnt:      s32
    WindCnt2:     s32
    OutIdx:       s32
    Next:         *TEdge
    Prev:         *TEdge
    NextInLML:    *TEdge
    NextInAEL:    *TEdge
    PrevInAEL:    *TEdge
    NextInSEL:    *TEdge
    PrevInSEL:    *TEdge
}

struct OutRec
{
    Idx:        s32
    IsHole:     bool
    IsOpen:     bool
    FirstLeft:  *OutRec
    PolyNd:     *PolyNode
    Pts:        *OutPt
    BottomPt:   *OutPt
}

struct OutPt
{
    Idx:      s32
    Pt:       IntPoint
    Next:     *OutPt
    Prev:     *OutPt
}

struct PolyNode
{
    Index:      u32
    m_IsOpen:   bool
    m_jointype: JoinType = ?
    m_endtype:  EndType = ?
}

struct Join
{
    OutPt1: *OutPt
    OutPt2: *OutPt
    OffPt:  IntPoint
}

#[Swag.opaque]
public struct Clipper
{
    m_UseFullRange:         bool
    m_PreserveCollinear:    bool
    m_UsingPolyTree:        bool
    m_StrictSimple:         bool
    m_ReverseOutput:        bool
    m_edges:                Array'*TEdge
    m_MinimaList:           Array'LocalMinimum
    m_PolyOuts:             Array'*OutRec
    m_Joins:                Array'*Join
    m_GhostJoins:           Array'*Join
    m_IntersectList:        Array'*IntersectNode
    m_Scanbeam:             Array'CInt
    m_Maxima:               Array'CInt
    m_ClipFillType:         PolyFillType = ?
    m_SubjFillType:         PolyFillType = ?
    m_ClipType:             ClipType = ?
    m_CurrentLM:            uint
    m_ActiveEdges:          *TEdge
    m_SortedEdges:          *TEdge
}

impl Clipper
{
    public method addPath(pg: *LinePath, polyType: PolyType) throw
    {
        if !pg.isClosed throw "Path not closed"
        if !pg.posBuffer.count throw "Path not flatten"

        var pt: ClipperPath
        visit p: pg.posBuffer
            pt.add(IntPoint{cast(s32) p.x, cast(s32) p.y})
        try addPath(&pt, polyType)
    }

    public method execute(clipType: ClipType, solution: *LinePathList, subjFillType, clipFillType: PolyFillType) throw
    {
        var res: ClipperPaths
        try Execute(clipType, &res, subjFillType, clipFillType)

        loop i: res
        {
            path := Memory.new'LinePath()
            visit *pt: dref res[i]
            {
                if @index == 0
                    path.start(cast(f32) pt.X, cast(f32) pt.Y)
                else
                    path.lineTo(cast(f32) pt.X, cast(f32) pt.Y)
            }

            solution.paths.add(path)
        }
    }

    method Execute(clipType: ClipType, solution: *ClipperPaths, subjFillType, clipFillType: PolyFillType) throw
    {
        m_SubjFillType  = subjFillType
        m_ClipFillType  = clipFillType
        m_ClipType      = clipType;
        m_UsingPolyTree = false

        defer DisposeAllOutRecs()
        try ExecuteInternal()

        BuildResult(solution)
    }

    method addPath(pg: *ClipperPath, polyType: PolyType) throw
    {
        highI := cast(int) pg.count - 1
        while highI > 0 and (dref pg[highI] == dref pg[0]) highI -= 1
        while highI > 0 and (dref pg[highI] == dref pg[highI -1]) highI -= 1
        if highI < 2 return

        edges  := Memory.new'TEdge(cast(uint) highI + 1)
        IsFlat := true

        // 1. Basic (first) edge initialization ...
        try
        {
            edges[1].Curr = pg.buffer[1]
            RangeTest(pg.buffer[0], &m_UseFullRange)
            RangeTest(pg.buffer[highI], &m_UseFullRange)
            InitEdge(&edges[0], &edges[1], &edges[highI], pg.buffer[0])
            InitEdge(&edges[highI], &edges[0], &edges[highI-1], pg.buffer[highI])
            loop i: highI-1 .. 1
            {
                RangeTest(pg.buffer[i], &m_UseFullRange)
                InitEdge(&edges[i], &edges[i+1], &edges[i-1], pg.buffer[i])
            }
        }

        eStart := &edges[0]

        //2. Remove duplicate vertices, and (when closed) collinear edges ...
        E, eLoopStop := eStart

        loop
        {
            //nb: allows matching start and end points when not Closed ...
            if E.Curr == E.Next.Curr
            {
                if E == E.Next break
                if E == eStart eStart = E.Next
                E = RemoveEdge(E)
                eLoopStop = E
                continue
            }

            //only two vertices
            if E.Prev == E.Next
                break
            else if SlopesEqual(E.Prev.Curr, E.Curr, E.Next.Curr) and
                    (!m_PreserveCollinear or !Pt2IsBetweenPt1AndPt3(E.Prev.Curr, E.Curr, E.Next.Curr))
            {
                //Collinear edges are allowed for open paths but in closed paths
                //the default is to merge adjacent collinear edges into a single edge.
                //However, if the PreserveCollinear property is enabled, only overlapping
                //collinear edges (ie spikes) will be removed from closed paths.
                if E == eStart eStart = E.Next
                E = RemoveEdge(E)
                E = E.Prev
                eLoopStop = E
                continue
            }

            E = E.Next
            if E == eLoopStop
                break
        }

        if E.Prev == E.Next
            throw "invalid path"

        //3. Do second stage of edge initialization ...
        E = eStart
        loop
        {
            InitEdge2(E, polyType)
            E = E.Next
            if IsFlat and E.Curr.Y != eStart.Curr.Y IsFlat = false
            if E == eStart break
        }

        //4. Finally, add edge bounds to LocalMinima list ...
        if IsFlat throw "Flat path"

        m_edges.add(edges);
        var leftBoundIsForward: bool
        var EMin: *TEdge

        //workaround to avoid an endless loop in the while loop below when
        //open paths have matching start and end points ...
        if E.Prev.Bot == E.Prev.Top E = E.Next

        loop
        {
            E = FindNextLocMin(E)
            if E == EMin break
            else if !EMin EMin = E

            //E and E.Prev now share a local minima (left aligned if horizontal).
            //Compare their slopes to find which starts which bound ...
            var locMin: LocalMinimum
            locMin.Y = E.Bot.Y
            if E.Dx < E.Prev.Dx
            {
                locMin.LeftBound = E.Prev
                locMin.RightBound = E
                leftBoundIsForward = false
            }
            else
            {
                locMin.LeftBound = E
                locMin.RightBound = E.Prev
                leftBoundIsForward = true
            }

            if locMin.LeftBound.Next == locMin.RightBound
                locMin.LeftBound.WindDelta = -1
            else
                locMin.LeftBound.WindDelta = 1
            locMin.RightBound.WindDelta = -locMin.LeftBound.WindDelta

            E = ProcessBound(locMin.LeftBound, leftBoundIsForward)
            if E.OutIdx == Skip E = ProcessBound(E, leftBoundIsForward)

            E2 := ProcessBound(locMin.RightBound, !leftBoundIsForward)
            if E2.OutIdx == Skip E2 = ProcessBound(E2, !leftBoundIsForward)

            if locMin.LeftBound.OutIdx == Skip
                locMin.LeftBound = null
            else if locMin.RightBound.OutIdx == Skip
                locMin.RightBound = null
            m_MinimaList.add(locMin)
            if !leftBoundIsForward E = E2
        }
    }

    func InitEdge(e, eNext, ePrev: *TEdge, pt: IntPoint)
    {
        Memory.clear(e, @sizeof(TEdge))
        e.Next = eNext
        e.Prev = ePrev
        e.Curr = pt
        e.OutIdx = Unassigned
    }

    func InitEdge2(e: *TEdge, Pt: PolyType)
    {
        if e.Curr.Y >= e.Next.Curr.Y
        {
            e.Bot = e.Curr
            e.Top = e.Next.Curr
        }
        else
        {
            e.Top = e.Curr;
            e.Bot = e.Next.Curr
        }

        SetDx(e)
        e.PolyTyp = Pt
    }

    func SetDx(e: *TEdge)
    {
        dy := (e.Top.Y - e.Bot.Y)
        if dy == 0 e.Dx = HORIZONTAL
        else e.Dx = cast(f64) (e.Top.X - e.Bot.X) / dy
    }

    func RemoveEdge(e: *TEdge)->*TEdge
    {
        e.Prev.Next = e.Next;
        e.Next.Prev = e.Prev;
        result := e.Next
        e.Prev = null
        return result
    }

    method AddJoin(op1, op2: *OutPt, OffPt: IntPoint)
    {
        j := Memory.new'Join()
        j.OutPt1 = op1
        j.OutPt2 = op2
        j.OffPt = OffPt
        m_Joins.add(j)
    }

    func SlopesEqual(e1: *TEdge, e2: *TEdge)->bool
    {
        return (e1.Top.Y - e1.Bot.Y) * (e2.Top.X - e2.Bot.X) == (e1.Top.X - e1.Bot.X) * (e2.Top.Y - e2.Bot.Y)
    }

    func SlopesEqual(pt1, pt2, pt3: IntPoint)->bool
    {
        return (pt1.Y-pt2.Y)*(pt2.X-pt3.X) == (pt1.X-pt2.X)*(pt2.Y-pt3.Y)
    }

    func SlopesEqual(pt1, pt2, pt3, pt4: IntPoint)->bool
    {
        return (pt1.Y-pt2.Y)*(pt3.X-pt4.X) == (pt1.X-pt2.X)*(pt3.Y-pt4.Y)
    }

    func Pt2IsBetweenPt1AndPt3(pt1, pt2, pt3: IntPoint)->bool
    {
        if (pt1 == pt3) or (pt1 == pt2) or (pt3 == pt2)
            return false
        if pt1.X != pt3.X
            return (pt2.X > pt1.X) == (pt2.X < pt3.X)
        return (pt2.Y > pt1.Y) == (pt2.Y < pt3.Y)
    }

    func RangeTest(Pt: IntPoint, useFullRange: *bool) throw
    {
        if dref useFullRange
        {
            if Pt.X > hiRange or Pt.Y > hiRange or -Pt.X > hiRange or -Pt.Y > hiRange
                throw "Coordinate outside allowed range"
        }
        else if Pt.X > loRange or Pt.Y > loRange or -Pt.X > loRange or -Pt.Y > loRange
        {
            dref useFullRange = true
            try RangeTest(Pt, useFullRange)
        }
    }

    func IsHorizontal(e: *TEdge)->bool
    {
        return e.Dx == HORIZONTAL
    }

    func FindNextLocMin(edge: *TEdge)->*TEdge
    {
        E := edge
        loop
        {
            while E.Bot != E.Prev.Bot or E.Curr == E.Top E = E.Next
            if !IsHorizontal(E) and !IsHorizontal(E.Prev) break
            while IsHorizontal(E.Prev) E = E.Prev
            E2 := E;
            while IsHorizontal(E) E = E.Next
            if E.Top.Y == E.Prev.Bot.Y continue
            if E2.Prev.Bot.X < E.Bot.X E = E2
            break
        }

        return E
    }

    func ReverseHorizontal(e: *TEdge)
    {
        swap(&e.Top.X, &e.Bot.X)
    }

    method ProcessBound(edge: *TEdge, NextIsForward: bool)->*TEdge
    {
        E := edge
        var Result: *TEdge = E
        var Horz: *TEdge

        if E.OutIdx == Skip
        {
            if NextIsForward
            {
                while E.Top.Y == E.Next.Bot.Y
                    E = E.Next
                while E != Result and IsHorizontal(E)
                    E = E.Prev
            }
            else
            {
                while E.Top.Y == E.Prev.Bot.Y
                    E = E.Prev
                while E != Result and IsHorizontal(E)
                    E = E.Next
            }

            if E == Result
            {
                if NextIsForward
                    Result = E.Next
                else
                    Result = E.Prev
            }
            else
            {
                if NextIsForward
                    E = Result.Next
                else
                    E = Result.Prev
                var locMin: LocalMinimum
                locMin.Y = E.Bot.Y
                locMin.LeftBound = null
                locMin.RightBound = E
                E.WindDelta = 0
                Result = ProcessBound(E, NextIsForward)
                m_MinimaList.add(locMin)
            }

            return Result
        }

        var EStart: *TEdge
        if IsHorizontal(E)
        {
            if NextIsForward
                EStart = E.Prev
            else
                EStart = E.Next
            if IsHorizontal(EStart)
            {
                if EStart.Bot.X != E.Bot.X and EStart.Top.X != E.Bot.X
                    ReverseHorizontal(E)
            }
            else if EStart.Bot.X != E.Bot.X
                ReverseHorizontal(E)
        }

        EStart = E
        if NextIsForward
        {
            while Result.Top.Y == Result.Next.Bot.Y and Result.Next.OutIdx != Skip
                Result = Result.Next
            if IsHorizontal(Result) and Result.Next.OutIdx != Skip
            {
                Horz = Result
                while IsHorizontal(Horz.Prev) Horz = Horz.Prev
                if Horz.Prev.Top.X > Result.Next.Top.X Result = Horz.Prev
            }

            while E != Result
            {
                E.NextInLML = E.Next
                if IsHorizontal(E) and E != EStart and E.Bot.X != E.Prev.Top.X
                    ReverseHorizontal(E)
                E = E.Next
            }

            if IsHorizontal(E) and E != EStart and E.Bot.X != E.Prev.Top.X
                ReverseHorizontal(E)
            Result = Result.Next
        }
        else
        {
            while Result.Top.Y == Result.Prev.Bot.Y and Result.Prev.OutIdx != Skip
                Result = Result.Prev
            if IsHorizontal(Result) and Result.Prev.OutIdx != Skip
            {
                Horz = Result
                while IsHorizontal(Horz.Next) Horz = Horz.Next
                if Horz.Next.Top.X == Result.Prev.Top.X or Horz.Next.Top.X > Result.Prev.Top.X
                    Result = Horz.Next
            }

            while E != Result
            {
                E.NextInLML = E.Prev
                if IsHorizontal(E) and E != EStart and E.Bot.X != E.Next.Top.X
                    ReverseHorizontal(E);
                E = E.Prev
            }

            if IsHorizontal(E) and E != EStart and E.Bot.X != E.Next.Top.X
                ReverseHorizontal(E)
            Result = Result.Prev
        }

        return Result
    }

    method PopScanbeam(res: *CInt)->bool
    {
        if !m_Scanbeam.count
            return false

        Y := m_Scanbeam.popBack()
        dref res = Y
        while m_Scanbeam.count and Y == m_Scanbeam.back()
            m_Scanbeam.count -= 1

        return true
    }

    method InsertScanbeam(Y: CInt)
    {
        m_Scanbeam.add(Y)
        m_Scanbeam.sort()
    }

    method Reset()
    {
        m_CurrentLM = 0
        if m_CurrentLM == m_MinimaList.count return
        m_MinimaList.sort()

        m_Scanbeam.clear()
        visit lm: m_MinimaList
        {
            InsertScanbeam(lm.Y)
            e := lm.LeftBound
            if e
            {
                e.Curr = e.Bot
                e.Side = .esLeft
                e.OutIdx = Unassigned
            }

            e = lm.RightBound
            if e
            {
                e.Curr = e.Bot
                e.Side = .esRight
                e.OutIdx = Unassigned
            }
        }

        m_ActiveEdges = null
        m_CurrentLM = 0
    }

    method LocalMinimaPending()->bool
    {
        return m_CurrentLM != m_MinimaList.count
    }

    method DeleteFromSEL(e: *TEdge)
    {
        SelPrev := e.PrevInSEL
        SelNext := e.NextInSEL
        if !SelPrev and !SelNext and (e != m_SortedEdges) return
        if SelPrev
            SelPrev.NextInSEL = SelNext
        else
            m_SortedEdges = SelNext
        if SelNext
            SelNext.PrevInSEL = SelPrev
        e.NextInSEL = null
        e.PrevInSEL = null
    }

    method PopEdgeFromSEL(edge: **TEdge)->bool
    {
        if !m_SortedEdges return false
        dref edge = m_SortedEdges
        DeleteFromSEL(m_SortedEdges)
        return true
    }

    method ProcessHorizontals()
    {
        var horzEdge: *TEdge
        while PopEdgeFromSEL(&horzEdge)
            ProcessHorizontal(horzEdge)
    }

    func GetHorzDirection(HorzEdge: *TEdge, Dir: *Direction, Left, Right: *CInt)
    {
        if HorzEdge.Bot.X < HorzEdge.Top.X
        {
            dref Left = HorzEdge.Bot.X
            dref Right = HorzEdge.Top.X
            dref Dir = .dLeftToRight
        }
        else
        {
            dref Left = HorzEdge.Top.X
            dref Right = HorzEdge.Bot.X
            dref Dir = .dRightToLeft
        }
    }

    func GetMaximaPair(e: *TEdge)->*TEdge
    {
        if (e.Next.Top == e.Top) and !e.Next.NextInLML
            return e.Next
        else if (e.Prev.Top == e.Top) and !e.Prev.NextInLML
            return e.Prev
        else
            return null
    }

    func GetNextInAEL(e: *TEdge, dir: Direction)->*TEdge
    {
        return dir == .dLeftToRight ? e.NextInAEL : e.PrevInAEL
    }

    method GetLastOutPt(e: *TEdge)->*OutPt
    {
        outRec := m_PolyOuts[e.OutIdx]
        if e.Side == .esLeft
            return outRec.Pts
        else
            return outRec.Pts.Prev
    }

    method AddGhostJoin(op: *OutPt, OffPt: IntPoint)
    {
        j := Memory.new'Join()
        j.OutPt1 = op
        j.OutPt2 = null
        j.OffPt = OffPt
        m_GhostJoins.add(j)
    }

    method ProcessHorizontal(hE: *TEdge)
    {
        horzEdge := hE

        var dir: Direction = ?
        var horzLeft, horzRight: CInt

        IsOpen := (horzEdge.WindDelta == 0)
        GetHorzDirection(horzEdge, &dir, &horzLeft, &horzRight)

        eLastHorz := horzEdge
        var eMaxPair: *TEdge
        while eLastHorz.NextInLML and IsHorizontal(eLastHorz.NextInLML)
            eLastHorz = eLastHorz.NextInLML
        if !eLastHorz.NextInLML
            eMaxPair = GetMaximaPair(eLastHorz)

        var maxIt, maxRit: uint
        if m_Maxima.count > 0
        {
            if dir == .dLeftToRight
            {
                maxIt = 0
                while maxIt != m_Maxima.count and m_Maxima.buffer[maxIt] <= horzEdge.Bot.X
                    maxIt += 1
                if maxIt != m_Maxima.count and m_Maxima.buffer[maxIt] >= eLastHorz.Top.X
                    maxIt = m_Maxima.count
            }
            else
            {
                maxRit = m_Maxima.count -,safe 1
                while maxRit != Swag.UInt.Max and m_Maxima.buffer[maxRit] > horzEdge.Bot.X
                    maxRit = maxRit -,safe 1
                if maxRit != Swag.UInt.Max and m_Maxima.buffer[maxRit] <= eLastHorz.Top.X
                    maxRit = Swag.UInt.Max
            }
        }

        var op1: *OutPt
        loop
        {
            IsLastHorz := (horzEdge == eLastHorz)
            e := GetNextInAEL(horzEdge, dir)
            while e
            {
                if m_Maxima.count > 0
                {
                    if dir == .dLeftToRight
                    {
                        while maxIt != m_Maxima.count and m_Maxima.buffer[maxIt] < e.Curr.X
                        {
                            if horzEdge.OutIdx >= 0 and !IsOpen
                                AddOutPt(horzEdge, IntPoint{m_Maxima.buffer[maxIt], horzEdge.Bot.Y})
                            maxIt += 1
                        }
                    }
                    else
                    {
                        while maxRit != Swag.UInt.Max and m_Maxima.buffer[maxRit] > e.Curr.X
                        {
                            if horzEdge.OutIdx >= 0 and !IsOpen
                                AddOutPt(horzEdge, IntPoint{m_Maxima.buffer[maxRit], horzEdge.Bot.Y})
                            maxRit = maxRit -,safe 1
                        }
                    }
                }

                if (dir == .dLeftToRight and e.Curr.X > horzRight) or (dir == .dRightToLeft and e.Curr.X < horzLeft)
                    break
                if e.Curr.X == horzEdge.Top.X and horzEdge.NextInLML and e.Dx < horzEdge.NextInLML.Dx
                    break

                if horzEdge.OutIdx >= 0 and !IsOpen
                {
                    op1 = AddOutPt(horzEdge, e.Curr)
                    eNextHorz := m_SortedEdges
                    while eNextHorz
                    {
                        if eNextHorz.OutIdx >= 0 and
                           HorzSegmentsOverlap(horzEdge.Bot.X, horzEdge.Top.X, eNextHorz.Bot.X, eNextHorz.Top.X)
                        {
                            op2 := GetLastOutPt(eNextHorz)
                            AddJoin(op2, op1, eNextHorz.Top)
                        }

                        eNextHorz = eNextHorz.NextInSEL
                    }

                    AddGhostJoin(op1, horzEdge.Bot)
                }

                if e == eMaxPair and IsLastHorz
                {
                    if horzEdge.OutIdx >= 0
                        AddLocalMaxPoly(horzEdge, eMaxPair, horzEdge.Top)
                    DeleteFromAEL(horzEdge)
                    DeleteFromAEL(eMaxPair)
                    return
                }

                if dir == .dLeftToRight
                {
                    Pt := IntPoint{e.Curr.X, horzEdge.Curr.Y}
                    IntersectEdges(horzEdge, e, Pt)
                }
                else
                {
                    Pt := IntPoint{e.Curr.X, horzEdge.Curr.Y}
                    IntersectEdges(e, horzEdge, Pt)
                }

                eNext := GetNextInAEL(e, dir)
                SwapPositionsInAEL(horzEdge, e)
                e = eNext
            }

            if !horzEdge.NextInLML or !IsHorizontal(horzEdge.NextInLML) break
            UpdateEdgeIntoAEL(&horzEdge)
            if horzEdge.OutIdx >= 0 AddOutPt(horzEdge, horzEdge.Bot)
            GetHorzDirection(horzEdge, &dir, &horzLeft, &horzRight)
        }

        if horzEdge.OutIdx >= 0 and !op1
        {
            op1 = GetLastOutPt(horzEdge)
            eNextHorz := m_SortedEdges
            while eNextHorz
            {
                if eNextHorz.OutIdx >= 0 and
                   HorzSegmentsOverlap(horzEdge.Bot.X, horzEdge.Top.X, eNextHorz.Bot.X, eNextHorz.Top.X)
                {
                    op2 := GetLastOutPt(eNextHorz)
                    AddJoin(op2, op1, eNextHorz.Top)
                }

                eNextHorz = eNextHorz.NextInSEL
            }

            AddGhostJoin(op1, horzEdge.Top)
        }

        if horzEdge.NextInLML
        {
            if horzEdge.OutIdx >= 0
            {
                op1 = AddOutPt(horzEdge, horzEdge.Top)
                UpdateEdgeIntoAEL(&horzEdge)
                if horzEdge.WindDelta == 0 return
                ePrev := horzEdge.PrevInAEL
                eNext := horzEdge.NextInAEL
                if ePrev and ePrev.Curr.X == horzEdge.Bot.X and
                   ePrev.Curr.Y == horzEdge.Bot.Y and ePrev.WindDelta != 0 and
                   ePrev.OutIdx >= 0 and ePrev.Curr.Y > ePrev.Top.Y and SlopesEqual(horzEdge, ePrev)
                {
                    op2 := AddOutPt(ePrev, horzEdge.Bot)
                    AddJoin(op1, op2, horzEdge.Top)
                }
                else if eNext and eNext.Curr.X == horzEdge.Bot.X and
                        eNext.Curr.Y == horzEdge.Bot.Y and eNext.WindDelta != 0 and
                        eNext.OutIdx >= 0 and eNext.Curr.Y > eNext.Top.Y and SlopesEqual(horzEdge, eNext)
                {
                    op2 := AddOutPt(eNext, horzEdge.Bot)
                    AddJoin(op1, op2, horzEdge.Top)
                }
            }
            else
                UpdateEdgeIntoAEL(&horzEdge)
        }
        else
        {
            if horzEdge.OutIdx >= 0 AddOutPt(horzEdge, horzEdge.Top)
            DeleteFromAEL(horzEdge)
        }
    }

    method UpdateEdgeIntoAEL(ed: **TEdge)
    {
        e := dref ed
        e.NextInLML.OutIdx = e.OutIdx
        AelPrev := e.PrevInAEL
        AelNext := e.NextInAEL
        if AelPrev AelPrev.NextInAEL = e.NextInLML
        else m_ActiveEdges = e.NextInLML
        if AelNext AelNext.PrevInAEL = e.NextInLML
        e.NextInLML.Side = e.Side
        e.NextInLML.WindDelta = e.WindDelta
        e.NextInLML.WindCnt = e.WindCnt
        e.NextInLML.WindCnt2 = e.WindCnt2
        e = e.NextInLML
        e.Curr = e.Bot
        e.PrevInAEL = AelPrev
        e.NextInAEL = AelNext
        if !IsHorizontal(e) InsertScanbeam(e.Top.Y)
        dref ed = e
    }

    method SwapPositionsInAEL(Edge1, Edge2: *TEdge)
    {
        if Edge1.NextInAEL == Edge1.PrevInAEL or Edge2.NextInAEL == Edge2.PrevInAEL
            return

        if Edge1.NextInAEL == Edge2
        {
            Next := Edge2.NextInAEL;
            if Next Next.PrevInAEL = Edge1
            Prev := Edge1.PrevInAEL
            if Prev Prev.NextInAEL = Edge2
            Edge2.PrevInAEL = Prev
            Edge2.NextInAEL = Edge1
            Edge1.PrevInAEL = Edge2
            Edge1.NextInAEL = Next
        }
        else if Edge2.NextInAEL == Edge1
        {
            Next := Edge1.NextInAEL;
            if Next Next.PrevInAEL = Edge2
            Prev := Edge2.PrevInAEL
            if Prev Prev.NextInAEL = Edge1
            Edge1.PrevInAEL = Prev
            Edge1.NextInAEL = Edge2
            Edge2.PrevInAEL = Edge1
            Edge2.NextInAEL = Next
        }
        else
        {
            Next := Edge1.NextInAEL
            Prev := Edge1.PrevInAEL
            Edge1.NextInAEL = Edge2.NextInAEL
            if Edge1.NextInAEL Edge1.NextInAEL.PrevInAEL = Edge1
            Edge1.PrevInAEL = Edge2.PrevInAEL
            if Edge1.PrevInAEL Edge1.PrevInAEL.NextInAEL = Edge1
            Edge2.NextInAEL = Next
            if Edge2.NextInAEL Edge2.NextInAEL.PrevInAEL = Edge2
            Edge2.PrevInAEL = Prev
            if Edge2.PrevInAEL Edge2.PrevInAEL.NextInAEL = Edge2
        }

        if !Edge1.PrevInAEL m_ActiveEdges = Edge1
        else if !Edge2.PrevInAEL m_ActiveEdges = Edge2
    }

    method DeleteFromAEL(e: *TEdge)
    {
        AelPrev := e.PrevInAEL
        AelNext := e.NextInAEL
        if !AelPrev and !AelNext and (e != m_ActiveEdges)
            return
        if AelPrev
            AelPrev.NextInAEL = AelNext
        else
            m_ActiveEdges = AelNext
        if AelNext
            AelNext.PrevInAEL = AelPrev
        e.NextInAEL = null
        e.PrevInAEL = null
    }

    method ClearJoins()
    {
        visit p: m_Joins
            Memory.delete(p)
        m_Joins.clear()
    }

    method ClearGhostJoins()
    {
        visit p: m_GhostJoins
            Memory.delete(p)
        m_GhostJoins.clear()
    }

    method ExecuteInternal() throw
    {
        Reset()
        m_Maxima.clear()
        m_SortedEdges = null

        var botY, topY: CInt
        if !PopScanbeam(&botY) throw "Error"

        InsertLocalMinimaIntoAEL(botY)
        while PopScanbeam(&topY) or LocalMinimaPending()
        {
            ProcessHorizontals()
            ClearGhostJoins()
            try ProcessIntersections(topY)
            ProcessEdgesAtTopOfScanbeam(topY)
            botY = topY
            InsertLocalMinimaIntoAEL(botY)
        }

        //fix orientations ...
        visit outRec: m_PolyOuts
        {
            if !outRec.Pts or outRec.IsOpen continue
            if (outRec.IsHole ^ m_ReverseOutput) == (Area(outRec.Pts) > 0)
                ReversePolyPtLinks(outRec.Pts)
        }

        if m_Joins.count JoinCommonEdges()

        //unfortunately FixupOutPolygon() must be done after JoinCommonEdges()
        visit outRec: m_PolyOuts
        {
            if !outRec.Pts continue
            if outRec.IsOpen
                FixupOutPolyline(outRec)
            else
                FixupOutPolygon(outRec)
        }

        if m_StrictSimple DoSimplePolygons()

        ClearJoins()
        ClearGhostJoins()
    }

    method DoSimplePolygons()
    {
        var i: uint
        while i < m_PolyOuts.count
        {
            outrec := m_PolyOuts[i]
            i += 1
            op := outrec.Pts
            if !op or outrec.IsOpen continue

            loop
            {
                op2 := op.Next
                while op2 != outrec.Pts
                {
                    if op.Pt == op2.Pt and op2.Next != op and op2.Prev != op
                    {
                        op3 := op.Prev
                        op4 := op2.Prev
                        op.Prev = op4
                        op4.Next = op
                        op2.Prev = op3
                        op3.Next = op2

                        outrec.Pts = op
                        outrec2 := CreateOutRec()
                        outrec2.Pts = op2
                        UpdateOutPtIdxs(outrec2)
                        if Poly2ContainsPoly1(outrec2.Pts, outrec.Pts)
                        {
                            outrec2.IsHole = !outrec.IsHole
                            outrec2.FirstLeft = outrec
                            if m_UsingPolyTree FixupFirstLefts2(outrec2, outrec)
                        }
                        else if Poly2ContainsPoly1(outrec.Pts, outrec2.Pts)
                        {
                            outrec2.IsHole = outrec.IsHole
                            outrec.IsHole = !outrec2.IsHole
                            outrec2.FirstLeft = outrec.FirstLeft
                            outrec.FirstLeft = outrec2
                            if m_UsingPolyTree FixupFirstLefts2(outrec, outrec2)
                        }
                        else
                        {
                            outrec2.IsHole = outrec.IsHole
                            outrec2.FirstLeft = outrec.FirstLeft
                            if m_UsingPolyTree FixupFirstLefts1(outrec, outrec2)
                        }

                        op2 = op
                    }

                    op2 = op2.Next
                }

                op = op.Next
                if op == outrec.Pts break
            }
        }
    }


    func FixupOutPolyline(outrec: *OutRec)
    {
        pp := outrec.Pts
        lastPP := pp.Prev
        while pp != lastPP
        {
            pp = pp.Next
            if pp.Pt == pp.Prev.Pt
            {
                if pp == lastPP lastPP = pp.Prev
                tmpPP := pp.Prev
                tmpPP.Next = pp.Next
                pp.Next.Prev = tmpPP
                Memory.delete(pp)
                pp = tmpPP
            }
        }

        if pp == pp.Prev
        {
            DisposeOutPts(pp)
            outrec.Pts = null
            return
        }
    }

    method FixupOutPolygon(outrec: *OutRec)
    {
        var lastOK: *OutPt
        outrec.BottomPt = null
        pp := outrec.Pts
        preserveCol := m_PreserveCollinear or m_StrictSimple

        loop
        {
            if pp.Prev == pp or pp.Prev == pp.Next
            {
                DisposeOutPts(pp)
                outrec.Pts = null
                return
            }

            if (pp.Pt == pp.Next.Pt) or (pp.Pt == pp.Prev.Pt) or
                (SlopesEqual(pp.Prev.Pt, pp.Pt, pp.Next.Pt) and
                (!preserveCol or !Pt2IsBetweenPt1AndPt3(pp.Prev.Pt, pp.Pt, pp.Next.Pt)))
            {
                lastOK = null
                tmp := pp
                pp.Prev.Next = pp.Next
                pp.Next.Prev = pp.Prev
                pp = pp.Prev
                Memory.delete(tmp)
            }
            else if pp == lastOK
                break
            else
            {
                if !lastOK lastOK = pp
                pp = pp.Next
            }
        }

        outrec.Pts = pp
    }

    method GetOutRec(Idx: s32)->*OutRec
    {
        outrec := m_PolyOuts[Idx]
        while outrec != m_PolyOuts[outrec.Idx]
            outrec = m_PolyOuts[outrec.Idx]
        return outrec
    }

    method JoinPoints(j: *Join, outRec1, outRec2: *OutRec)->bool
    {
        return true
    }

    func UpdateOutPtIdxs(outrec: *OutRec)
    {
        op := outrec.Pts
        loop
        {
            op.Idx = outrec.Idx
            op = op.Prev
            if op == outrec.Pts break
        }
    }

    func PointInPolygon(pt: IntPoint, outPoints: *OutPt)->s32
    {
        op := outPoints
        result := 0
        startOp := op
        loop
        {
            if op.Next.Pt.Y == pt.Y
            {
                if (op.Next.Pt.X == pt.X) or (op.Pt.Y == pt.Y and ((op.Next.Pt.X > pt.X) == (op.Pt.X < pt.X)))
                    return -1
            }

            if (op.Pt.Y < pt.Y) != (op.Next.Pt.Y < pt.Y)
            {
                if op.Pt.X >= pt.X
                {
                    if op.Next.Pt.X > pt.X
                        result = 1 - result
                    else
                    {
                        d := cast(f64) (op.Pt.X - pt.X) * (op.Next.Pt.Y - pt.Y) - cast(f64) (op.Next.Pt.X - pt.X) * (op.Pt.Y - pt.Y)
                        if d == 0 return -1
                        if (d > 0) == (op.Next.Pt.Y > op.Pt.Y) result = 1 - result
                    }
                }
                else
                {
                    if op.Next.Pt.X > pt.X
                    {
                        d := cast(f64) (op.Pt.X - pt.X) * (op.Next.Pt.Y - pt.Y) - cast(f64) (op.Next.Pt.X - pt.X) * (op.Pt.Y - pt.Y)
                        if d == 0 return -1
                        if (d > 0) == (op.Next.Pt.Y > op.Pt.Y) result = 1 - result
                    }
                }
            }

            op = op.Next
            if startOp == op break
        }

        return result
    }

    func Poly2ContainsPoly1(OutPt1, OutPt2: *OutPt)->bool
    {
        op := OutPt1
        loop
        {
            res := PointInPolygon(op.Pt, OutPt2)
            if res >= 0 return res > 0
            op = op.Next
            if op == OutPt1 break
        }

        return true
    }

    func ParseFirstLeft(FL: *OutRec)->*OutRec
    {
        FirstLeft := FL
        while FirstLeft and !FirstLeft.Pts
            FirstLeft = FirstLeft.FirstLeft
        return FirstLeft
    }

    method FixupFirstLefts2(InnerOutRec, OuterOutRec: *OutRec)
    {
        orfl := OuterOutRec.FirstLeft
        visit outRec: m_PolyOuts
        {
            if !outRec.Pts or outRec == OuterOutRec or outRec == InnerOutRec
                continue
            firstLeft := ParseFirstLeft(outRec.FirstLeft)
            if firstLeft != orfl and firstLeft != InnerOutRec and firstLeft != OuterOutRec
                continue
            if Poly2ContainsPoly1(outRec.Pts, InnerOutRec.Pts)
                outRec.FirstLeft = InnerOutRec
            else if Poly2ContainsPoly1(outRec.Pts, OuterOutRec.Pts)
                outRec.FirstLeft = OuterOutRec
            else if outRec.FirstLeft == InnerOutRec or outRec.FirstLeft == OuterOutRec
                outRec.FirstLeft = orfl
        }
    }

    method FixupFirstLefts1(OldOutRec, NewOutRec: *OutRec)
    {
        visit outRec: m_PolyOuts
        {
            firstLeft := ParseFirstLeft(outRec.FirstLeft)
            if outRec.Pts and firstLeft == OldOutRec
            {
                if Poly2ContainsPoly1(outRec.Pts, NewOutRec.Pts)
                    outRec.FirstLeft = NewOutRec
            }
        }
    }

    method FixupFirstLefts3(OldOutRec, NewOutRec: *OutRec)
    {
        visit outRec: m_PolyOuts
        {
            firstLeft := ParseFirstLeft(outRec.FirstLeft)
            if outRec.Pts and firstLeft == OldOutRec
                outRec.FirstLeft = NewOutRec
        }
    }

    method JoinCommonEdges()
    {
        visit join: m_Joins
        {
            outRec1 := GetOutRec(join.OutPt1.Idx)
            outRec2 := GetOutRec(join.OutPt2.Idx)

            if !outRec1.Pts or !outRec2.Pts continue
            if outRec1.IsOpen or outRec2.IsOpen continue

            var holeStateRec: *OutRec
            if outRec1 == outRec2 holeStateRec = outRec1
            else if OutRec1RightOfOutRec2(outRec1, outRec2) holeStateRec = outRec2
            else if OutRec1RightOfOutRec2(outRec2, outRec1) holeStateRec = outRec1
            else holeStateRec = GetLowermostRec(outRec1, outRec2)

            if !JoinPoints(join, outRec1, outRec2) continue

            if (outRec1 == outRec2)
            {
                outRec1.Pts = join.OutPt1
                outRec1.BottomPt = null
                outRec2 = CreateOutRec()
                outRec2.Pts = join.OutPt2
                UpdateOutPtIdxs(outRec2)

                if Poly2ContainsPoly1(outRec2.Pts, outRec1.Pts)
                {
                    outRec2.IsHole = !outRec1.IsHole
                    outRec2.FirstLeft = outRec1
                    if m_UsingPolyTree FixupFirstLefts2(outRec2, outRec1)
                    if (outRec2.IsHole ^ m_ReverseOutput) == (Area(outRec2.Pts) > 0)
                    ReversePolyPtLinks(outRec2.Pts)

                }
                else if Poly2ContainsPoly1(outRec1.Pts, outRec2.Pts)
                {
                    outRec2.IsHole = outRec1.IsHole
                    outRec1.IsHole = !outRec2.IsHole
                    outRec2.FirstLeft = outRec1.FirstLeft
                    outRec1.FirstLeft = outRec2

                    if m_UsingPolyTree FixupFirstLefts2(outRec1, outRec2)

                    if (outRec1.IsHole ^ m_ReverseOutput) == (Area(outRec1.Pts) > 0)
                    ReversePolyPtLinks(outRec1.Pts)
                }
                else
                {
                    outRec2.IsHole = outRec1.IsHole
                    outRec2.FirstLeft = outRec1.FirstLeft
                    if m_UsingPolyTree FixupFirstLefts1(outRec1, outRec2)
                }

            }
            else
            {
                outRec2.Pts = null
                outRec2.BottomPt = null
                outRec2.Idx = outRec1.Idx

                outRec1.IsHole = holeStateRec.IsHole
                if holeStateRec == outRec2
                    outRec1.FirstLeft = outRec2.FirstLeft
                outRec2.FirstLeft = outRec1

                if m_UsingPolyTree FixupFirstLefts3(outRec2, outRec1)
            }
        }
    }

    func IsMaxima(e: *TEdge, Y: CInt)->bool
    {
        return e and e.Top.Y == Y and !e.NextInLML
    }

    func GetMaximaPairEx(e: *TEdge)->*TEdge
    {
        result := GetMaximaPair(e)
        if result and (result.OutIdx == Skip or
           (result.NextInAEL == result.PrevInAEL and !IsHorizontal(result)))
            return null
        return result
    }

    func IsIntermediate(e: *TEdge, Y: CInt)->bool
    {
        return e.Top.Y == Y and e.NextInLML
    }

    method DoMaxima(e: *TEdge)
    {
        eMaxPair := GetMaximaPairEx(e)
        if !eMaxPair
        {
            if e.OutIdx >= 0
                AddOutPt(e, e.Top)
            DeleteFromAEL(e)
            return;
        }

        eNext := e.NextInAEL
        while eNext and eNext != eMaxPair
        {
            IntersectEdges(e, eNext, e.Top)
            SwapPositionsInAEL(e, eNext)
            eNext = e.NextInAEL
        }

        if e.OutIdx == Unassigned and eMaxPair.OutIdx == Unassigned
        {
            DeleteFromAEL(e)
            DeleteFromAEL(eMaxPair)
        }
        else if e.OutIdx >= 0 and eMaxPair.OutIdx >= 0
        {
            if e.OutIdx >= 0 AddLocalMaxPoly(e, eMaxPair, e.Top)
            DeleteFromAEL(e)
            DeleteFromAEL(eMaxPair)
        }
        else
        {
            Debug.assert(false)
        }
    }

    method ProcessEdgesAtTopOfScanbeam(topY: CInt)
    {
        e := m_ActiveEdges
        while e
        {
            //1. process maxima, treating them as if they're 'bent' horizontal edges,
            //   but exclude maxima with horizontal edges. nb: e can't be a horizontal.
            IsMaximaEdge := IsMaxima(e, topY)
            if IsMaximaEdge
            {
                eMaxPair := GetMaximaPairEx(e)
                IsMaximaEdge = !eMaxPair or !IsHorizontal(eMaxPair)
            }

            if IsMaximaEdge
            {
                if m_StrictSimple m_Maxima.add(e.Top.X)
                ePrev := e.PrevInAEL
                DoMaxima(e)
                if !ePrev e = m_ActiveEdges
                else e = ePrev.NextInAEL
            }
            else
            {
                //2. promote horizontal edges, otherwise update Curr.X and Curr.Y ...
                if IsIntermediate(e, topY) and IsHorizontal(e.NextInLML)
                {
                    UpdateEdgeIntoAEL(&e)
                    if e.OutIdx >= 0
                        AddOutPt(e, e.Bot)
                    AddEdgeToSEL(e)
                }
                else
                {
                    e.Curr.X = TopX(e, topY)
                    e.Curr.Y = topY
                }

                //When StrictlySimple and 'e' is being touched by another edge, then
                //make sure both edges have a vertex here ...
                if m_StrictSimple
                {
                    ePrev := e.PrevInAEL
                    if  (e.OutIdx >= 0) and (e.WindDelta != 0) and ePrev and (ePrev.OutIdx >= 0) and
                        (ePrev.Curr.X == e.Curr.X) and (ePrev.WindDelta != 0)
                    {
                        pt := e.Curr
                        op := AddOutPt(ePrev, pt)
                        op2 := AddOutPt(e, pt)
                        AddJoin(op, op2, pt)
                    }
                }

                e = e.NextInAEL
            }
        }

        //3. Process horizontals at the Top of the scanbeam ...
        m_Maxima.sort()
        ProcessHorizontals()
        m_Maxima.clear()

        //4. Promote intermediate vertices ...
        e = m_ActiveEdges
        while e
        {
            if IsIntermediate(e, topY)
            {
                var op: *OutPt
                if e.OutIdx >= 0
                    op = AddOutPt(e, e.Top)
                UpdateEdgeIntoAEL(&e)

                ePrev := e.PrevInAEL
                eNext := e.NextInAEL
                if ePrev and ePrev.Curr.X == e.Bot.X and
                    ePrev.Curr.Y == e.Bot.Y and op and
                    ePrev.OutIdx >= 0 and ePrev.Curr.Y > ePrev.Top.Y and
                    SlopesEqual(e.Curr, e.Top, ePrev.Curr, ePrev.Top) and
                    (e.WindDelta != 0) and (ePrev.WindDelta != 0)
                {
                    op2 := AddOutPt(ePrev, e.Bot)
                    AddJoin(op, op2, e.Top)
                }
                else if eNext and eNext.Curr.X == e.Bot.X and
                    eNext.Curr.Y == e.Bot.Y and op and
                    eNext.OutIdx >= 0 and eNext.Curr.Y > eNext.Top.Y and
                    SlopesEqual(e.Curr, e.Top, eNext.Curr, eNext.Top) and
                    (e.WindDelta != 0) and (eNext.WindDelta != 0)
                {
                    op2 := AddOutPt(eNext, e.Bot)
                    AddJoin(op, op2, e.Top)
                }
            }

            e = e.NextInAEL
        }
    }

    method ProcessIntersections(topY: CInt) throw
    {
        if !m_ActiveEdges return
        BuildIntersectList(topY)
        IlSize := m_IntersectList.count
        if IlSize == 0 return

        if IlSize == 1 or FixupIntersectionOrder()
            ProcessIntersectList()
        else
            throw "ProcessIntersections"
        m_SortedEdges = null
    }

    method BuildIntersectList(topY: CInt)
    {
        if !m_ActiveEdges return

        e := m_ActiveEdges
        m_SortedEdges = e
        while e
        {
            e.PrevInSEL = e.PrevInAEL
            e.NextInSEL = e.NextInAEL
            e.Curr.X = TopX(e, topY)
            e = e.NextInAEL
        }

        var isModified: bool
        loop
        {
            isModified = false
            e = m_SortedEdges
            while e.NextInSEL
            {
                eNext := e.NextInSEL
                var Pt: IntPoint
                if e.Curr.X > eNext.Curr.X
                {
                    IntersectPoint(e, eNext, &Pt)
                    if Pt.Y < topY Pt = IntPoint{TopX(e, topY), topY}
                    newNode := Memory.new'IntersectNode()
                    newNode.Edge1 = e;
                    newNode.Edge2 = eNext
                    newNode.Pt = Pt
                    m_IntersectList.add(newNode)
                    SwapPositionsInSEL(e, eNext)
                    isModified = true
                }
                else
                    e = eNext
            }

            if !e.PrevInSEL break
            e.PrevInSEL.NextInSEL = null
            if !isModified break
        }

        m_SortedEdges = null
    }

    method CopyAELToSEL()
    {
        e := m_ActiveEdges
        m_SortedEdges = e
        while e
        {
            e.PrevInSEL = e.PrevInAEL
            e.NextInSEL = e.NextInAEL
            e = e.NextInAEL
        }
    }

    func EdgesAdjacent(inode: *IntersectNode)->bool
    {
        return (inode.Edge1.NextInSEL == inode.Edge2) or (inode.Edge1.PrevInSEL == inode.Edge2)
    }

    method FixupIntersectionOrder()->bool
    {
        CopyAELToSEL()
        m_IntersectList.sort()
        loop i: m_IntersectList.count
        {
            if !EdgesAdjacent(m_IntersectList[i])
            {
                j := i + 1
                while j < m_IntersectList.count and !EdgesAdjacent(m_IntersectList[j]) j += 1
                if j == m_IntersectList.count return false
                swap(m_IntersectList.buffer + i, m_IntersectList.buffer + j)
            }

            SwapPositionsInSEL(m_IntersectList[i].Edge1, m_IntersectList[i].Edge2)
        }

        return true
    }

    method ProcessIntersectList()
    {
        visit iNode: m_IntersectList
        {
            IntersectEdges(iNode.Edge1, iNode.Edge2, iNode.Pt)
            SwapPositionsInAEL(iNode.Edge1, iNode.Edge2)
            Memory.delete(iNode)
        }

        m_IntersectList.clear()
    }

    method SwapPositionsInSEL(Edge1, Edge2: *TEdge)
    {
        if !Edge1.NextInSEL and !Edge1.PrevInSEL return
        if !Edge2.NextInSEL and !Edge2.PrevInSEL return

        if Edge1.NextInSEL == Edge2
        {
            Next := Edge2.NextInSEL
            if Next Next.PrevInSEL = Edge1
            Prev := Edge1.PrevInSEL
            if Prev Prev.NextInSEL = Edge2
            Edge2.PrevInSEL = Prev
            Edge2.NextInSEL = Edge1
            Edge1.PrevInSEL = Edge2
            Edge1.NextInSEL = Next
        }
        else if Edge2.NextInSEL == Edge1
        {
            Next := Edge1.NextInSEL
            if Next Next.PrevInSEL = Edge2
            Prev := Edge2.PrevInSEL
            if Prev Prev.NextInSEL = Edge1
            Edge1.PrevInSEL = Prev
            Edge1.NextInSEL = Edge2
            Edge2.PrevInSEL = Edge1
            Edge2.NextInSEL = Next
        }
        else
        {
            Next := Edge1.NextInSEL
            Prev := Edge1.PrevInSEL
            Edge1.NextInSEL = Edge2.NextInSEL
            if Edge1.NextInSEL Edge1.NextInSEL.PrevInSEL = Edge1
            Edge1.PrevInSEL = Edge2.PrevInSEL
            if Edge1.PrevInSEL Edge1.PrevInSEL.NextInSEL = Edge1
            Edge2.NextInSEL = Next
            if Edge2.NextInSEL Edge2.NextInSEL.PrevInSEL = Edge2
            Edge2.PrevInSEL = Prev
            if Edge2.PrevInSEL Edge2.PrevInSEL.NextInSEL = Edge2
        }

        if !Edge1.PrevInSEL m_SortedEdges = Edge1
        else if !Edge2.PrevInSEL m_SortedEdges = Edge2
    }

    method IntersectPoint(Edge1, Edge2: *TEdge, ip: *IntPoint)
    {
        var b1, b2: f64
        if Edge1.Dx == Edge2.Dx
        {
            ip.Y = Edge1.Curr.Y
            ip.X = TopX(Edge1, ip.Y)
            return
        }
        else if Edge1.Dx == 0
        {
            ip.X = Edge1.Bot.X
            if IsHorizontal(Edge2)
                ip.Y = Edge2.Bot.Y
            else
            {
                b2 = Edge2.Bot.Y - (Edge2.Bot.X / Edge2.Dx)
                ip.Y = Round(ip.X / Edge2.Dx + b2)
            }
        }
        else if Edge2.Dx == 0
        {
            ip.X = Edge2.Bot.X
            if IsHorizontal(Edge1)
                ip.Y = Edge1.Bot.Y
            else
            {
                b1 = Edge1.Bot.Y - (Edge1.Bot.X / Edge1.Dx)
                ip.Y = Round(ip.X / Edge1.Dx + b1)
            }
        }
        else
        {
            b1 = Edge1.Bot.X - Edge1.Bot.Y * Edge1.Dx
            b2 = Edge2.Bot.X - Edge2.Bot.Y * Edge2.Dx
            q := (b2-b1) / (Edge1.Dx - Edge2.Dx)
            ip.Y = Round(q)
            if Math.abs(Edge1.Dx) < Math.abs(Edge2.Dx)
                ip.X = Round(Edge1.Dx * q + b1)
            else
                ip.X = Round(Edge2.Dx * q + b2)
        }

        if ip.Y < Edge1.Top.Y or ip.Y < Edge2.Top.Y
        {
            if Edge1.Top.Y > Edge2.Top.Y
                ip.Y = Edge1.Top.Y
            else
                ip.Y = Edge2.Top.Y
            if Math.abs(Edge1.Dx) < Math.abs(Edge2.Dx)
                ip.X = TopX(Edge1, ip.Y)
            else
                ip.X = TopX(Edge2, ip.Y)
        }

        if (ip.Y > Edge1.Curr.Y)
        {
            ip.Y = Edge1.Curr.Y
            if Math.abs(Edge1.Dx) > Math.abs(Edge2.Dx)
                ip.X = TopX(Edge2, ip.Y)
            else
                ip.X = TopX(Edge1, ip.Y)
        }
    }

    method PopLocalMinima(Y: CInt, locMin: **LocalMinimum)->bool
    {
        if m_CurrentLM == m_MinimaList.count or m_MinimaList[m_CurrentLM].Y != Y return false
        dref locMin = m_MinimaList[m_CurrentLM]
        m_CurrentLM += 1
        return true
    }

    func Round(val: f64)->CInt
    {
        if val < 0 return cast(CInt) (val - 0.5)
        return cast(CInt) (val + 0.5)
    }

    func TopX(edge: *TEdge, currentY: CInt)->CInt
    {
        return currentY == edge.Top.Y ? edge.Top.X : edge.Bot.X + Round(edge.Dx * (currentY - edge.Bot.Y))
    }

    func E2InsertsBeforeE1(e1, e2: *TEdge)->bool
    {
        if e2.Curr.X == e1.Curr.X
        {
            if e2.Top.Y > e1.Top.Y
                return e2.Top.X < TopX(e1, e2.Top.Y)
            else
                return e1.Top.X > TopX(e2, e1.Top.Y)
        }
        else
            return e2.Curr.X < e1.Curr.X
    }

    method InsertEdgeIntoAEL(edge, startE: *TEdge)
    {
        startEdge := startE
        if !m_ActiveEdges
        {
            edge.PrevInAEL = null
            edge.NextInAEL = null
            m_ActiveEdges = edge
        }
        else if !startEdge and E2InsertsBeforeE1(m_ActiveEdges, edge)
        {
            edge.PrevInAEL = null
            edge.NextInAEL = m_ActiveEdges
            m_ActiveEdges.PrevInAEL = edge
            m_ActiveEdges = edge
        }
        else
        {
            if !startEdge startEdge = m_ActiveEdges
            while startEdge.NextInAEL and !E2InsertsBeforeE1(startEdge.NextInAEL, edge)
                startEdge = startEdge.NextInAEL
            edge.NextInAEL = startEdge.NextInAEL
            if startEdge.NextInAEL startEdge.NextInAEL.PrevInAEL = edge
            edge.PrevInAEL = startEdge
            startEdge.NextInAEL = edge
        }
    }

    method IsEvenOddFillType(edge: *TEdge)->bool
    {
        if edge.PolyTyp == .ptSubject
            return m_SubjFillType == .pftEvenOdd
        return m_ClipFillType == .pftEvenOdd
    }

    method IsEvenOddAltFillType(edge: *TEdge)->bool
    {
        if edge.PolyTyp == .ptSubject
            return m_ClipFillType == .pftEvenOdd
        return m_SubjFillType == .pftEvenOdd
    }

    method SetWindingCount(edge: *TEdge)
    {
        e := edge.PrevInAEL

        while e and ((e.PolyTyp != edge.PolyTyp) or (e.WindDelta == 0))
            e = e.PrevInAEL

        if !e
        {
            if edge.WindDelta == 0
            {
                pft := edge.PolyTyp == .ptSubject ? m_SubjFillType : m_ClipFillType
                edge.WindCnt = (pft == .pftNegative ? -1 : 1)
            }
            else
                edge.WindCnt = edge.WindDelta
            edge.WindCnt2 = 0
            e = m_ActiveEdges
        }
        else if edge.WindDelta == 0 and m_ClipType != .ctUnion
        {
            edge.WindCnt = 1
            edge.WindCnt2 = e.WindCnt2
            e = e.NextInAEL
        }
        else if IsEvenOddFillType(edge)
        {
            if edge.WindDelta == 0
            {
                Inside := true
                e2 := e.PrevInAEL
                while e2
                {
                    if e2.PolyTyp == e.PolyTyp and e2.WindDelta != 0
                        Inside = !Inside
                    e2 = e2.PrevInAEL
                }
                edge.WindCnt = (Inside ? 0 : 1)
            }
            else
                edge.WindCnt = edge.WindDelta
            edge.WindCnt2 = e.WindCnt2
            e = e.NextInAEL
        }
        else
        {
            if e.WindCnt * e.WindDelta < 0
            {
                if Math.abs(e.WindCnt) > 1
                {
                    if e.WindDelta * edge.WindDelta < 0
                        edge.WindCnt = e.WindCnt
                    else
                        edge.WindCnt = e.WindCnt + edge.WindDelta
                }
                else
                    edge.WindCnt = edge.WindDelta == 0 ? 1 : edge.WindDelta
            }
            else
            {
                if edge.WindDelta == 0
                    edge.WindCnt = (e.WindCnt < 0 ? e.WindCnt - 1 : e.WindCnt + 1)
                else if e.WindDelta * edge.WindDelta < 0
                    edge.WindCnt = e.WindCnt
                else
                    edge.WindCnt = e.WindCnt + edge.WindDelta
            }

            edge.WindCnt2 = e.WindCnt2
            e = e.NextInAEL
        }

        if IsEvenOddAltFillType(edge)
        {
            while e != edge
            {
                if e.WindDelta != 0
                    edge.WindCnt2 = edge.WindCnt2 == 0 ? 1 : 0
                e = e.NextInAEL
            }
        }
        else
        {
            while e != edge
            {
                edge.WindCnt2 += e.WindDelta
                e = e.NextInAEL
            }
        }
    }

    method IsContributing(edge: *TEdge)->bool
    {
        var pft, pft2: PolyFillType = ?
        if edge.PolyTyp == .ptSubject
        {
            pft = m_SubjFillType
            pft2 = m_ClipFillType
        }
        else
        {
            pft = m_ClipFillType
            pft2 = m_SubjFillType
        }

        switch pft
        {
        case pftEvenOdd:
            if edge.WindDelta == 0 and edge.WindCnt != 1
                return false
        case pftNonZero:
            if Math.abs(edge.WindCnt) != 1
                return false
        case pftPositive:
            if edge.WindCnt != 1
                return false
        default:
            if edge.WindCnt != -1
                return false
        }

        switch m_ClipType
        {
        case ctIntersection:
            switch pft2
            {
            case pftEvenOdd, pftNonZero:
                return edge.WindCnt2 != 0
            case pftPositive:
                return edge.WindCnt2 > 0
            default:
                return edge.WindCnt2 < 0
            }

        case ctUnion:
            switch pft2
            {
            case pftEvenOdd, pftNonZero:
                return edge.WindCnt2 == 0
            case pftPositive:
                return edge.WindCnt2 <= 0
            default:
                return edge.WindCnt2 >= 0
            }

        case ctDifference:
            if edge.PolyTyp == .ptSubject
            {
                switch pft2
                {
                case pftEvenOdd, pftNonZero:
                    return edge.WindCnt2 == 0
                case pftPositive:
                    return edge.WindCnt2 <= 0
                default:
                    return edge.WindCnt2 >= 0
                }
            }
            else
            {
                switch(pft2)
                {
                case pftEvenOdd, pftNonZero:
                    return edge.WindCnt2 != 0
                case pftPositive:
                    return edge.WindCnt2 > 0
                default:
                    return edge.WindCnt2 < 0
                }
            }

        case ctXor:
            if edge.WindDelta == 0
            {
                switch pft2
                {
                case pftEvenOdd, pftNonZero:
                    return edge.WindCnt2 == 0
                case pftPositive:
                    return edge.WindCnt2 <= 0
                default:
                    return edge.WindCnt2 >= 0
                }
            }
        }

        return true
    }

    method CreateOutRec()->*OutRec
    {
        result := Memory.new'OutRec()
        m_PolyOuts.add(result)
        result.Idx = cast(s32) (m_PolyOuts.count - 1)
        return result
    }

    method SetHoleState(e: *TEdge, outrec: *OutRec)
    {
        e2 := e.PrevInAEL
        var eTmp: *TEdge
        while e2
        {
            if e2.OutIdx >= 0 and e2.WindDelta != 0
            {
                if !eTmp eTmp = e2
                else if eTmp.OutIdx == e2.OutIdx eTmp = null
            }

            e2 = e2.PrevInAEL
        }
        if !eTmp
        {
            outrec.FirstLeft = null
            outrec.IsHole = false
        }
        else
        {
            outrec.FirstLeft = m_PolyOuts[eTmp.OutIdx]
            outrec.IsHole = !outrec.FirstLeft.IsHole
        }
    }

    #[Swag.discardable]
    method AddOutPt(e: *TEdge, pt: IntPoint)->*OutPt
    {
        if e.OutIdx < 0
        {
            outRec := CreateOutRec()
            outRec.IsOpen = e.WindDelta == 0
            newOp := Memory.new'OutPt();
            outRec.Pts = newOp
            newOp.Idx = outRec.Idx
            newOp.Pt = pt
            newOp.Next = newOp
            newOp.Prev = newOp
            if !outRec.IsOpen
                SetHoleState(e, outRec)
            e.OutIdx = outRec.Idx
            return newOp
        }
        else
        {
            outRec := m_PolyOuts[e.OutIdx]
            op := outRec.Pts

            ToFront := (e.Side == .esLeft)
            if ToFront and (pt == op.Pt) return op
            if !ToFront and (pt == op.Prev.Pt) return op.Prev

            newOp := Memory.new'OutPt()
            newOp.Idx = outRec.Idx
            newOp.Pt = pt
            newOp.Next = op
            newOp.Prev = op.Prev
            newOp.Prev.Next = newOp;
            op.Prev = newOp
            if ToFront outRec.Pts = newOp
            return newOp
        }
    }

    #[Swag.discardable]
    method AddLocalMinPoly(e1, e2: *TEdge, Pt: IntPoint)->*OutPt
    {
        var result: *OutPt
        var e, prevE: *TEdge

        if IsHorizontal(e2) or (e1.Dx > e2.Dx)
        {
            result = AddOutPt(e1, Pt)
            e2.OutIdx = e1.OutIdx
            e1.Side = .esLeft
            e2.Side = .esRight
            e = e1
            if e.PrevInAEL == e2
                prevE = e2.PrevInAEL
            else
                prevE = e.PrevInAEL
        }
        else
        {
            result = AddOutPt(e2, Pt)
            e1.OutIdx = e2.OutIdx
            e1.Side = .esRight
            e2.Side = .esLeft
            e = e2
            if e.PrevInAEL == e1
                prevE = e1.PrevInAEL
            else
                prevE = e.PrevInAEL
        }

        if prevE and prevE.OutIdx >= 0 and prevE.Top.Y < Pt.Y and e.Top.Y < Pt.Y
        {
            xPrev := TopX(prevE, Pt.Y)
            xE := TopX(e, Pt.Y)
            if  xPrev == xE and
                (e.WindDelta != 0) and
                (prevE.WindDelta != 0) and
                SlopesEqual(IntPoint{xPrev, Pt.Y}, prevE.Top, IntPoint{xE, Pt.Y}, e.Top)
            {
                outPt := AddOutPt(prevE, Pt)
                AddJoin(result, outPt, e.Top)
            }
        }

        return result
    }

    method AddEdgeToSEL(edge: *TEdge)
    {
        if !m_SortedEdges
        {
            m_SortedEdges = edge
            edge.PrevInSEL = null
            edge.NextInSEL = null
        }
        else
        {
            edge.NextInSEL = m_SortedEdges
            edge.PrevInSEL = null
            m_SortedEdges.PrevInSEL = edge
            m_SortedEdges = edge
        }
    }

    func HorzSegmentsOverlap(s1a, s1b, s2a, s2b: CInt)->bool
    {
        seg1a := s1a
        seg1b := s1b
        seg2a := s2a
        seg2b := s2b
        if seg1a > seg1b swap(&seg1a, &seg1b)
        if seg2a > seg2b swap(&seg2a, &seg2b)
        return (seg1a < seg2b) and (seg2a < seg1b)
    }

    func OutRec1RightOfOutRec2(o1, outRec2: *OutRec)->bool
    {
        outRec1 := o1
        loop
        {
            outRec1 = outRec1.FirstLeft
            if outRec1 == outRec2 return true
            if !outRec1 break
        }
        return false
    }

    func GetDx(pt1, pt2: IntPoint)->f64
    {
        return (pt1.Y == pt2.Y) ? HORIZONTAL : cast(f64) (pt2.X - pt1.X) / (pt2.Y - pt1.Y)
    }

    func Area(op1: *OutPt)->f64
    {
        op := op1
        startOp := op
        if !op return 0

        a := 0'f64
        loop
        {
            a += cast(f64) (op.Prev.Pt.X + op.Pt.X) * cast(f64) (op.Prev.Pt.Y - op.Pt.Y)
            op = op.Next
            if op == startOp break
        }

        return a * 0.5
    }

    func FirstIsBottomPt(btmPt1, btmPt2: *OutPt)->bool
    {
        p := btmPt1.Prev
        while (p.Pt == btmPt1.Pt) and (p != btmPt1) p = p.Prev
        dx1p := Math.abs(GetDx(btmPt1.Pt, p.Pt))
        p = btmPt1.Next
        while (p.Pt == btmPt1.Pt) and (p != btmPt1) p = p.Next
        dx1n := Math.abs(GetDx(btmPt1.Pt, p.Pt))

        p = btmPt2.Prev
        while (p.Pt == btmPt2.Pt) and (p != btmPt2) p = p.Prev
        dx2p := Math.abs(GetDx(btmPt2.Pt, p.Pt))
        p = btmPt2.Next
        while (p.Pt == btmPt2.Pt) and (p != btmPt2) p = p.Next
        dx2n := Math.abs(GetDx(btmPt2.Pt, p.Pt))

        if Math.max(dx1p, dx1n) == Math.max(dx2p, dx2n) and
           Math.min(dx1p, dx1n) == Math.min(dx2p, dx2n)
            return Area(btmPt1) > 0
        else
            return (dx1p >= dx2p and dx1p >= dx2n) or (dx1n >= dx2p and dx1n >= dx2n)
    }

    func GetBottomPt(pp1: *OutPt)->*OutPt
    {
        var dups: *OutPt
        pp := pp1
        p := pp.Next
        while p != pp
        {
            if p.Pt.Y > pp.Pt.Y
            {
                pp = p
                dups = null
            }
            else if p.Pt.Y == pp.Pt.Y and p.Pt.X <= pp.Pt.X
            {
                if p.Pt.X < pp.Pt.X
                {
                    dups = null
                    pp = p
                }
                else
                {
                    if p.Next != pp and p.Prev != pp dups = p
                }
            }
            p = p.Next
        }

        if dups
        {
            while dups != p
            {
                if !FirstIsBottomPt(p, dups) pp = dups
                dups = dups.Next
                while dups.Pt != pp.Pt
                    dups = dups.Next
            }
        }

        return pp
    }

    func GetLowermostRec(outRec1, outRec2: *OutRec)->*OutRec
    {
        if !outRec1.BottomPt
            outRec1.BottomPt = GetBottomPt(outRec1.Pts)
        if !outRec2.BottomPt
            outRec2.BottomPt = GetBottomPt(outRec2.Pts)
        OutPt1 := outRec1.BottomPt
        OutPt2 := outRec2.BottomPt
        if OutPt1.Pt.Y > OutPt2.Pt.Y return outRec1
        if OutPt1.Pt.Y < OutPt2.Pt.Y return outRec2
        if OutPt1.Pt.X < OutPt2.Pt.X return outRec1
        if OutPt1.Pt.X > OutPt2.Pt.X return outRec2
        if OutPt1.Next == OutPt1 return outRec2
        if OutPt2.Next == OutPt2 return outRec1
        if FirstIsBottomPt(OutPt1, OutPt2) return outRec1
        return outRec2
    }

    func ReversePolyPtLinks(pp: *OutPt)
    {
        if !pp return
        var pp1, pp2: *OutPt
        pp1 = pp
        loop
        {
            pp2 = pp1.Next
            pp1.Next = pp1.Prev
            pp1.Prev = pp2
            pp1 = pp2
            if pp1 == pp break
        }
    }

    method AppendPolygon(e1, e2: *TEdge)
    {
        //get the start and ends of both output polygons ...
        outRec1 := m_PolyOuts[e1.OutIdx]
        outRec2 := m_PolyOuts[e2.OutIdx]

        var holeStateRec: *OutRec
        if OutRec1RightOfOutRec2(outRec1, outRec2)
            holeStateRec = outRec2
        else if OutRec1RightOfOutRec2(outRec2, outRec1)
            holeStateRec = outRec1
        else
            holeStateRec = GetLowermostRec(outRec1, outRec2)

        //get the start and ends of both output polygons and
        //join e2 poly onto e1 poly and delete pointers to e2 ...

        p1_lft := outRec1.Pts
        p1_rt := p1_lft.Prev
        p2_lft := outRec2.Pts
        p2_rt := p2_lft.Prev

        //join e2 poly onto e1 poly and delete pointers to e2 ...
        if e1.Side == .esLeft
        {
            if e2.Side == .esLeft
            {
                ReversePolyPtLinks(p2_lft)
                p2_lft.Next = p1_lft
                p1_lft.Prev = p2_lft
                p1_rt.Next = p2_rt
                p2_rt.Prev = p1_rt
                outRec1.Pts = p2_rt
            }
            else
            {
                p2_rt.Next = p1_lft
                p1_lft.Prev = p2_rt
                p2_lft.Prev = p1_rt
                p1_rt.Next = p2_lft
                outRec1.Pts = p2_lft
            }
        }
        else
        {
            if e2.Side == .esRight
            {
                ReversePolyPtLinks(p2_lft)
                p1_rt.Next = p2_rt
                p2_rt.Prev = p1_rt
                p2_lft.Next = p1_lft
                p1_lft.Prev = p2_lft
            }
            else
            {
                p1_rt.Next = p2_lft
                p2_lft.Prev = p1_rt
                p1_lft.Prev = p2_rt
                p2_rt.Next = p1_lft
            }
        }

        outRec1.BottomPt = null
        if holeStateRec == outRec2
        {
            if outRec2.FirstLeft != outRec1
                outRec1.FirstLeft = outRec2.FirstLeft
            outRec1.IsHole = outRec2.IsHole
        }

        outRec2.Pts = null
        outRec2.BottomPt = null
        outRec2.FirstLeft = outRec1

        OKIdx := e1.OutIdx
        ObsoleteIdx := e2.OutIdx

        e1.OutIdx = Unassigned
        e2.OutIdx = Unassigned

        e := m_ActiveEdges
        while e
        {
            if e.OutIdx == ObsoleteIdx
            {
                e.OutIdx = OKIdx
                e.Side = e1.Side
                break
            }

            e = e.NextInAEL
        }

        outRec2.Idx = outRec1.Idx
    }

    method AddLocalMaxPoly(e1, e2: *TEdge, Pt: IntPoint)
    {
        AddOutPt(e1, Pt)
        if e2.WindDelta == 0 AddOutPt(e2, Pt)

        if e1.OutIdx == e2.OutIdx
        {
            e1.OutIdx = Unassigned
            e2.OutIdx = Unassigned
        }
        else if e1.OutIdx < e2.OutIdx
            AppendPolygon(e1, e2)
        else
            AppendPolygon(e2, e1)
    }

    func SwapSides(Edge1, Edge2: *TEdge)
    {
        Side := Edge1.Side
        Edge1.Side = Edge2.Side
        Edge2.Side = Side
    }

    func SwapPolyIndexes(Edge1, Edge2: *TEdge)
    {
        OutIdx := Edge1.OutIdx
        Edge1.OutIdx = Edge2.OutIdx
        Edge2.OutIdx = OutIdx
    }

    method IntersectEdges(e1, e2: *TEdge, Pt: IntPoint)
    {
        e1Contributing := e1.OutIdx >= 0
        e2Contributing := e2.OutIdx >= 0

        //update winding counts...
        //assumes that e1 will be to the Right of e2 ABOVE the intersection
        if e1.PolyTyp == e2.PolyTyp
        {
            if IsEvenOddFillType(e1)
            {
                oldE1WindCnt := e1.WindCnt
                e1.WindCnt = e2.WindCnt
                e2.WindCnt = oldE1WindCnt
            }
            else
            {
                if e1.WindCnt + e2.WindDelta == 0
                    e1.WindCnt = -e1.WindCnt
                else
                    e1.WindCnt += e2.WindDelta
                if e2.WindCnt - e1.WindDelta == 0
                    e2.WindCnt = -e2.WindCnt
                else
                    e2.WindCnt -= e1.WindDelta
            }
        }
        else
        {
            if !IsEvenOddFillType(e2)
                e1.WindCnt2 += e2.WindDelta
            else
                e1.WindCnt2 = (e1.WindCnt2 == 0) ? 1 : 0
            if !IsEvenOddFillType(e1)
                e2.WindCnt2 -= e1.WindDelta
            else
                e2.WindCnt2 = (e2.WindCnt2 == 0) ? 1 : 0
        }

        var e1FillType, e2FillType, e1FillType2, e2FillType2: PolyFillType = ?
        if e1.PolyTyp == .ptSubject
        {
            e1FillType = m_SubjFillType
            e1FillType2 = m_ClipFillType
        }
        else
        {
            e1FillType = m_ClipFillType
            e1FillType2 = m_SubjFillType
        }

        if e2.PolyTyp == .ptSubject
        {
            e2FillType = m_SubjFillType
            e2FillType2 = m_ClipFillType
        }
        else
        {
            e2FillType = m_ClipFillType
            e2FillType2 = m_SubjFillType
        }

        var e1Wc, e2Wc: CInt
        switch e1FillType
        {
        case pftPositive:   e1Wc = e1.WindCnt
        case pftNegative:   e1Wc = -e1.WindCnt
        default:            e1Wc = Math.abs(e1.WindCnt)
        }

        switch e2FillType
        {
        case pftPositive:   e2Wc = e2.WindCnt
        case pftNegative:   e2Wc = -e2.WindCnt
        default:            e2Wc = Math.abs(e2.WindCnt)
        }

        if e1Contributing and e2Contributing
        {
            if (e1Wc != 0 and e1Wc != 1) or (e2Wc != 0 and e2Wc != 1) or (e1.PolyTyp != e2.PolyTyp and m_ClipType != .ctXor)
            {
                AddLocalMaxPoly(e1, e2, Pt)
            }
            else
            {
                AddOutPt(e1, Pt)
                AddOutPt(e2, Pt)
                SwapSides(e1, e2)
                SwapPolyIndexes(e1, e2)
            }
        }
        else if e1Contributing
        {
            if e2Wc == 0 or e2Wc == 1
            {
                AddOutPt(e1, Pt)
                SwapSides(e1, e2)
                SwapPolyIndexes(e1, e2)
            }
        }
        else if e2Contributing
        {
            if e1Wc == 0 or e1Wc == 1
            {
                AddOutPt(e2, Pt);
                SwapSides(e1, e2)
                SwapPolyIndexes(e1, e2)
            }
        }
        else if (e1Wc == 0 or e1Wc == 1) and (e2Wc == 0 or e2Wc == 1)
        {
            var e1Wc2, e2Wc2: CInt
            switch e1FillType2
            {
            case pftPositive:   e1Wc2 = e1.WindCnt2
            case pftNegative:   e1Wc2 = -e1.WindCnt2
            default:            e1Wc2 = Math.abs(e1.WindCnt2)
            }

            switch e2FillType2
            {
            case pftPositive:   e2Wc2 = e2.WindCnt2
            case pftNegative:   e2Wc2 = -e2.WindCnt2
            default:            e2Wc2 = Math.abs(e2.WindCnt2)
            }

            if e1.PolyTyp != e2.PolyTyp
            {
                AddLocalMinPoly(e1, e2, Pt);
            }
            else if e1Wc == 1 and e2Wc == 1
            {
                switch m_ClipType
                {
                case ctIntersection:
                    if e1Wc2 > 0 and e2Wc2 > 0
                        AddLocalMinPoly(e1, e2, Pt)
                case ctUnion:
                    if e1Wc2 <= 0 and e2Wc2 <= 0
                        AddLocalMinPoly(e1, e2, Pt)
                case ctDifference:
                    if ((e1.PolyTyp == .ptClip) and (e1Wc2 > 0) and (e2Wc2 > 0)) or
                       ((e1.PolyTyp == .ptSubject) and (e1Wc2 <= 0) and (e2Wc2 <= 0))
                        AddLocalMinPoly(e1, e2, Pt)
                case ctXor:
                    AddLocalMinPoly(e1, e2, Pt)
                }
            }
            else
                SwapSides(e1, e2)
        }
    }

    method InsertLocalMinimaIntoAEL(botY: CInt)
    {
        var lm: *LocalMinimum
        while PopLocalMinima(botY, &lm)
        {
            lb := lm.LeftBound
            rb := lm.RightBound

            var Op1: *OutPt
            if !lb
            {
                //nb: don't insert LB into either AEL or SEL
                InsertEdgeIntoAEL(rb, null)
                SetWindingCount(rb)
                if IsContributing(rb)
                    Op1 = AddOutPt(rb, rb.Bot)
            }
            else if (!rb)
            {
                InsertEdgeIntoAEL(lb, null)
                SetWindingCount(lb)
                if IsContributing(lb)
                    Op1 = AddOutPt(lb, lb.Bot)
                InsertScanbeam(lb.Top.Y)
            }
            else
            {
                InsertEdgeIntoAEL(lb, null)
                InsertEdgeIntoAEL(rb, lb)
                SetWindingCount(lb)
                rb.WindCnt = lb.WindCnt
                rb.WindCnt2 = lb.WindCnt2
                if IsContributing(lb)
                    Op1 = AddLocalMinPoly(lb, rb, lb.Bot)
                InsertScanbeam(lb.Top.Y)
            }

            if rb
            {
                if IsHorizontal(rb)
                {
                    AddEdgeToSEL(rb)
                    if rb.NextInLML
                        InsertScanbeam(rb.NextInLML.Top.Y)
                }
                else
                    InsertScanbeam(rb.Top.Y)
            }

            if !lb or !rb continue

            //if any output polygons share an edge, they'll need joining later ...
            if Op1 and IsHorizontal(rb) and m_GhostJoins.count > 0 and (rb.WindDelta != 0)
            {
                loop i: m_GhostJoins.count
                {
                    jr := m_GhostJoins[i]
                    if HorzSegmentsOverlap(jr.OutPt1.Pt.X, jr.OffPt.X, rb.Bot.X, rb.Top.X)
                        AddJoin(jr.OutPt1, Op1, jr.OffPt)
                }
            }

            if  lb.OutIdx >= 0 and lb.PrevInAEL and
                lb.PrevInAEL.Curr.X == lb.Bot.X and
                lb.PrevInAEL.OutIdx >= 0 and
                SlopesEqual(lb.PrevInAEL.Bot, lb.PrevInAEL.Top, lb.Curr, lb.Top) and
                (lb.WindDelta != 0) and (lb.PrevInAEL.WindDelta != 0)
            {
                Op2 := AddOutPt(lb.PrevInAEL, lb.Bot)
                AddJoin(Op1, Op2, lb.Top)
            }

            if lb.NextInAEL != rb
            {
                if  rb.OutIdx >= 0 and
                    rb.PrevInAEL.OutIdx >= 0 and
                    SlopesEqual(rb.PrevInAEL.Curr, rb.PrevInAEL.Top, rb.Curr, rb.Top) and
                    (rb.WindDelta != 0) and
                    (rb.PrevInAEL.WindDelta != 0)
                {
                    Op2 := AddOutPt(rb.PrevInAEL, rb.Bot)
                    AddJoin(Op1, Op2, rb.Top)
                }

                e := lb.NextInAEL
                if e
                {
                    while e != rb
                    {
                        IntersectEdges(rb, e, lb.Curr)
                        e = e.NextInAEL
                    }
                }
            }
        }
    }

    func PointCount(Pts: *OutPt)->s32
    {
        if !Pts return 0
        result := 0
        p := Pts
        loop
        {
            result += 1
            p = p.Next
            if p == Pts break
        }

        return result
    }

    method BuildResult(polys: *ClipperPaths)
    {
        polys.reserve(m_PolyOuts.count)
        loop i: m_PolyOuts
        {
            if !m_PolyOuts[i].Pts continue

            var pg: ClipperPath

            p := m_PolyOuts[i].Pts.Prev
            cnt := PointCount(p)
            if cnt < 2 continue

            pg.reserve(cast(uint) cnt)
            loop cnt
            {
                pg.add(p.Pt)
                p = p.Prev
            }

            polys.emplace(&pg)
        }
    }

    method DisposeAllOutRecs()
    {
        loop i: m_PolyOuts DisposeOutRec(i)
        m_PolyOuts.clear()
    }

    func DisposeOutPts(pts: *OutPt)
    {
        pp := pts
        if pp == null return
        pp.Prev.Next = null
        while pp
        {
            tmpPp := pp
            pp = pp.Next
            Memory.delete(tmpPp)
        }
    }

    method DisposeOutRec(index: uint)
    {
        outRec := m_PolyOuts[index]
        if outRec.Pts DisposeOutPts(outRec.Pts)
        Memory.delete(outRec)
        m_PolyOuts[index] = null
    }
}