/**
# Tuples in Swag

In Swag, a tuple represents an anonymous structure, sometimes referred to as a struct literal. Tuples are particularly useful for grouping together elements of various types without requiring a formally defined structure. The syntax for creating a tuple involves enclosing the elements within curly braces `{}`.
*/
#test
{
    let tuple1 = {2, 2}                    // A tuple containing two integer elements.
    let tuple2 = {"string", 2, true}       // A tuple with a string, an integer, and a boolean element.
}

/**
# Accessing Tuple Values

By default, the values in a tuple are accessed using automatically generated field names. These names follow the pattern `itemX`, where `X` represents the zero-based index of the element within the tuple.
*/
#test
{
    let tuple = {"string", 2, true}        // A tuple containing three elements.
    @assert(tuple.item0 == "string")       // Accesses the first element (index 0).
    @assert(tuple.item1 == 2)              // Accesses the second element (index 1).
    @assert(tuple.item2 == true)           // Accesses the third element (index 2).
}

/**
# Named Fields in Tuples

In Swag, it is possible to assign custom names to tuple fields, enhancing the readability and maintainability of the code. When custom names are assigned, the default `itemX` access pattern remains available, offering flexibility in how you interact with tuple elements.
*/
#test
{
    let tuple = {x: 1.0, y: 2.0}           // A tuple with named fields 'x' and 'y'.
    @assert(tuple.x == 1.0)                // Accesses the value of 'x' via its custom name.
    @assert(tuple.item0 == 1.0)            // Accesses the value of 'x' using the default 'item0'.
    @assert(tuple.y == 2.0)                // Accesses the value of 'y' via its custom name.
    @assert(tuple.item1 == 2.0)            // Accesses the value of 'y' using the default 'item1'.
}

/**
# Automatic Naming of Tuple Fields

When creating a tuple using variables, Swag automatically assigns the field names based on the variable names, unless explicitly overridden. This feature simplifies tuple creation by directly reflecting variable names within the tuple structure.
*/
#test
{
    let x = 555
    let y = 666
    let t = {x, y}                         // Tuple fields are automatically named 'x' and 'y'.
    @assert(t.x == 555)                    // Accesses the value of 'x' using the variable name.
    @assert(t.item0 == t.x)                // 'item0' corresponds to the variable 'x'.
    @assert(t.y == 666)                    // Accesses the value of 'y' using the variable name.
    @assert(t.item1 == t.y)                // 'item1' corresponds to the variable 'y'.
}

/**
# Tuple Assignment and Compatibility

Swag provides flexibility in assigning tuples to each other, even if the field names differ. As long as the field types are compatible, tuples can be assigned across different structures, facilitating easy data transfer and manipulation.
*/
#test
{
    var x: { a: s32, b: s32 }              // A tuple with fields 'a' and 'b'.
    var y: { c: s32, d: s32 }              // A tuple with fields 'c' and 'd'.

    y = {1, 2}                             // Initializes 'y' with integer values.
    x = y                                  // Assigns 'y' to 'x', field names do not need to match.
    @assert(x.a == 1)                      // 'x.a' acquires the value of 'y.c'.
    @assert(x.b == 2)                      // 'x.b' acquires the value of 'y.d'.

    // Although 'x' and 'y' have different field names, their types are not identical.
    #assert(#typeof(x) != #typeof(y))
}

/**
# Tuple Unpacking

Swag allows tuples to be unpacked, meaning their individual elements can be extracted into separate variables. This feature is particularly useful for disaggregating a tuple's data for specific operations or further processing.
*/
#test
{
    let tuple1 = {x: 1.0, y: 2.0}          // A tuple with fields 'x' and 'y'.

    // Unpacking the tuple: 'value0' gets the value of 'x', and 'value1' gets the value of 'y'.
    let (value0, value1) = tuple1
    @assert(value0 == 1.0)
    @assert(value1 == 2.0)

    let tuple2 = {"name", true}            // A tuple containing a string and a boolean.
    let (name, value) = tuple2             // Unpacking into 'name' and 'value'.
    @assert(name == "name")
    @assert(value == true)
}

/**
# Ignoring Tuple Fields During Unpacking

When unpacking a tuple, you can choose to ignore specific fields using the `?` placeholder. This is particularly useful when only certain elements of a tuple are needed, allowing you to disregard others efficiently.
*/
#test
{
    let tuple1 = {x: 1.0, y: 2.0}          // A tuple with fields 'x' and 'y'.
    let (x, ?) = tuple1                    // Unpacks 'x' and ignores the second field.
    @assert(x == 1.0)
    let (?, y) = tuple1                    // Ignores the first field and unpacks 'y'.
    @assert(y == 2.0)
}
