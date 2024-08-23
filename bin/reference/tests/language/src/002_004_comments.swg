/**
Swag supports both single-line and multi-line comments, allowing for flexible documentation 
and inline explanations within your code. This capability is essential for maintaining clarity 
and readability, especially in complex codebases.
*/

// Example of a single-line comment
// Single-line comments are typically used for brief explanations or notes.

/*
    Example of a multi-line comment
    that spans several lines.
    
    Multi-line comments are useful when more detailed explanations are required. 
    These comments can describe the overall purpose of a function, document complex 
    logic, or provide additional context to developers.
*/

const A = 0                 // A constant named 'A' is declared and assigned the value '0'.
const B = /* false */ true  // A constant named 'B' is assigned the value 'true', with an inline 
                            // comment indicating that 'false' was an alternative considered value.

/**
Swag also supports nested comments within multi-line comments.
This feature can be particularly useful when temporarily disabling a block of code 
or when adding additional notes inside an existing comment block.
*/

/*
    /* Example of a nested multi-line comment */
    The nested comment above is encapsulated within another multi-line comment.
    This demonstrates Swag's ability to handle complex comment structures without 
    causing errors or ambiguity.
*/

