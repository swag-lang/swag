/**
Swag supports both copy and move semantics for structures. In this example, we demonstrate these concepts using a 'Vector3' struct. Although a 'Vector3' typically wouldn't require move semantics (no heap allocation), this illustrates how these features can be implemented and used in Swag.
*/

struct Vector3 { x, y, z: s32 = 666 }

impl Vector3
{
    // Called after a copy; customize post-copy behavior here.
    mtd opPostCopy()
    {
        .x, .y, .z += 1
    }

    // Called after a move; customize post-move behavior here.
    mtd opPostMove()
    {
        .x, .y, .z += 2
    }

    // Called before destruction; place cleanup here if needed.
    mtd opDrop() {}
}

#test
{
    var a = Vector3{}                  // Default init.
    var b = Vector3{100, 200, 300}     // Custom init.

    // Copy semantics: drop 'a' (if needed), copy 'b' to 'a', then call 'opPostCopy' on 'a'.
    a = b
    @assert(a.x == 101)
    @assert(a.y == 201)
    @assert(a.z == 301)

    // Move semantics with '#move': drop 'a' (if needed), move 'b' to 'a', then call 'opPostMove' on 'a'.
    // With 'opDrop' present, 'b' is reinitialized to defaults (666).
    a = #move b
    @assert(a.x == 102)
    @assert(a.y == 202)
    @assert(a.z == 302)
    @assert(b.x == 666)

    // '#nodrop' skips the initial drop of 'a'.
    a = #nodrop b // Copy without dropping 'a' first.
    a = #nodrop #move b // Move without dropping 'a' first.

    // '#moveraw' prevents reinitialization of the source after move. Use with care.
    a = #moveraw b
    a = #nodrop #moveraw b
}

/**
# Move Semantics in Functions

Move semantics can be expressed in function parameters using '&&' instead of '&'.
*/

#test
{
    // Move overload: takes ownership of 'from' and moves into 'assignTo'.
    #[Swag.Overload]
    func assign(assignTo: &Vector3, from: &&Vector3)
    {
        assignTo = #move from
    }

    // Copy overload: copies 'from' into 'assignTo'.
    #[Swag.Overload]
    func assign(assignTo: &Vector3, from: Vector3)
    {
        assignTo = from
    }

    var a  = Vector3{1, 2, 3}
    var b: Vector3

    // Copy path.
    assign(&b, a)
    @assert(b.x == 2 and b.y == 3 and b.z == 4) // +1 via 'opPostCopy'.
    @assert(a.x == 1 and a.y == 2 and a.z == 3) // 'a' unchanged.

    // Move path using 'moveref'.
    assign(&b, moveref &a)
    @assert(b.x == 3 and b.y == 4 and b.z == 5) // +2 via 'opPostMove'.
    @assert(a.x == 666 and a.y == 666 and a.z == 666) // 'a' reset to defaults after move.
}
