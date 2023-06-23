# Markdown Syntax Guide (GitHub Flavor)

Markdown is a lightweight markup language that allows you to write formatted text using plain text syntax.
It is widely used on platforms like Reddit for formatting comments or GitHub for creating documentation, README files, and other text-based content.

This guide will provide a comprehensive overview of the Markdown syntax with examples.


## Table of Contents

- [Headings](#headings)
- [Text Formatting](#text-formatting)
- [Lists](#lists)
- [Links](#links)
- [Images](#images)
- [Tables](#tables)
- [Code Blocks](#code-blocks)
- [Blockquotes](#blockquotes)
- [Horizontal Rules](#horizontal-rules)


## Headings

Headings in Markdown are created using hash symbols (#) followed by a space and the heading text. The number of hash symbols determines the heading level, where one hash symbol represents the highest level (H1) and six hash symbols represent the lowest level (H6).

Example:
```
# Heading 1
## Heading 2
### Heading 3
```


## Text Formatting

You can apply various formatting options to your text:

- **Bold**: Surround the text with double asterisks or double underscores (`**bold text**` or `__bold text__`).
- *Italic*: Surround the text with single asterisks or single underscores (`*italic text*` or `_italic text_`).
- ~~Strikethrough~~: Use two tilde symbols (`~~strikethrough text~~`).


## Lists

There are two types of lists in Markdown: ordered and unordered.

**Ordered List:**

1. Item 1
2. Item 2
3. Item 3

To create an ordered list, start each line with a number followed by a period.

**Unordered List:**

- Item 1
- Item 2
- Item 3

To create an unordered list, start each line with a hyphen (-), plus sign (+), or asterisk (*).


## Links

You can create links to other web pages or sections within the document.

[MarkdownEdit](https://github.com/tim-gromeyer/MarkdownEdit)

## Images

You can also include images in your Markdown files.

![Alt Text](image.jpg)


## Tables

Markdown allows you to create tables with columns and rows.

| Header 1 | Header 2 |
| -------- | -------- |
| Cell 1   | Cell 2   |
| Cell 3   | Cell 4   |

To create a table, use pipes (|) to separate columns and hyphens (-) to define the table header.


## Code Blocks

You can display code snippets or blocks using backticks.

**Inline Code:** Use single backticks (`code`).

**Code Block:**

```js
function greet() {
  console.log("Hello, World!");
}
```


## Blockquotes

Blockquotes are used to display quoted content.

> This is a blockquote.
> It can span multiple lines.

To create a blockquote, prefix each line with a greater-than symbol (>) and a space.


## Horizontal Rules

You can add horizontal rules to visually separate sections.

---
___

Use three hyphens (-), underscores (_), or asterisks (*) on a line by themselves to create a horizontal rule.

---

This guide covers the basic syntax of Markdown. It should give you a good starting point for creating nicely formatted documents on GitHub or other platforms that support Markdown.

For more advanced features, refer to the official Markdown documentation.
