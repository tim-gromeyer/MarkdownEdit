# Markdown syntax

Core of what MD4C can undertand is defined by [CommonMark specification](http://spec.commonmark.org/0.27/).
Therefore the below is just a condensed summary.

* Plain text (two paragraphs):
```
This is first paragraph.

And this is second paragraph.
```

* Italic:

```
Text enclosed in *single asterisks* or _single underscores_ is recognized
as an emphasis, and it usually gets rendered as an italic text.
```

* Bold:

```
Similarly, text enclosed in **double asterisks** or __double underscores__
is recognized as a strong emphasis, and it usually gets rendered as a bold
text.
```

* Bold italic:

```
*Italic* and **bold text** can be combined into ***bold italic***, even 
__in quite a _complicated_ ways__.
```

* Headings:

```markdown
# Header level 1
## Header level 2
### Header level 3
#### Header level 4
##### Header level 5
###### Header level 6

First two header level may alternatively be written using an underline:

Header level 1
==============

Header level 2
--------------
```

* Link: 

```
Inline link: [link text](http://www.example.com)

Or link using a reference definition:

[Reference link][1]

[1]: http://www.example.com
```

* Image: 

```
Inline image: ![image alt text](http://www.example.com/image.png)

Or image using a reference definition:

![image alt text][2]

[2]: http://www.example.com/image.png
```

* Unordered lists:

```
* List item 1
* List item 2
* List item 3

or

- List item 1
- List item 2
- List item 3
```

* Ordered lists:

```
1. List item 1
2. List item 2
3. List item 3

or

1) List item 1
2) List item 2
3) List item 3
```

* Nested lists:

```
* Foo
* Foo's subitem
* Bar
* Bar's subitem 1
* Bar's subitem 2
* Bar's subitem 3
* Baz
* Baz's subitem 1
* Baz's subitem 2
```

* Blockquote:

```
> Part of documements with lines prepended with `>` is treated as a
> blockquote.
>
> > It may contain even nested block quote or lists:
> 
> * List item 1
> * List item 2
```

* Inline code (a.k.a code span):

```
Run shell command `ls` to get contents of the current directory.
```

* Code block:

```
Set of lines enclosed within a lines formed by three (or more) consecutive
backticks (```) or tildes (~~~) is recognized as a code block.

Alternatively, the code may just be indented by four or more spaces:

  $ ./configure
  $ make
  $ make install

An [info string] can be provided after the opening code fence.
Although this spec doesn't mandate any particular treatment of
the info string, the first word is typically used to specify
the language of the code block. In HTML output, the language is
normally indicated by adding a class to the `code` element consisting
of `language-` followed by the language name.

example:

~~~ruby
def foo(x)
return 3
end
~~~

will turn into:

<pre><code class="language-ruby">def foo(x)
return 3
end
</code></pre>
```