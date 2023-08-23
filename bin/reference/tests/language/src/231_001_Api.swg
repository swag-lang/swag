/**
In 'Swag.DocKind.Api' mode, swag will collect all **public definitions** to generate the documentation. [Std.Core](std.core.php) is an example of documentation generated in that mode.
*/

/**
The main module documentation should be placed at the top of the corresponding `module.swg` file.
```
// This is the main module documentation.
#dependencies
{
}
```
*/

/**
Other comments need to be placed just before a function, struct or enum.
*/
#test
{
    // Everything between empty lines is considered to be a simple paragraph. Which
    // means that if you put several comments on several lines like this, they all
    // will be part of the same paragraph.
    //
    // This is another paragraph because there's an empty line before.
    //
    // This is yet another paragraph.
}


/**
For constants or enum values, the document comment is the one declared at the end of the line.
*/
#test
{
    const A = 0     // This is a documentation comment
    enum Color
    {
        Red         // This is a documentation comment
    }
}
