/**
# Comments

Swag supports both single-line and multi-line comments, providing flexibility for
inline documentation and explanations within your code. This helps maintain clarity
and readability, especially in larger or more complex codebases.
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

const A = 0        // A constant named 'A' assigned the value '0'.
const B = /* false */true // A constant named 'B' assigned the value 'true', with an inline
// comment noting that 'false' was an alternative value considered.

/**
# Nested Comments

Swag also supports nested comments within multi-line comments. This can be particularly
useful when temporarily disabling a block of code or when adding notes inside an
existing comment block.
*/

/*
    /* Example of a nested multi-line comment */
    The nested comment above is encapsulated within another multi-line comment.
    This demonstrates Swag's ability to handle complex comment structures without
    causing errors or ambiguity.
*/
