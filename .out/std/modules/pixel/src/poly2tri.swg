// https://github.com/jhasse/poly2tri

using Core

enum Orientation
{
    CW
    CCW
    Collinear
}

func orient2d(pa, pb, pc: *Point)->Orientation
{
    detleft  := (pa.x - pc.x) * (pb.y - pc.y)
    detright := (pa.y - pc.y) * (pb.x - pc.x)
    val := detleft - detright
    if val > -Math.ConstF64.Epsilon and val < Math.ConstF64.Epsilon
        return Orientation.Collinear
    if val > 0
        return Orientation.CCW
    return Orientation.CW
}

struct Point
{
    x, y:       f64
    edge_list:  Array'(*Edge)
}

struct Edge
{
    p: *Point
    q: *Point
}

impl Edge
{
    method set(p1, p2: *Point)
    {
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

        q.edge_list.add(self)
    }
}


struct Triangle
{
    constrained_edge:   [3] bool
    delaunay_edge:      [3] bool

    points:     [3] *Point
    neighbors:  [3] *Triangle
    interior:   bool
}

impl Triangle
{
    method oppositePoint(t: *Triangle, p: *Point)->*Point
    {
        cw := t.pointCW(p)
        return pointCW(cw)
    }

    method index(p: *Point)->s32
    {
        switch p
        {
        case points[0]: return 0
        case points[1]: return 1
        case points[1]: return 2
        }

        Debug.assert(false)
        return -1
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
        case points[0]: return constrained_edge[2]
        case points[1]: return constrained_edge[0]
        default:        return constrained_edge[1]
        }
    }

    method getConstrainedEdgeCW(p: *Point)->bool
    {
        switch p
        {
        case points[0]: return constrained_edge[1]
        case points[1]: return constrained_edge[2]
        default:        return constrained_edge[0]
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

    method getDelunayEdgeCCW(p: *Point)->bool
    {
        switch p
        {
        case points[0]: return delaunay_edge[2]
        case points[1]: return delaunay_edge[0]
        default:        return delaunay_edge[1]
        }
    }

    method getDelunayEdgeCW(p: *Point)->bool
    {
        switch p
        {
        case points[0]: return delaunay_edge[1]
        case points[1]: return delaunay_edge[2]
        default:        return delaunay_edge[0]
        }
    }

    method setDelunayEdgeCCW(p: *Point, e: bool)
    {
        switch p
        {
        case points[0]: delaunay_edge[2] = e
        case points[1]: delaunay_edge[0] = e
        default:        delaunay_edge[1] = e
        }
    }

    method setDelunayEdgeCW(p: *Point, e: bool)
    {
        switch p
        {
        case points[0]: delaunay_edge[1] = e
        case points[1]: delaunay_edge[2] = e
        default:        delaunay_edge[0] = e
        }
    }

    method setConstrainedEdgeCCW(p: *Point, ce: bool)
    {
        switch p
        {
        case points[0]: constrained_edge[2] = ce
        case points[1]: constrained_edge[0] = ce
        default:        constrained_edge[1] = ce
        }
    }

    method setConstrainedEdgeCW(p: *Point, ce: bool)
    {
        switch p
        {
        case points[0]: constrained_edge[1] = ce
        case points[1]: constrained_edge[2] = ce
        default:        constrained_edge[0] = ce
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

    method contains(p: *Point)->bool
    {
        return p == points[0] or p == points[1] or p == points[2]
    }

    method contains(p1, p2: *Point)->bool
    {
        return contains(p1) or contains(p2)
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
        }

        Debug.assert(false)
    }
}

struct EdgeEvent
{
    constrained_edge: *Edge
    right:            bool
}

struct Basin
{
    left_node:    *Node
    bottom_node:  *Node
    right_node:   *Node
    width:        f64
    left_highest: bool
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
    search_node:    *Node
}

impl AdvancingFront
{
    method locatePoint(point: *Point)->*Node
    {
        px := point.x
        node := search_node
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

        if node search_node = node
        return node
    }

    method locateNode(x: f64)->*Node
    {
        node := search_node
        if x < node.value
        {
            node = node.prev
            while node
            {
                if x >= node.value
                {
                    search_node = node
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
                    search_node = node.prev
                    return node.prev
                }

                node = node.next
            }
        }

        return null
    }
}

#[Swag.opaque]
public struct Poly2Tri
{
    basin:      Basin
    edge_event: EdgeEvent
    triangles:  Array'*Triangle
    map:        Array'*Triangle
    edge_list:  Array'*Edge
    points:     Array'*Point
    nodes:      Array'*Node
    front:      AdvancingFront
    head:       *Point
    tail:       *Point
    af_head:    *Node
    af_middle:  *Node
    af_tail:    *Node
    polyStart:  uint
}

impl Poly2Tri
{
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
                edge_list.add(edge)
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

    method sweepPoints()
    {
        loop i: 1..points.count-1
        {
            point := points[i]
            node := pointEvent(point)
            visit e: point.edge_list
                edgeEvent(e, node)
        }
    }

    method edgeEvent(edge: *Edge, node: *Node)
    {

    }

    method pointEvent(point: *Point)->*Node
    {
        node := locateNode(point)
        Debug.assert(node and node.point and node.next and node.next.point, "null node")
        new_node := newFrontTriangle(point, node)
        if point.x < node.point.x + Math.ConstF64.Epsilon
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
            basin.left_node = node.next.next
        else
            basin.left_node = node.next

        basin.bottom_node = basin.left_node
        while basin.bottom_node.next and basin.bottom_node.point.y >= basin.bottom_node.next.point.y
            basin.bottom_node = basin.bottom_node.next
        if basin.bottom_node == basin.left_node
            return

        basin.right_node = basin.bottom_node
        while basin.right_node.next and basin.right_node.point.y < basin.right_node.next.point.y
            basin.right_node = basin.right_node.next
        if basin.right_node == basin.bottom_node
            return

        basin.width = basin.right_node.point.x - basin.left_node.point.x
        basin.left_highest = basin.left_node.point.y > basin.right_node.point.y

        fillBasinReq(basin.bottom_node)
    }

    method fillBasinReq(n: *Node)
    {
        node := n
        if isShallow(node)
            return

        fill(node)

        if node.prev == basin.left_node and node.next == basin.right_node
            return

        if node.prev == basin.left_node
        {
            o := orient2d(node.point, node.next.point, node.next.next.point)
            if o == .CW
                return
            node = node.next
        }
        else if node.next == basin.right_node
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
        if basin.left_highest
            height = basin.left_node.point.y - node.point.y
         else
            height = basin.right_node.point.y - node.point.y
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
        triangle.markNeighbor(node.triangle)
        map.add(triangle)

        new_node := Memory.new'Node()
        nodes.add(new_node)

        new_node.next = node.next
        new_node.prev = node
        node.next.prev = new_node
        node.next = new_node

        if !legalize(triangle)
            mapTriangleToNodes(triangle)

        return new_node
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
            if t.delaunay_edge[i]
                continue

            ot := t.neighbors[i]
            if ot
            {
                p := t.points[i]
                op := ot.oppositePoint(t, p)
                oi := ot.index(op)

                if (ot.constrained_edge[oi] or ot.delaunay_edge[oi])
                {
                    t.constrained_edge[i] = ot.constrained_edge[oi]
                    continue
                }

                inside := incircle(p, t.pointCCW(p), t.pointCW(p), op)
                if (inside)
                {
                    t.delaunay_edge[i] = true
                    ot.delaunay_edge[oi] = true

                    rotateTrianglePair(t, p, ot, op)

                    not_legalized := !legalize(t)
                    if not_legalized
                        mapTriangleToNodes(t)

                    not_legalized = !legalize(ot)
                    if not_legalized
                        mapTriangleToNodes(ot)

                    t.delaunay_edge[i] = false
                    ot.delaunay_edge[oi] = false
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

        de1 := t.getDelunayEdgeCCW(p)
        de2 := t.getDelunayEdgeCW(p)
        de3 := ot.getDelunayEdgeCCW(op)
        de4 := ot.getDelunayEdgeCW(op)

        t.legalize(p, op)
        ot.legalize(op, p)

        ot.setDelunayEdgeCCW(p, de1)
        t.setDelunayEdgeCW(p, de2)
        t.setDelunayEdgeCCW(op, de3)
        ot.setDelunayEdgeCW(op, de4)

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
                if !t.constrained_edge[i]
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

        const KAlpha = 0.3
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

        af_head = Memory.new'Node()
        af_middle = Memory.new'Node()
        af_tail = Memory.new'Node()

        front.head = af_head
        front.tail = af_tail

        af_head.point = triangle.points[1]
        af_head.triangle = triangle
        af_head.next = af_middle

        af_middle.point = triangle.points[0]
        af_middle.triangle = triangle
        af_middle.next = af_tail
        af_middle.prev = af_head

        af_tail.point = triangle.points[2]
        af_tail.triangle = triangle
        af_tail.prev = af_middle
    }
}