/**
In 'Swag.DocKind.Api' mode, swag will collect all **public definitions** to generate the documentation. [Std.Core](std.core.php) is an example of documentation generated in that mode.
*/

/**
The main module documentation should be placed at the top of the corresponding `module.swg` file.
```swag
// This is the main module documentation.
#dependencies
{
}
```
*/

/**
Other comments need to be placed just before a function, struct or enum.

The first paragraph is considered to be the **short description** which can appear on specific parts
of the documentation. So make it... short.

If the first line ends with a dot `.`, then this marks the end of the paragraph, i.e. the end of the short description.
*/

// This first paragraph is the short description of function 'test1'
//
// This second paragraph should be the long description.
func test1()
{
}

// This is the short description of 'test'.
// As the previous first line ends with '.', this is another paragraph, so this should be
// the long description. No need for an empty line before.
func test()
{
}

/**
For constants or enum values, the document comment is the one declared at the end of the line.
*/

const A = 0     // This is the documentation comment of constant 'A'

enum Color
{
    Red         // This is the documentation comment of enum value 'Red'
    Blue        // This is the documentation comment of enum value 'Blue'
}

/**
# References
You can create a **reference** to something in the current module with `[name]` or `[name1.name2 etc.]`
*/

// This is a function with a 'value' parameter.
func one(value: s32)
{
}

// This is a reference to [one]
func two()
{
}

/**
# NoDoc
You can use the `#[Swag.NoDoc]` attribute to prevent a certain element from showing up in the documentation.
*/

// The function 'one' will be ignored when generating the documentation.
#[Swag.NoDoc]
func one()
{
}
