In `Swag.DocKind.Examples` mode, documentation is generated systematically, with each file representing a chapter or subchapter. The following guidelines outline the structure and formatting required for effective documentation creation.

# File Naming Convention

File names must adhere to the format `DDD_DDD_name`, where each `D` represents a digit. This naming convention facilitates the hierarchical organization of the documentation.

+ Main Titles
    Files with names formatted as `100_000_my_title.swg` will generate a main heading (`<h1>My Title</h1>`).
+ Subtitles
    Files named `101_001_my_sub_title.swg` or `102_002_my_sub_title.swg` will generate subheadings (`<h2>My Sub Title</h2>`) under the corresponding main title.
+ Multiple Main Titles
    For separate sections, such as `110_000_my_other_title.swg`, another main heading (`<h1>My Other Title</h1>`) will be generated.
+ File Type Flexibility
    You can mix `.swg` files with `.md` files. For example, `111_000_my_other_title.md` will seamlessly integrate Markdown files into the documentation structure.

# Comment Format for Documentation

To include comments in your code that should be interpreted as part of the documentation (as opposed to standard Swag comments), use the following syntax:

```plaintext
/**
 This is a valid documentation comment.
 The comment must start with /** and end with */, each on a separate line.
 */
```

These comments will be processed and included in the generated documentation, ensuring that inline comments are properly formatted and contribute to the final output.

# Source of Documentation

The documentation you are reading is generated from the [std/reference/language](https://github.com/swag-lang/swag/tree/master/bin/reference/tests/language) module. This directory contains examples and files structured according to these guidelines, showcasing how to effectively create and manage documentation in `Swag.DocKind.Examples` mode.
