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
    front:      *AdvancingFront
    head:       *Point
    tail:       *Point
    af_head:    *Node
    af_middle:  *Node
    af_tail:    *Node
}

impl Sweep
{
    func initEdges(using self, polyline: Array'(*Point))
    {
        visit pt, i: polyline
        {
            nexti := (i + 1) % polyline.count
            edge := Memory.new'Edge()
            edge.set(polyline[i], polyline[nexti])
            edge_list.add(edge)
        }
    }

    func initTriangulation(using self)
    {

    }
}