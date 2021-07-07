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

public enum PolyType { ptSubject; ptClip; }
enum EdgeSide { esLeft = 1; esRight = 2; }

struct LocalMinimum
{
    Y:            CInt
    LeftBound:    *TEdge
    RightBound:   *TEdge
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

#[Swag.opaque]
public struct Clipper
{
    m_UseFullRange:         bool
    m_PreserveCollinear:    bool
    m_edges:                Array'*TEdge
    m_MinimaList:           Array'LocalMinimum

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

    func SlopesEqual(e1: *TEdge, e2: *TEdge)->bool
    {
        return (e1.Top.Y - e1.Bot.Y) * (e2.Top.X - e2.Bot.X) == (e1.Top.X - e1.Bot.X) * (e2.Top.Y - e2.Bot.Y)
    }

    func SlopesEqual(pt1, pt2, pt3: IntPoint)->bool
    {
        return (pt1.Y-pt2.Y)*(pt2.X-pt3.X) == (pt1.X-pt2.X)*(pt2.Y-pt3.Y);
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
        //swap horizontal edges' Top and Bottom x's so they follow the natural
        //progression of the bounds - ie so their xbots will align with the
        //adjoining lower edge. [Helpful in the ProcessHorizontal() method.]
        swap(&e.Top.X, &e.Bot.X)
    }

    method ProcessBound(edge: *TEdge, NextIsForward: bool)->*TEdge
    {
        E := edge
        var Result: *TEdge = E
        var Horz: *TEdge

        if E.OutIdx == Skip
        {
            //if edges still remain in the current bound beyond the skip edge then
            //create another LocMin and call ProcessBound once more
            if NextIsForward
            {
                while (E.Top.Y == E.Next.Bot.Y) E = E.Next
                //don't include top horizontals when parsing a bound a second time,
                //they will be contained in the opposite bound ...
                while E != Result and IsHorizontal(E) E = E.Prev
            }
            else
            {
                while E.Top.Y == E.Prev.Bot.Y E = E.Prev
                while E != Result and IsHorizontal(E) E = E.Next
            }

            if E == Result
            {
                if NextIsForward Result = E.Next
                else Result = E.Prev
            }
            else
            {
                //there are more edges in the bound beyond result starting with E
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
            //We need to be careful with open paths because this may not be a
            //true local minima (ie E may be following a skip edge).
            //Also, consecutive horz. edges may start heading left before going right.
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
                //nb: at the top of a bound, horizontals are added to the bound
                //only when the preceding edge attaches to the horizontal's left vertex
                //unless a Skip edge is encountered when that becomes the top divide
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
}