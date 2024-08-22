/**
Swag supports both copy and move semantics for structures. In this example, we'll demonstrate these concepts using a `Vector3` struct. Although a `Vector3` struct typically wouldn't require move semantics (since it doesn't involve heap allocation), this example serves to illustrate how these features work in Swag.
*/

struct Vector3
{
    x, y, z: s32 = 666
}

impl Vector3
{
    // This method is invoked after a copy operation.
    // It represents the "copy semantics" and is useful for custom behavior after copying.
    mtd opPostCopy()
    {
        x, y, z += 1  // Increment all fields by 1 to indicate a copy has occurred.
    }

    // This method is invoked after a move operation.
    // It represents the "move semantics" and is useful for custom behavior after moving.
    mtd opPostMove()
    {
        x, y, z += 2  // Increment all fields by 2 to indicate a move has occurred.
    }

    // This method is invoked when an object is about to be destroyed.
    // Even though `Vector3` doesn't manage resources, this is where you would clean up resources like heap-allocated memory.
    mtd opDrop() {}
}

#test
{
    var a = Vector3{}           // Default initialization
    var b = Vector3{100, 200, 300}  // Custom initialization

    // Copy Semantics (default behavior)
    // 1. Calls 'opDrop' on 'a' (if it exists), because 'a' might already hold a value.
    // 2. Copies 'b' to 'a'.
    // 3. Calls 'opPostCopy' on 'a' (if it exists).
    a = b
    @assert(a.x == 101)  // +1 because of 'opPostCopy'
    @assert(a.y == 201)
    @assert(a.z == 301)

    // Move Semantics
    // The `#move` modifier triggers move semantics:
    // 1. Calls 'opDrop' on 'a' (if it exists).
    // 2. Moves 'b' to 'a'.
    // 3. Calls 'opPostMove' on 'a' (if it exists).
    // 4. Reinitializes 'b' to default values (if 'opDrop' exists).
    a = #move b
    @assert(a.x == 102)  // +2 because of 'opPostMove'
    @assert(a.y == 202)
    @assert(a.z == 302)

    // After the move, 'b' is reinitialized to default values (666) because 'opDrop' exists.
    @assert(b.x == 666)

    // You can bypass the first 'opDrop' call with the `#nodrop` modifier.
    // Use this when you know 'a' is in an undefined state and doesn't require cleanup.
    a = #nodrop b         // Copy 'b' to 'a' without dropping 'a' first.
    a = #nodrop #move b   // Move 'b' to 'a' without dropping 'a' first.

    // For move semantics, you can avoid the reinitialization of 'b' using `#moveraw`.
    // This is risky and should only be used if you're certain 'b' won't be dropped or if you plan to reinitialize it manually.
    a = #moveraw b        // Move 'b' to 'a' without resetting 'b'.
    a = #nodrop #moveraw b // Move 'b' to 'a' without dropping 'a' first and without resetting 'b'.
}

/**
# Move Semantics in Functions
You can indicate a move semantic intention in function parameters by using `&&` instead of `&`.
*/

#test
{
    // This version of 'assign' takes ownership of 'from' by moving its contents into 'assignTo'.
    // The 'moveref' tells the compiler that this version takes ownership of 'from'.
    #[Swag.Overload]
    func assign(assignTo: &Vector3, from: &&Vector3)
    {
        assignTo = #move from  // Move 'from' to 'assignTo'.
    }

    // This version of 'assign' performs a copy instead of a move.
    // 'from' remains unchanged, so it's passed by value.
    #[Swag.Overload]
    func assign(assignTo: &Vector3, from: Vector3)
    {
        assignTo = from  // Copy 'from' to 'assignTo'.
    }

    var a = Vector3{1, 2, 3}  // Initialize 'a'.
    var b: Vector3            // Declare 'b'.

    // Call the copy version of 'assign'.
    assign(&b, a)
    @assert(b.x == 2 and b.y == 3 and b.z == 4)  // +1 on each field due to 'opPostCopy'.
    @assert(a.x == 1 and a.y == 2 and a.z == 3)  // 'a' remains unchanged.

    // Call the move version of 'assign' using 'moveref'.
    assign(&b, moveref &a)
    @assert(b.x == 3 and b.y == 4 and b.z == 5)  // +2 on each field due to 'opPostMove'.
    @assert(a.x == 666 and a.y == 666 and a.z == 666)  // 'a' is reset to default values after the move.
}
