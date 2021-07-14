// The original code has been modified for swag
// https://github.com/jhasse/poly2tri

// Copyright (c) 2009-2018, Poly2Tri Contributors
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the name of Poly2Tri nor the names of its contributors may be
//   used to endorse or promote products derived from this software without specific
//   prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#global namespace Poly2Tri
using Core

enum Orientation
{
    CW
    CCW
    Collinear
}

public struct Point
{
    x, y:       f64
    edgeList:  Array'*Edge
}

public struct Edge
{
    p: *Point
    q: *Point
}

impl Edge
{
    method set(p1, p2: *Point)
    {
        p = p1
        q = p2
        if p1.y > p2.y
        {
            q = p1
            p = p2
        }
        else if p1.y == p2.y
        {
            if p1.x > p2.x
            {
                q = p1
                p = p2
            }
            else if p1.x == p2.x
            {
                Debug.assert(false, "Edge::Edge: p1 == p2")
            }
        }

        q.edgeList.add(self)
    }
}

public struct Triangle
{
    constrainedEdge:   [3] bool
    delaunayEdge:      [3] bool
    points:            [3] *Point
    neighbors:         [3] *Triangle
    interior:          bool
}

impl Triangle
{
    method markConstrainedEdge(index: s32)
    {
        constrainedEdge[index] = true
    }

    method markConstrainedEdge(edge: *Edge)
    {
        markConstrainedEdge(edge.p, edge.q)
    }

    method markConstrainedEdge(p, q: *Point)
    {
        if (q == points[0] and p == points[1]) or (q == points[1] and p == points[0])
            constrainedEdge[2] = true
        else if (q == points[0] and p == points[2]) or (q == points[2] and p == points[0])
            constrainedEdge[1] = true
        else if (q == points[1] and p == points[2]) or (q == points[2] and p == points[1])
            constrainedEdge[0] = true
    }

    method oppositePoint(t: *Triangle, p: *Point)->*Point
    {
        cw := t.pointCW(p)
        return pointCW(cw)
    }

    method edgeIndex(p1, p2: *Point)->s32
    {
        switch p1
        {
        case points[0]:
            if points[1] == p2 return 2
            if points[2] == p2 return 1

        case points[1]:
            if points[2] == p2 return 0
            if points[0] == p2 return 2

        case points[2]:
            if points[0] == p2 return 1
            if points[1] == p2 return 0
        }

        return -1
    }

    method index(p: *Point)->s32
    {
        switch p
        {
        case points[0]: return 0
        case points[1]: return 1
        case points[2]: return 2
        }

        Debug.assert(false)
        return -1
    }

    method neighborAcross(point: *Point)->*Triangle
    {
        switch point
        {
        case points[0]: return neighbors[0]
        case points[1]: return neighbors[1]
        default:        return neighbors[2]
        }
    }

    method neighborCW(point: *Point)->*Triangle
    {
        switch point
        {
        case points[0]: return neighbors[1]
        case points[1]: return neighbors[2]
        default:        return neighbors[0]
        }
    }

    method neighborCCW(p: *Point)->*Triangle
    {
        switch p
        {
        case points[0]:  return neighbors[2]
        case points[1]:  return neighbors[0]
        default:         return neighbors[1]
        }
    }

    method getConstrainedEdgeCCW(p: *Point)->bool
    {
        switch p
        {
        case points[0]: return constrainedEdge[2]
        case points[1]: return constrainedEdge[0]
        default:        return constrainedEdge[1]
        }
    }

    method getConstrainedEdgeCW(p: *Point)->bool
    {
        switch p
        {
        case points[0]: return constrainedEdge[1]
        case points[1]: return constrainedEdge[2]
        default:        return constrainedEdge[0]
        }
    }

    method pointCW(point: *Point)->*Point
    {
        switch point
        {
        case points[0]: return points[2]
        case points[1]: return points[0]
        case points[2]: return points[1]
        }

        Debug.assert(false)
        return null
    }

    method pointCCW(point: *Point)->*Point
    {
        switch point
        {
        case points[0]: return points[1]
        case points[1]: return points[2]
        case points[2]: return points[0]
        }

        Debug.assert(false)
        return null
    }

    method getDelaunayEdgeCCW(p: *Point)->bool
    {
        switch p
        {
        case points[0]: return delaunayEdge[2]
        case points[1]: return delaunayEdge[0]
        default:        return delaunayEdge[1]
        }
    }

    method getDelaunayEdgeCW(p: *Point)->bool
    {
        switch p
        {
        case points[0]: return delaunayEdge[1]
        case points[1]: return delaunayEdge[2]
        default:        return delaunayEdge[0]
        }
    }

    method setDelaunayEdgeCCW(p: *Point, e: bool)
    {
        switch p
        {
        case points[0]: delaunayEdge[2] = e
        case points[1]: delaunayEdge[0] = e
        default:        delaunayEdge[1] = e
        }
    }

    method setDelaunayEdgeCW(p: *Point, e: bool)
    {
        switch p
        {
        case points[0]: delaunayEdge[1] = e
        case points[1]: delaunayEdge[2] = e
        default:        delaunayEdge[0] = e
        }
    }

    method setConstrainedEdgeCCW(p: *Point, ce: bool)
    {
        switch p
        {
        case points[0]: constrainedEdge[2] = ce
        case points[1]: constrainedEdge[0] = ce
        default:        constrainedEdge[1] = ce
        }
    }

    method setConstrainedEdgeCW(p: *Point, ce: bool)
    {
        switch p
        {
        case points[0]: constrainedEdge[1] = ce
        case points[1]: constrainedEdge[2] = ce
        default:        constrainedEdge[0] = ce
        }
    }

    method clearNeighbor(triangle: *Triangle)
    {
        switch triangle
        {
        case neighbors[0]:  neighbors[0] = null
        case neighbors[1]:  neighbors[1] = null
        default:            neighbors[2] = null
        }
    }

    method clearNeighbors()
    {
        neighbors[0] = null
        neighbors[1] = null
        neighbors[2] = null
    }

    method clearDelaunayEdges()
    {
        delaunayEdge[0] = false
        delaunayEdge[1] = false
        delaunayEdge[2] = false
    }

    method contains(p: *Point)->bool
    {
        return p == points[0] or p == points[1] or p == points[2]
    }

    method contains(p1, p2: *Point)->bool
    {
        return contains(p1) and contains(p2)
    }

    method markNeighbor(p1, p2: *Point, t: *Triangle)
    {
        if (p1 == points[2] and p2 == points[1]) or (p1 == points[1] and p2 == points[2])
            neighbors[0] = t
        else if (p1 == points[0] and p2 == points[2]) or (p1 == points[2] and p2 == points[0])
            neighbors[1] = t
        else if (p1 == points[0] and p2 == points[1]) or (p1 == points[1] and p2 == points[0])
            neighbors[2] = t
        else
            Debug.assert(false)
    }

    method markNeighbor(t: *Triangle)
    {
        if t.contains(points[1], points[2])
        {
            neighbors[0] = t
            t.markNeighbor(points[1], points[2], self)
        }
        else if t.contains(points[0], points[2])
        {
            neighbors[1] = t
            t.markNeighbor(points[0], points[2], self)
        }
        else if t.contains(points[0], points[1])
        {
            neighbors[2] = t
            t.markNeighbor(points[0], points[1], self)
        }
    }

    method legalize(opoint, npoint: *Point)
    {
        switch opoint
        {
        case points[0]:
            points[1] = points[0]
            points[0] = points[2]
            points[2] = npoint
        case points[1]:
            points[2] = points[1]
            points[1] = points[0]
            points[0] = npoint
        case points[2]:
            points[0] = points[2]
            points[2] = points[1]
            points[1] = npoint
        default:
            Debug.assert(false)
        }
    }
}

struct EdgeEvent
{
    constrainedEdge: *Edge
    right:            bool
}

struct Basin
{
    leftNode:       *Node
    bottomNode:     *Node
    rightNode:      *Node
    width:          f64
    leftHighest:    bool
}

struct Node
{
    point:      *Point
    triangle:   *Triangle
    next:       *Node
    prev:       *Node
    value:      f64
}

struct AdvancingFront
{
    head:           *Node
    tail:           *Node
    searchNode:     *Node
}

impl AdvancingFront
{
    method locatePoint(point: *Point)->*Node
    {
        px := point.x
        node := searchNode
        nx := node.point.x

        if px == nx
        {
            if point != node.point
            {
                if point == node.prev.point
                    node = node.prev
                else if point == node.next.point
                    node = node.next
                else
                    Debug.assert(false)
            }
        }
        else if px < nx
        {
            node = node.prev
            while node
            {
                if point == node.point
                    break
                node = node.prev
            }
        }
        else
        {
            node = node.next
            while node
            {
                if point == node.point
                    break
                node = node.next
            }
        }

        if node searchNode = node
        return node
    }

    method locateNode(x: f64)->*Node
    {
        node := searchNode
        if x < node.value
        {
            node = node.prev
            while node
            {
                if x >= node.value
                {
                    searchNode = node
                    return node
                }

                node = node.prev
            }
        }
        else
        {
            node = node.next
            while node
            {
                if x < node.value
                {
                    searchNode = node.prev
                    return node.prev
                }

                node = node.next
            }
        }

        return null
    }
}

#[Swag.Opaque]
public struct Tesselate
{
    basin:      Basin
    edgeEvent:  EdgeEvent
    triangles:  Array'*Triangle
    map:        Array'*Triangle
    edgeList:   Array'*Edge
    points:     Array'*Point
    nodes:      Array'*Node
    front:      AdvancingFront
    head:       *Point
    tail:       *Point
    afHead:     *Node
    afMiddle:   *Node
    afTail:     *Node
    polyStart:  uint
}

impl Tesselate
{
    method opDrop()
    {
        clear()
    }

    method clear()
    {
        Memory.delete(head)
        Memory.delete(tail)
        Memory.delete(afHead)
        Memory.delete(afMiddle)
        Memory.delete(afTail)
        visit p: map Memory.delete(p)
        visit p: edgeList Memory.delete(p)
        visit p: nodes Memory.delete(p)
        visit p: points Memory.delete(p)

        @init(self)
    }

    // To call before adding a polyline
    public method startPolyLine()
    {
        polyStart = points.count
    }

    // Register a new point
    public method addPoint(x, y: f64)
    {
        p := Memory.new'Point()
        p.x = x
        p.y = y
        points.add(p)
    }

    // To call to register the polyline
    public method endPolyLine()
    {
        func initEdges(using self, polyline: [..] *Point)
        {
            visit pt, i: polyline
            {
                nexti := (i + 1) % @countof(polyline)
                edge := Memory.new'Edge()
                edge.set(polyline[i], polyline[nexti])
                edgeList.add(edge)
            }
        }

        Debug.assert(polyStart != points.count, "no points !")
        initEdges(self, points[polyStart .. points.count - 1])
    }

    // Triangulate
    public method triangulate()
    {
        if points.isEmpty() return

        initTriangulation()
        createAdvancingFront()
        sweepPoints()
        finalizationPolygon()
    }

    // Get the result
    public method getTriangles()->const [..] *Triangle
    {
        return triangles.toSlice()
    }

    method sweepPoints()
    {
        loop i: 1..points.count-1
        {
            point := points[i]
            node := pointEvent(point)
            visit e: point.edgeList
                addEdgeEvent(e, node)
        }
    }

    method addEdgeEvent(ep, eq: *Point, t: *Triangle, point: *Point)
    {
        triangle := t
        Debug.assert(triangle != null, "EdgeEvent - null triangle")

        if isEdgeSideOfTriangle(triangle, ep, eq)
            return

        p1 := triangle.pointCCW(point)
        o1 := orient2d(eq, p1, ep)
        if o1 == .Collinear
        {
            if triangle.contains(eq, p1)
            {
                triangle.markConstrainedEdge(eq, p1)
                edgeEvent.constrainedEdge.q = p1
                triangle = triangle.neighborAcross(point)
                addEdgeEvent(ep, p1, triangle, p1)
            }
            else
            {
                Debug.assert(false, "EdgeEvent - collinear points not supported")
            }

            return
        }

        p2 := triangle.pointCW(point)
        o2 := orient2d(eq, p2, ep)
        if o2 == .Collinear
        {
            if triangle.contains(eq, p2)
            {
                triangle.markConstrainedEdge(eq, p2)
                edgeEvent.constrainedEdge.q = p2
                triangle = triangle.neighborAcross(point)
                addEdgeEvent(ep, p2, triangle, p2)
            }
            else
            {
                Debug.assert(false, "EdgeEvent - collinear points not supported")
            }

            return
        }

        if o1 == o2
        {
            if o1 == .CW
                triangle = triangle.neighborCCW(point)
            else
                triangle = triangle.neighborCW(point)
            addEdgeEvent(ep, eq, triangle, point)
        }
        else
        {
            flipEdgeEvent(ep, eq, triangle, point)
        }
    }

    method flipEdgeEvent(ep, eq: *Point, tri: *Triangle, p: *Point)
    {
        t := tri
        Debug.assert(t != null)

        ot := t.neighborAcross(p)
        Debug.assert(ot != null, "FlipEdgeEvent - null neighbor across")

        op := ot.oppositePoint(t, p)

        if inScanArea(p, t.pointCCW(p), t.pointCW(p), op)
        {
            rotateTrianglePair(t, p, ot, op)
            mapTriangleToNodes(t)
            mapTriangleToNodes(ot)

            if p == eq and op == ep
            {
                if eq == edgeEvent.constrainedEdge.q and ep == edgeEvent.constrainedEdge.p
                {
                    t.markConstrainedEdge(ep, eq)
                    ot.markConstrainedEdge(ep, eq)
                    discard legalize(t)
                    discard legalize(ot)
                }
            }
            else
            {
                o := orient2d(eq, op, ep)
                t = nextFlipTriangle(o, t, ot, p, op)
                flipEdgeEvent(ep, eq, t, p)
            }
        }
        else
        {
            newP := nextFlipPoint(ep, eq, ot, op)
            flipScanEdgeEvent(ep, eq, t, ot, newP)
            addEdgeEvent(ep, eq, t, p)
        }
    }

    method nextFlipTriangle(o: Orientation, t, ot: *Triangle, p, op: *Point)->*Triangle
    {
        if o == .CCW
        {
            edge_index := ot.edgeIndex(p, op)
            ot.delaunayEdge[edge_index] = true
            discard legalize(ot)
            ot.clearDelaunayEdges()
            return t
        }

        edge_index := t.edgeIndex(p, op)
        t.delaunayEdge[edge_index] = true
        discard legalize(t)
        t.clearDelaunayEdges()
        return ot
    }

    func nextFlipPoint(ep, eq: *Point, ot: *Triangle, op: *Point)->*Point
    {
        o2d := orient2d(eq, op, ep)
        if o2d == .CW  return ot.pointCCW(op)
        if o2d == .CCW return ot.pointCW(op)
        Debug.assert(false, "[Unsupported] Opposing point on constrained edge")
        return null
    }

    method flipScanEdgeEvent(ep, eq: *Point, flip_triangle: *Triangle, t: *Triangle, p: *Point)
    {
        ot := t.neighborAcross(p)
        Debug.assert(ot != null, "FlipScanEdgeEvent - null neighbor across")

        op := ot.oppositePoint(t, p)
        Debug.assert(op != null, "FlipScanEdgeEvent - null opposing point")

        p1 := flip_triangle.pointCCW(eq)
        p2 := flip_triangle.pointCW(eq)
        Debug.assert(p1 != null and p2 != null, "FlipScanEdgeEvent - null on either of points")

        if inScanArea(eq, p1, p2, op)
            flipEdgeEvent(eq, op, ot, op)
        else
        {
            newP := nextFlipPoint(ep, eq, ot, op)
            flipScanEdgeEvent(ep, eq, flip_triangle, ot, newP)
        }
    }

    method addEdgeEvent(edge: *Edge, node: *Node)
    {
        edgeEvent.constrainedEdge = edge
        edgeEvent.right = edge.p.x > edge.q.x

        if isEdgeSideOfTriangle(node.triangle, edge.p, edge.q)
            return

        fillEdgeEvent(edge, node)
        addEdgeEvent(edge.p, edge.q, node.triangle, edge.q)
    }

    method fillEdgeEvent(edge: *Edge, node: *Node)
    {
        if edgeEvent.right
            fillRightAboveEdgeEvent(edge, node)
        else
            fillLeftAboveEdgeEvent(edge, node)
    }

    method fillRightAboveEdgeEvent(edge: *Edge, n: *Node)
    {
        node := n
        while node.next.point.x < edge.p.x
        {
            if orient2d(edge.q, node.next.point, edge.p) == .CCW
                fillRightBelowEdgeEvent(edge, node)
            else
                node = node.next
        }
    }

    method fillRightBelowEdgeEvent(edge: *Edge, node: *Node)
    {
        if node.point.x < edge.p.x
        {
            if orient2d(node.point, node.next.point, node.next.next.point) == .CCW
                fillRightConcaveEdgeEvent(edge, node)
            else
            {
                fillRightConvexEdgeEvent(edge, node)
                fillRightBelowEdgeEvent(edge, node)
            }
        }
    }

    method fillRightConcaveEdgeEvent(edge: *Edge, node: *Node)
    {
        fill(node.next)
        if node.next.point != edge.p
        {
            if orient2d(edge.q, node.next.point, edge.p) == .CCW
            {
                if orient2d(node.point, node.next.point, node.next.next.point) == .CCW
                    fillRightConcaveEdgeEvent(edge, node);
            }
        }
    }

    method fillRightConvexEdgeEvent(edge: *Edge, node: *Node)
    {
        if orient2d(node.next.point, node.next.next.point, node.next.next.next.point) == .CCW
        {
            fillRightConcaveEdgeEvent(edge, node.next)
        }
        else
        {
            if orient2d(edge.q, node.next.next.point, edge.p) == .CCW
                fillRightConvexEdgeEvent(edge, node.next);
        }
    }

    method fillLeftAboveEdgeEvent(edge: *Edge, n: *Node)
    {
        node := n
        while node.prev.point.x > edge.p.x
        {
            if orient2d(edge.q, node.prev.point, edge.p) == .CW
                fillLeftBelowEdgeEvent(edge, node)
            else
                node = node.prev
        }
    }

    method fillLeftBelowEdgeEvent(edge: *Edge, node: *Node)
    {
        if node.point.x > edge.p.x
        {
            if orient2d(node.point, node.prev.point, node.prev.prev.point) == .CW
                fillLeftConcaveEdgeEvent(edge, node)
            else
            {
                fillLeftConvexEdgeEvent(edge, node)
                fillLeftBelowEdgeEvent(edge, node)
            }
        }
    }

    method fillLeftConvexEdgeEvent(edge: *Edge, node: *Node)
    {
        if orient2d(node.prev.point, node.prev.prev.point, node.prev.prev.prev.point) == .CW
            fillLeftConcaveEdgeEvent(edge, node.prev)
        else
        {
            if orient2d(edge.q, node.prev.prev.point, edge.p) == .CW
                fillLeftConvexEdgeEvent(edge, node.prev)
        }
    }

    method fillLeftConcaveEdgeEvent(edge: *Edge, node: *Node)
    {
        fill(node.prev)
        if node.prev.point != edge.p
        {
            if orient2d(edge.q, node.prev.point, edge.p) == .CW
            {
                if orient2d(node.point, node.prev.point, node.prev.prev.point) == .CW
                    fillLeftConcaveEdgeEvent(edge, node)
            }
        }
    }

    func isEdgeSideOfTriangle(triangle: *Triangle, ep, eq : *Point)->bool
    {
        index := triangle.edgeIndex(ep, eq)
        if index != -1
        {
            triangle.markConstrainedEdge(index)
            t := triangle.neighbors[index]
            if t t.markConstrainedEdge(ep, eq)
            return true
        }

        return false
    }

    method pointEvent(point: *Point)->*Node
    {
        node := locateNode(point)
        Debug.assert(node and node.point and node.next and node.next.point, "null node")
        new_node := newFrontTriangle(point, node)
        if point.x <= node.point.x + Math.ConstF64.Epsilon
            fill(node)
        fillAdvancingFront(new_node)
        return new_node
    }

    method fillAdvancingFront(n: *Node)
    {
        node := n.next
        while node and node.next
        {
            if largeHoleDontFill(node) break
            fill(node)
            node = node.next
        }

        node = n.prev
        while node and node.prev
        {
            if largeHoleDontFill(node) break
            fill(node)
            node = node.prev
        }

        if n.next and n.next.next
        {
            angle := basinAngle(n)
            if angle < Math.ConstF64.ThreePiBy4
                fillBasin(n)
        }
    }

    method fillBasin(node: *Node)
    {
        if orient2d(node.point, node.next.point, node.next.next.point) == .CCW
            basin.leftNode = node.next.next
        else
            basin.leftNode = node.next

        basin.bottomNode = basin.leftNode
        while basin.bottomNode.next and basin.bottomNode.point.y >= basin.bottomNode.next.point.y
            basin.bottomNode = basin.bottomNode.next
        if basin.bottomNode == basin.leftNode
            return

        basin.rightNode = basin.bottomNode
        while basin.rightNode.next and basin.rightNode.point.y < basin.rightNode.next.point.y
            basin.rightNode = basin.rightNode.next
        if basin.rightNode == basin.bottomNode
            return

        basin.width = basin.rightNode.point.x - basin.leftNode.point.x
        basin.leftHighest = basin.leftNode.point.y > basin.rightNode.point.y

        fillBasinReq(basin.bottomNode)
    }

    method fillBasinReq(n: *Node)
    {
        node := n
        if isShallow(node)
            return

        fill(node)

        if node.prev == basin.leftNode and node.next == basin.rightNode
            return

        if node.prev == basin.leftNode
        {
            o := orient2d(node.point, node.next.point, node.next.next.point)
            if o == .CW
                return
            node = node.next
        }
        else if node.next == basin.rightNode
        {
            o := orient2d(node.point, node.prev.point, node.prev.prev.point)
            if o == .CCW
                return
            node = node.prev
        }
        else
        {
            if node.prev.point.y < node.next.point.y
                node = node.prev
            else
                node = node.next
        }

        fillBasinReq(node)
    }

    method isShallow(node: *Node)->bool
    {
        var height: f64
        if basin.leftHighest
            height = basin.leftNode.point.y - node.point.y
         else
            height = basin.rightNode.point.y - node.point.y
        return basin.width > height
    }

    func basinAngle(node: *Node)->f64
    {
        ax := node.point.x - node.next.next.point.x
        ay := node.point.y - node.next.next.point.y
        return Math.atan2(ay, ax)
    }

    func largeHoleDontFill(node: *Node)->bool
    {
        nextNode := node.next
        prevNode := node.prev
        if !angleExceeds90Degrees(node.point, nextNode.point, prevNode.point)
            return false

        next2Node := nextNode.next
        if next2Node and !angleExceedsPlus90DegreesOrIsNegative(node.point, next2Node.point, prevNode.point)
            return false

        prev2Node := prevNode.prev
        if prev2Node and !angleExceedsPlus90DegreesOrIsNegative(node.point, nextNode.point, prev2Node.point)
            return false

        return true;
    }

    func angleExceeds90Degrees(origin, pa, pb: *Point)->bool
    {
        angle := angle(origin, pa, pb)
        return (angle > Math.ConstF64.PiBy2) or (angle < -Math.ConstF64.PiBy2)
    }

    func angleExceedsPlus90DegreesOrIsNegative(origin, pa, pb: *Point)->bool
    {
        angle := angle(origin, pa, pb)
        return (angle > Math.ConstF64.PiBy2) or (angle < 0)
    }

    func angle(origin, pa, pb: *Point)->f64
    {
        px := origin.x
        py := origin.y
        ax := pa.x - px
        ay := pa.y - py
        bx := pb.x - px
        by := pb.y - py
        x := ax * by - ay * bx
        y := ax * bx + ay * by
        return Math.atan2(x, y)
    }

    method fill(node: *Node)
    {
        triangle := Memory.new'Triangle()
        triangle.points[0] = node.prev.point
        triangle.points[1] = node.point
        triangle.points[2] = node.next.point

        triangle.markNeighbor(node.prev.triangle)
        triangle.markNeighbor(node.triangle)
        map.add(triangle)

        node.prev.next = node.next
        node.next.prev = node.prev

        if !legalize(triangle)
            mapTriangleToNodes(triangle)
    }

    method newFrontTriangle(point: *Point, node: *Node)->*Node
    {
        triangle := Memory.new'Triangle()
        triangle.points[0] = point
        triangle.points[1] = node.point
        triangle.points[2] = node.next.point

        triangle.markNeighbor(node.triangle)
        map.add(triangle)

        new_node := Memory.new'Node()
        new_node.point = point
        new_node.value = point.x
        nodes.add(new_node)

        new_node.next = node.next
        new_node.prev = node
        node.next.prev = new_node
        node.next = new_node

        if !legalize(triangle)
            mapTriangleToNodes(triangle)

        return new_node
    }

    func orient2d(pa, pb, pc: *Point)->Orientation
    {
        detleft  := (pa.x - pc.x) * (pb.y - pc.y)
        detright := (pa.y - pc.y) * (pb.x - pc.x)
        val := detleft - detright
        if Math.isZeroEpsilon(val)
            return Orientation.Collinear
        if val > 0
            return Orientation.CCW
        return Orientation.CW
    }

    func inScanArea(pa, pb, pc, pd: *Point)->bool
    {
        oadb := (pa.x - pb.x) * (pd.y - pb.y) - (pd.x - pb.x) * (pa.y - pb.y)
        if oadb >= -Math.ConstF64.Epsilon
            return false

        oadc := (pa.x - pc.x) * (pd.y - pc.y) - (pd.x - pc.x) * (pa.y - pc.y)
        if oadc <= Math.ConstF64.Epsilon
            return false

        return true
    }

    func incircle(pa, pb, pc, pd: *Point)->bool
    {
        adx := pa.x - pd.x
        ady := pa.y - pd.y
        bdx := pb.x - pd.x
        bdy := pb.y - pd.y

        adxbdy := adx * bdy
        bdxady := bdx * ady
        oabd := adxbdy - bdxady

        if oabd <= 0
            return false

        cdx := pc.x - pd.x
        cdy := pc.y - pd.y

        cdxady := cdx * ady
        adxcdy := adx * cdy
        ocad := cdxady - adxcdy

        if ocad <= 0
            return false

        bdxcdy := bdx * cdy
        cdxbdy := cdx * bdy

        alift := adx * adx + ady * ady
        blift := bdx * bdx + bdy * bdy
        clift := cdx * cdx + cdy * cdy

        det := alift * (bdxcdy - cdxbdy) + blift * ocad + clift * oabd
        return det > 0
    }

    method legalize(t: *Triangle)->bool
    {
        loop i: 3
        {
            if t.delaunayEdge[i]
                continue

            ot := t.neighbors[i]
            if ot
            {
                p := t.points[i]
                op := ot.oppositePoint(t, p)
                oi := ot.index(op)

                if (ot.constrainedEdge[oi] or ot.delaunayEdge[oi])
                {
                    t.constrainedEdge[i] = ot.constrainedEdge[oi]
                    continue
                }

                inside := incircle(p, t.pointCCW(p), t.pointCW(p), op)
                if (inside)
                {
                    t.delaunayEdge[i] = true
                    ot.delaunayEdge[oi] = true

                    rotateTrianglePair(t, p, ot, op)

                    not_legalized := !legalize(t)
                    if not_legalized
                        mapTriangleToNodes(t)

                    not_legalized = !legalize(ot)
                    if not_legalized
                        mapTriangleToNodes(ot)

                    t.delaunayEdge[i] = false
                    ot.delaunayEdge[oi] = false
                    return true;
                }
            }
        }

        return false
    }

    func rotateTrianglePair(t: *Triangle, p: *Point, ot: *Triangle, op: *Point)
    {
        n1 := t.neighborCCW(p)
        n2 := t.neighborCW(p)
        n3 := ot.neighborCCW(op)
        n4 := ot.neighborCW(op)

        ce1 := t.getConstrainedEdgeCCW(p)
        ce2 := t.getConstrainedEdgeCW(p)
        ce3 := ot.getConstrainedEdgeCCW(op)
        ce4 := ot.getConstrainedEdgeCW(op)

        de1 := t.getDelaunayEdgeCCW(p)
        de2 := t.getDelaunayEdgeCW(p)
        de3 := ot.getDelaunayEdgeCCW(op)
        de4 := ot.getDelaunayEdgeCW(op)

        t.legalize(p, op)
        ot.legalize(op, p)

        ot.setDelaunayEdgeCCW(p, de1)
        t.setDelaunayEdgeCW(p, de2)
        t.setDelaunayEdgeCCW(op, de3)
        ot.setDelaunayEdgeCW(op, de4)

        ot.setConstrainedEdgeCCW(p, ce1)
        t.setConstrainedEdgeCW(p, ce2)
        t.setConstrainedEdgeCCW(op, ce3)
        ot.setConstrainedEdgeCW(op, ce4)

        t.clearNeighbors()
        ot.clearNeighbors()
        if n1 ot.markNeighbor(n1)
        if n2 t.markNeighbor(n2)
        if n3 t.markNeighbor(n3)
        if n4 ot.markNeighbor(n4)
        t.markNeighbor(ot)
    }

    method mapTriangleToNodes(t: *Triangle)
    {
        loop i: 3
        {
            if !t.neighbors[i]
            {
                n := front.locatePoint(t.pointCW(t.points[i]))
                if n n.triangle = t
            }
        }
    }

    method locateNode(point: *Point)->*Node
    {
        return front.locateNode(point.x)
    }

    method finalizationPolygon()
    {
        t := front.head.next.triangle
        p := front.head.next.point
        while t and !t.getConstrainedEdgeCW(p)
            t = t.neighborCCW(p)
        if t meshClean(t)
    }

    method meshClean(triangle: *Triangle)
    {
        var stack: Array'*Triangle
        stack.add(triangle)

        while stack.count
        {
            t := stack.popBack()
            if !t or t.interior continue
            t.interior = true
            triangles.add(t)
            loop i: 3
            {
                if !t.constrainedEdge[i]
                    stack.add(t.neighbors[i])
            }
        }
    }

    method initTriangulation()
    {
        xmin, xmax := points[0].x
        ymin, ymax := points[0].y
        visit p: points
        {
            xmax = Math.max(xmax, p.x)
            xmin = Math.min(xmin, p.x)
            ymax = Math.max(ymax, p.y)
            ymin = Math.min(ymin, p.y)
        }

        const KAlpha = 0.3'f64
        dx := KAlpha * (xmax - xmin)
        dy := KAlpha * (ymax - ymin)
        head = Memory.new'Point()
        head.x = xmin - dx
        head.y = ymin - dy
        tail = Memory.new'Point()
        tail.x = xmax + dx
        tail.y = ymin - dy

        func cmp(a, b: *Point)->s32
        {
            if a.y < b.y return -1
            if a.y > b.y return 1
            if a.x < b.x return -1
            if a.x > b.x return 1
            return 0
        }

        Slice.sort(points.toSlice(), &cmp)
    }

    method createAdvancingFront()
    {
        triangle := Memory.new'Triangle()
        triangle.points[0] = points[0]
        triangle.points[1] = head
        triangle.points[2] = tail
        map.add(triangle)

        afHead = Memory.new'Node()
        afMiddle = Memory.new'Node()
        afTail = Memory.new'Node()

        front.head = afHead
        front.tail = afTail
        front.searchNode = afHead

        afHead.point = triangle.points[1]
        afHead.triangle = triangle
        afHead.next = afMiddle
        afHead.value = afHead.point.x

        afMiddle.point = triangle.points[0]
        afMiddle.triangle = triangle
        afMiddle.next = afTail
        afMiddle.prev = afHead
        afMiddle.value = afMiddle.point.x

        afTail.point = triangle.points[2]
        afTail.triangle = triangle
        afTail.prev = afMiddle
        afTail.value = afTail.point.x
    }
}