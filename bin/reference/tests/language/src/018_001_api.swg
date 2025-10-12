/**
# Documentation Generation in Swag

When using `Swag.DocKind.Api` mode, Swag collects all **public definitions**
to automatically generate API documentation.

A good example of this is the documentation for [Std.Core](std.core.php).
*/

/**
# Module-Level Documentation

The main module documentation should be placed at the top of the `module.swg` file.

```swag
// This is the main module documentation.
#dependencies
{
}
````

*/

/**

# Element-Level Documentation

Comments placed immediately before a `func`, `struct`, or `enum`
become their documentation entries.

The **first paragraph** acts as the *short description* that appears in summaries.

If the first line ends with a dot `.`, it marks the end of the short description.
*/

// This first paragraph is the short description of function 'test1'.
//
// This second paragraph serves as the long description.
func test1() {}

// This is the short description of 'test'.
// As the previous line ends with '.', the following text becomes
// the long description, even without a blank line.
func test() {}

/**

# Inline Comments for Constants and Enums

Constants and enum values can include documentation comments on the same line.
*/
const A = 0 // This is the documentation comment of constant 'A'

enum Color
{
    Red      // This is the documentation comment of enum value 'Red'
    Blue     // This is the documentation comment of enum value 'Blue'
}

/**

# References

You can reference other documented elements using `[[name]]`
or fully qualified forms like `[[name1.name2]]`.
*/

// This is a function with a 'value' parameter.
func one(value: s32) {}

// This is a reference to [[one]].
func two() {}

/**

# Hiding from Documentation

Use the `#[Swag.NoDoc]` attribute to exclude an element from the generated documentation.
*/

// The function 'three' will be ignored by the documentation generator.
#[Swag.NoDoc]
func three() {}
