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
    func set(using self, p1, p2: *Point)
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
    func neighborCCW(using self, p: *Point)->*Triangle
    {
        if p == points[0]
            return neighbors[2]
        if p == points[1]
            return neighbors[0]
        return neighbors[1]
    }

    func getConstrainedEdgeCCW(using self, p: *Point)->bool
    {
        if p == points[0]
            return constrained_edge[2]
        if p == points[1]
            return constrained_edge[0]
        return constrained_edge[1]
    }

    func getConstrainedEdgeCW(using self, p: *Point)->bool
    {
        if p == points[0]
            return constrained_edge[1]
        if p == points[1]
            return constrained_edge[2]
        return constrained_edge[0]
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

struct Sweep
{
    basin:      Basin
    edge_event: EdgeEvent
    triangles:  Array'*Triangle
    map:        Array'*Triangle
    edge_list:  Array'*Edge
    points:     Array'*Point
    front:      AdvancingFront
    head:       *Point
    tail:       *Point
    af_head:    *Node
    af_middle:  *Node
    af_tail:    *Node
}

impl Sweep
{
    func addPoint(using self, x, y: f64)
    {
        p := Memory.new'Point()
        p.x = x
        p.y = y
        points.add(p)
    }

    func triangulate(using self)
    {
        initTriangulation()
        createAdvancingFront()
        sweepPoints()
        finalizationPolygon()
    }

    func sweepPoints(using self)
    {
        visit p: points
        {
        }
    }

    func finalizationPolygon(using self)
    {
        t := front.head.next.triangle
        p := front.head.next.point
        while t and !t.getConstrainedEdgeCW(p)
            t = t.neighborCCW(p)
        if t meshClean(t)
    }

    func meshClean(triangle: *Triangle)
    {
    }

    func initEdges(using self, polyline: Array'*Point)
    {
        visit pt, i: polyline
        {
            nexti := (i + 1) % @countof(polyline)
            edge := Memory.new'Edge()
            edge.set(polyline[i], polyline[nexti])
            edge_list.add(edge)
        }
    }

    func initTriangulation(using self)
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

    func createAdvancingFront(using self)
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