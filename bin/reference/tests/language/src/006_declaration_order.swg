/**
In Swag, the order of all **top level** declarations does not matter.
*/

// Here we declare a constant 'A' and initialize it with 'B', which is not yet known (neither its value or its type).
const A = B
// Declare a constant 'B' equals to 'C', still unknown at this point.
const B = C
// Declare a constant C of type 'u64' (unsigned 64 bits integer) and assigned it to 1.
// At this point A and B are then also defined.
const C: u64 = 1

/**
In this test, we call the function `functionDeclaredLater` before it is known. This is fine.
*/

#test
{
    functionDeclaredLater()
}

func functionDeclaredLater()
{
}

/**
Note that the order is not relevant in the same file, but it is also irrelevant across multiple files. You can for example call a function in one file and declare it in another one. Ordrer does not matter !
*/