The Swag compiler is capable of generating comprehensive documentation for all modules within a specified workspace.

To generate documentation for your workspace, use the following command:

    swag doc -w:myWorkspaceFolder

Swag supports various documentation generation modes, which should be specified in the module.swg file within the [[Swag.BuildCfg]] structure.

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

| Kind                  | Purpose
| --------------------- | -------
| Swag.DocKind.Api      | Generates an api documentation (all public symbols)
| Swag.DocKind.Examples | Generates a documentation like this one
| Swag.DocKind.Pages    | Generates different pages, where each file is a page (a variation of  'Examples')

# Markdown files

If the module contains **markdown** files with the '.md' extension, they will be processed as if they were Swag comments.

# Format of comments

## Paragraphs

```swag
// Everything between empty lines is considered to be a simple paragraph. Which
// means that if you put several comments on several lines like this, they all
// will be part of the same paragraph.
//
// This is another paragraph because there's an empty line before.
//
// This is yet another paragraph.
```

> EXAMPLE:
> Everything between empty lines is considered to be a simple paragraph. Which
> means that if you put several comments on several lines like this, they all
> will be part of the same paragraph.
>
> This is another paragraph because there's an empty line before.
>
> This is yet another paragraph.

Inside a paragraph, you can end of line with '\\' to force a break without creating a new paragraph.

```swag
// First line.
// Second line is on first line.\
// But third line has a break before.
```

> EXAMPLE:
> First line.
> Second line is on first line.\
> But third line has a break before.

A paragraph that starts with `<html>` is a **verbatim** paragraph where every blanks and end of lines
are respected. The paragraph will be generated **as is** without any markdown change.

```swag
// <html>
// Even...
//
// ...empty lines are preserved.
//
// You end that kind of paragraph with '</html>' alone on its line.
// Note that **everything** is not bold, put printed 'as it is'.
// </html>
```

> EXAMPLE:
> <html>
> Even...
>
> ...empty lines are preserved.
>
> You end that kind of paragraph with '</html>' alone on its line.
> Note that **everything** is not bold, put printed 'as it is'.
> </html>

## Horizontal Lines

Use '---' at the start of a line to create an HTML horizontal line ('<br>')

## Lists

You can create a **list** of bullet points with `\*`.

```swag
// * This is a bullet point
// * This is a bullet point
// * This is a bullet point
```

> EXAMPLE:
> * This is a bullet point
> * This is a bullet point
> * This is a bullet point

```swag
// - This is a bullet point
// - This is a bullet point
// - This is a bullet point
```

> EXAMPLE:
> - This is a bullet point
> - This is a bullet point
> - This is a bullet point

You can create an **ordered** list by starting the line with a digit followed by a '.'.

```swag
// 1. This is an ordered list
// 1. The digit itself does not matter, real numbers will be computed
// 0. This is another one
```

> EXAMPLE:
> 1. This is an ordered list
> 1. The digit itself does not matter, real numbers will be computed
> 0. This is another one

> WARNING:
> Swag only supports single line list items. You cannot have complex paragraphs (or sub lists).

## Definition Lists

You can add a definition title with the '+' character followed by a blank, and then the title.
The description paragraph should come just after the title, with at least 4 blanks or one tabulation.

```swag
// + Title
//     This is the description.
// + Other title
//     This is the other description.
```

> EXAMPLE:
> + Title
>     This is the description.
> + Other title
>     This is the other description.

A description can contain complex paragraphs.

```swag
// + Title
//     This is an embedded list.
//     * Item1
//     * Item2
```

> EXAMPLE:
> + Title
>     This is an embedded list.
>     * Item1
>     * Item2

The description paragraph can contain some empty lines.

```swag
// + Other title
//
//     This is the other description
//     on more than one line.
```

> EXAMPLE:
> + Other title
>
>     This is the other description
>     on more than one line.

## Quotes

You can create a **quote** with `>`

```swag
// > This is a block quote on multiple
// > lines.
// >
// > End of the quote.
```

> This is a block quote on multiple
> lines.
>
> End of the quote.

You can create a special quote by adding a title on the first line. There must be exactly one blank between `>` and the title, and the title case should be respected.

* `NOTE:`
* `TIP:`
* `WARNING:`
* `ATTENTION:`
* `EXAMPLE:`

```swag
// > NOTE:
// > This is the note content
```

> NOTE:
> This is the note content

```swag
// > TIP:
// > This is a tip.
```

> TIP:
> This is a tip.

```swag
// > WARNING:
// > This is the warning content
// >
// > Another paragraph
```

> WARNING:
> This is the warning content
>
> Another paragraph

```swag
// > ATTENTION: The content of the quote can be written on the same line as the title
```

> ATTENTION: The content of the quote can be written on the same line as the title

```swag
// > EXAMPLE:
// > In the 'module.swg' file, we have changed the 'example' title to be `"Result"` instead of `"Example"`.
```

> EXAMPLE:
> In the 'module.swg' file, we have changed the 'example' title to be `"Result"` instead of `"Example"`.

## Tables

You can create a **table** by starting a line with '|'. Each column must then be separated with '|'. The last column can end with '|', but this is not mandatory.

```swag
// A table with 4 lines of 2 columns:
// | boundcheck   | Check out of bound access
// | overflow     | Check type conversion lost of bits or precision
// | math         | Various math checks (like a negative '@sqrt')        |
// | switch       | Check an invalid case in a '#[Swag.Complete]' switch |
```

> EXAMPLE:
> | boundcheck   | Check out of bound access
> | overflow     | Check type conversion lost of bits or precision
> | math         | Various math checks (like a negative '@sqrt')        |
> | switch       | Check an invalid case in a '#[Swag.Complete]' switch |

You can define a header to the table if you separate the first line from the second one with a separator like '---'. A valid separator must have a length of at least 3 characters, and must only contain '-' or ':'.

```swag
// | Title1 | Title2
// | ------ | ------
// | Item1  | Item2
// | Item1  | Item2
```

> EXAMPLE:
> | Title1 | Title2
> | ------ | ------
> | Item1  | Item2
> | Item1  | Item2

You can define the **column alignment** by adding ':' at the start and/or at the end of a separator.

```swag
// | Title1     | Title2       | Title3
// | :-----     | :----:       | -----:
// | Align left | Align center | Align right
```

> EXAMPLE:
> | Title1     | Title2       | Title3
> | :-----     | :----:       | -----:
> | Align left | Align center | Align right

## Code

You can create a simple **code paragraph** with three backticks before and after the code.

```swag
// ```
// if a == true
//   @print("true")
// ```
```

> EXAMPLE:
> ```
> if a == true
>   @print("true")
> ```

You can also create that kind of paragraph by simply indenting the code with four blanks or one tabulation.

```swag
//    if a == false
//        @print("false")
```

And if you want **syntax coloration**, add 'swag' after the three backticks. Only Swag syntax is supported right now.

```swag
// ```swag
// if a == true
//   @print("true")
// ```
```

> EXAMPLE:
> ```swag
> if a == true
>   @print("true")
> ```

## Titles

You can define **titles** with '#', '##' ... followed by a blank, and then the text.
The real level of the title will depend on the context and the generated documentation kind.

```swag
// # Title 1
// ## Title 2
// ### Title 3
// #### Title 4
// ##### Title 5
// ###### Title 6
```

## References

You can create an external **reference** with `\[name\](link)`.

```swag
// This is a [reference](https://github.com/swag-lang/swag) to the Swag repository on GitHub.
```

> EXAMPLE:
> This is a [reference](https://github.com/swag-lang/swag) to the Swag repository on GitHub.

## Images

You can insert an external **image** with `!\[name\](link)`.

```swag
// This is an image ![image](https://swag-lang/imgs/swag_icon.png).
```

## Markdown

Some other markers are also supported inside texts.

```swag
// This is `inline code` with back ticks.\
// This is inline 'code' with normal ticks, but just for a single word (no blanks).\
// This is **bold**.\
// This is *italic*.\
// This is ***bold and italic***.\
// This is ~~strikethrough~~.\
// This character \n is escaped, and 'n' will be output as is.\
```

> EXAMPLE:
> This is `inline code` with back ticks.\
> This is inline 'code' with normal ticks, but just for a single word (no blanks).\
> This is **bold**.\
> This is *italic*.\
> This is ***bold and italic***.\
> This is ~~strikethrough~~.\
> This character \n is escaped, and 'n' will be output as is.\