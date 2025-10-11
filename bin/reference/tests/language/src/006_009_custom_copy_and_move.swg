/**
Swag supports both copy and move semantics for structures. In this example, we demonstrate these concepts using a `Vector3` struct. Although a `Vector3` struct typically wouldn't require move semantics (as it doesn't involve heap allocation), this example serves to illustrate how these features are implemented and can be utilized within the Swag language.
*/

struct Vector3 { x, y, z: s32 = 666 }

impl Vector3
{
    // This method is invoked following a copy operation.
    // It exemplifies "copy semantics" and can be customized for specific behaviors post-copy.
    mtd opPostCopy()
    {
        me.x, me.y, me.z += 1
        // Increment all fields by 1 to signify a copy operation has occurred.
    }

    // This method is invoked following a move operation.
    // It represents "move semantics" and can be customized for specific behaviors post-move.
    mtd opPostMove()
    {
        me.x, me.y, me.z += 2
        // Increment all fields by 2 to signify a move operation has occurred.
    }

    // This method is invoked when an object is about to be destroyed.
    // While `Vector3` does not manage resources, this is where resource cleanup would typically occur, such as releasing heap-allocated memory.
    mtd opDrop() {}
}

#test
{
    var a = Vector3{}                  // Default initialization of `a`.
    var b = Vector3{100, 200, 300}     // Custom initialization of `b`.

    // Copy Semantics (default behavior):
    // 1. If 'a' already holds a value, 'opDrop' on 'a' is invoked.
    // 2. 'b' is copied to 'a'.
    // 3. 'opPostCopy' on 'a' is invoked to finalize the copy operation.

    a = b
    @assert(a.x == 101) // +1 due to 'opPostCopy'.
    @assert(a.y == 201)
    @assert(a.z == 301)

    // Move Semantics:
    // The `#move` modifier triggers move semantics:
    // 1. 'opDrop' on 'a' is invoked (if it exists).
    // 2. 'b' is moved to 'a'.
    // 3. 'opPostMove' on 'a' is invoked to finalize the move operation.
    // 4. 'b' is reinitialized to default values (666) if 'opDrop' exists.

    a = #move b
    @assert(a.x == 102) // +2 due to 'opPostMove'.
    @assert(a.y == 202)
    @assert(a.z == 302)

    // Post-move, 'b' is reinitialized to its default values (666) as 'opDrop' is present.
    @assert(b.x == 666)

    // The `#nodrop` modifier bypasses the initial 'opDrop' invocation.
    // Use this when 'a' is in an undefined state and does not require cleanup.

    a = #nodrop b // Copy 'b' to 'a' without invoking 'opDrop' on 'a' first.
    a = #nodrop #move b // Move 'b' to 'a' without invoking 'opDrop' on 'a' first.

    // The `#moveraw` modifier prevents the reinitialization of 'b' after the move.
    // This approach is risky and should be employed only when certain that 'b' won't be dropped or
    // when reinitialization is handled manually.

    a = #moveraw b // Move 'b' to 'a' without resetting 'b'.
    a = #nodrop #moveraw b // Move 'b' to 'a' without invoking 'opDrop' on 'a' first and without resetting 'b'.
}

/**
# Move Semantics in Functions
Move semantics can be explicitly indicated in function parameters by utilizing `&&` instead of `&`.
*/

#test
{
    // This variant of 'assign' takes ownership of 'from' by moving its contents into 'assignTo'.
    // The 'moveref' keyword informs the compiler that this version of 'assign' assumes ownership of 'from'.

    #[Swag.Overload]
    func assign(assignTo: &Vector3, from: &&Vector3)
    {
        assignTo = #move from // Move 'from' into 'assignTo'.
    }

    // This variant of 'assign' performs a copy instead of a move.
    // 'from' remains unchanged and is passed by value.
    #[Swag.Overload]
    func assign(assignTo: &Vector3, from: Vector3)
    {
        assignTo = from // Copy 'from' into 'assignTo'.
    }

    var a  = Vector3{1, 2, 3}     // Initialize 'a'.
    var b: Vector3                // Declare 'b'.

    // Invoke the copy version of 'assign'.
    assign(&b, a)
    @assert(b.x == 2 and b.y == 3 and b.z == 4) // +1 on each field due to 'opPostCopy'.
    @assert(a.x == 1 and a.y == 2 and a.z == 3) // 'a' remains unchanged.

    // Invoke the move version of 'assign' using 'moveref'.
    assign(&b, moveref &a)
    @assert(b.x == 3 and b.y == 4 and b.z == 5) // +2 on each field due to 'opPostMove'.
    @assert(a.x == 666 and a.y == 666 and a.z == 666) // 'a' is reset to default values post-move.
}
