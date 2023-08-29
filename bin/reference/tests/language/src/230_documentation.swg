/**
The Swag compiler can generate documentation for all the modules of a given workspace.
```
swag doc -w:myWorkspaceFolder
```
Swag can generate documentations in various modes. That mode should be specified in the 'module.swg' file, in the [Swag.BuildCfg] struct.
```swag
#dependencies
{
    #import "pixel"

    #run
    {
        let itf = @compiler()
        let cfg = itf.getBuildCfg()
        cfg.genDoc.kind = .Api // Specify the documentation generation mode
    }
}
```
| Swag.DocKind.Api      | Generates an api documentation (all public symbols)
| Swag.DocKind.Examples | Generates a documentation like this one
| Swag.DocKind.Pages    | Generates different pages, where each file is a page (a variation of 'Examples')
*/

/**
# Markdown files
If the module contains **markdown** files with the '.md' extension, they will be processed as if they were Swag comments.
*/

/**
# Format of comments
*/

/**
## Paragraphs
*/

// Everything between empty lines is considered to be a simple paragraph. Which
// means that if you put several comments on several lines like this, they all
// will be part of the same paragraph.
//
// This is another paragraph because there's an empty line before.
//
// This is yet another paragraph.

/**
A paragraph that starts with `---` is a paragraph where every blanks and end of lines
are respected.
*/

// ---
// Even...
//
// ...empty lines are preserved.
//
// You end that kind of paragraph with another '---' alone on its line.
// ---

/**
## Lists
*/

/**
 You can create a **list** of bullet points with `*`.
 */

// * This is a bullet point
// * This is a bullet point
// * This is a bullet point

/**
* This is a bullet point
* This is a bullet point
* This is a bullet point
*/

/**
## Quotes
*/

/**
You can create a **quote** with `>`
*/

// > This is a block quote on multiple
// > lines.
// >
// > End of the quote.

/**
> This is a block quote on multiple
> lines.
>
> End of the quote.
*/

/**
## Tables
*/

/**
You can create a **table** with `|`.
*/

// A table with 4 lines of 2 columns:
// | 'boundcheck'   | Check out of bound access
// | 'overflow'     | Check type conversion lost of bits or precision
// | 'math'         | Various math checks (like a negative '@sqrt')
// | 'switch'       | Check an invalid case in a '#[Swag.Complete]' switch

/**
| 'boundcheck'   | Check out of bound access
| 'overflow'     | Check type conversion lost of bits or precision
| 'math'         | Various math checks (like a negative '@sqrt')
| 'switch'       | Check an invalid case in a '#[Swag.Complete]' switch
*/

/**
## Code
*/

/**
You can create a simple **code paragraph** with three backticks before and after the code, or just by indenting the code with four blanks or one tabulation.
*/

// ```
// if a == true
//   @print("true")
// ```

//    if a == true
//        @print("true")

/**
```
if a == true
  @print("true")
```
*/

/**
You can create a code paragraph with **syntax coloration** by adding 'swag' after the three backticks.
*/

// ```swag
// if a == true
//   @print("true")
// ```

/**
```swag
if a == true
  @print("true")
```
*/

/**
## Titles
*/

/**
You can define **titles** with '#', '##'...
The real level of the title will depend on the context and the generated documentation kind.
*/

// # Title 1
// ## Title 2
// ### Title 3

/**
## References
*/

/**
You can create an external **reference** with `\[name\](link)`.
*/

// This is a [reference](https://github.com/swag-lang/swag) to the Swag repo on GitHub.

/**
> This is a [reference](https://github.com/swag-lang/swag) to the Swag repo on GitHub.
*/

/**
## Markdown
*/

/**
Some other markers are also supported inside texts.
*/

// This is `inline code` with back ticks.
// This is inline 'code' with normal ticks, but just for a single word.
// This is **bold**.
// This is *italic*.
// This character \n is escaped, and 'n' will be output as is.

/**
> This is `inline code` with back ticks.
> This is inline 'code' with normal ticks, but just for a single word.
> This is **bold**.
> This is *italic*.
> This character \n is escaped, and 'n' will be output as is.
*/