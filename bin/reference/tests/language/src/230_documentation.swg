/**
The Swag compiler can generate documentation for all the modules of a given workspace.
```raw
swag doc -w:myWorkspaceFolder
```
Swag can generate documentations in various modes. That mode should be specified in the 'module.swg' file, in the [Swag.BuildCfg] struct.
```
#dependencies
{
    #import "pixel"

    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.docKind = .Api // Specify the documentation generation mode
    }
}
```
| Swag.DocKind.Api      | Generates an api documentation (all public symbols)
| Swag.DocKind.Examples | Generates a documentation like this one
| Swag.DocKind.Pages    | Generates different pages, where each file is a page (a variation of 'Examples')
*/

/**
# Format of comments
The first paragraph is considered to be the 'short description' which can appear on specific parts
of the documentation. So make it short.

If the first line ends with a dot `.`, then this marks the end of the paragraph, i.e. the end of the short description.
*/
#test
{
    // This is the short description.
    // As the previous first line ends with '.', this is another paragraph, so this should be
    // the long description. No need for an empty line before.
    func test()
    {
    }
}

/**
A paragraph that starts with `---` is a paragraph where every blanks and end of lines
are respected.
*/
#test
{
    // ---
    // Even...
    //
    // ...empty lines are preserved.
    //
    // You end that kind of paragraph with another '---' alone on its line.
    // ---
    func test()
    {
    }
}

/**
 You can create a **list** of bullet points with `*`.
 */
 #test
 {
    // * This is a bullet point
    // * This is a bullet point
    // * This is a bullet point
    struct RGB
    {
        r, g, b: s32
    }
}

/**
You can create a **quote** with `>`
*/
#test
{
    // This is the short description.
    // > This is a block quote on multiple
    // > lines.
    // >
    // > End of the quote.
    struct RGB
    {
        r, g, b: s32
    }
}

/**
You can create a **table** with `|`.
*/
#test
{
    // A table with 4 lines of 2 columns:
    // | 'boundcheck'   | Check out of bound access
    // | 'overflow'     | Check type conversion lost of bits or precision
    // | 'math'         | Various math checks (like a negative '@sqrt')
    // | 'switch'       | Check an invalid case in a '#[Swag.Complete]' switch
    func myFunc()
    {
    }
}

/**
You can create a **code paragraph** with three backticks.
*/
#test
{
    // For example:
    // ```
    // if a == true
    //   @print("true")
    // ```
    func test()
    {
    }
}

/**
You can create a code paragraph **without** syntax coloration by adding 'raw' after the three backticks.
*/
#test
{
    // For example:
    // ```raw
    // if a == true
    //   @print("true")
    // ```
    func test()
    {
    }
}

/**
Some other markdown markers (or kind of) are also supported inside paragraphs.
*/
#test
{
    // `this is code` (backtick) for 'inline' code.
    // 'single_word'  (tick) for 'inline' code.
    // *italic*
    // **bold**
    // # Title
    // ## Title
    // ### Title
    struct RGB
    {
        r, g, b: s32
    }
}

/**
# References
You can create a reference to something in the current module with `[name]` or `[name1.name2 etc.]`
*/
#test
{
    // This is a function with a 'value' parameter.
    func one(value: s32)
    {
    }

    // This is a reference to [one]
    func two()
    {
    }
}

/**
The attribute `#[Swag.NoDoc]` can be used to avoid a given element to appear in the documentation.
*/
#test
{
    // This function will be ignored when generating documentation.
    #[Swag.NoDoc]
    func one()
    {
    }
}