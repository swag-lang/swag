// https://github.com/jhasse/poly2tri

using Core

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
        return null
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
            node := pointEvent(points[i])
        }
    }

    method pointEvent(point: *Point)->*Node
    {
        node := locateNode(point)
        Debug.assert(node and node.point and node.next and node.next.point, "null node")
        new_node := newFrontTriangle(point, node)
        /*if point.x < node.point.x + Math.ConstF64.Epsilon
            fill(node)
        fillAdvancingFront(new_node)*/
        return new_node
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

    method legalize(triangle: *Triangle)->bool
    {
        return true
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