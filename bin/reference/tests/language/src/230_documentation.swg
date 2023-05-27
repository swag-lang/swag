/**
The Swag compiler can generate documentation for all the modules of a given workspace.
```
swag doc -w:myWorkspaceFolder
```
*/

/**
The documentation comment needs to be placed just before a function, struct or enum.
*/
#test
{
    // Everything between empty lines is considered to be a simple paragraph.
    // Which means that if you put several comments on several lines like
    // this, they all will be part of the same paragraph.
    //
    // This is another paragraph because there's an empty line before.
    //
    // ---
    // A paragraph that starts with '---' is a paragraph where every blanks and end of lines
    // are respected. Even...
    //
    // ...empty lines are preserved.
    // You end that kind of paragraph with another '---' alone on its line.
    // ---
    func test()
    {

    }
}

/**
For constants or enum values, the document comment is the one declared at the end of the line.
*/
#test
{
    const A = 0 // This is a documentation comment
    enum Color
    {
        Red // This is a documentation comment
    }
}

 /**
 Some other markdown markers are also supported.
 */
 #test
 {
    // `this is code` (backtick) for 'inline' code.
    // 'single_word'  (tick) for 'inline' code.
    // **bold**
    struct RGB
    {
        r, g, b: s32
    }
}

/**
You can create a reference to something of the current package with [name] or [name1.name2 etc.]
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