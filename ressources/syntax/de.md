# Markdown-Syntax

Der Kern dessen, was MD4C verstehen kann, ist in der [CommonMark-Spezifikation](http://spec.commonmark.org/0.27/) definiert.
Daher ist das Folgende nur eine verkürzte Zusammenfassung.

* Klartext (zwei Absätze):
```
Dies ist der erste Absatz.

Und dies ist der zweite Absatz.
```

* Kursivschrift:

```
Text, der in *einzelne Sternchen* oder _einzelne Unterstriche_ eingeschlossen ist, wird
als Hervorhebung erkannt und in der Regel als kursiver Text wiedergegeben.
```

* Fettdruck:

```
In ähnlicher Weise wird Text, der von **doppelten Sternchen** oder __doppelten Unterstrichen__
eingeschlossener Text wird als starke Betonung erkannt und normalerweise als fetter
Text.
```

* Fett kursiv:

```
*Kursiv* und **fetter Text** können zu ***fett kursiv*** kombiniert werden, sogar 
__auf recht __komplizierte_ Weise__.
```

* Überschriften:

```markdown
# Überschrift Ebene 1
## Überschrift Ebene 2
## Überschrift Ebene 3
#### Überschrift Ebene 4
##### Kopfzeile Ebene 5
###### Überschriftsebene 6

Die ersten beiden Überschriftenebenen können alternativ auch mit einem Unterstrich geschrieben werden:

Überschriftsebene 1
==============

Kopfzeile Ebene 2
--------------
```

* Link: 

```
Inline-Link: [Linktext](http://www.example.com)

Oder Link unter Verwendung einer Referenzdefinition:

[Verweislink][1]

[1]: http://www.example.com
```

* Bild: 

```
Inline-Bild: ![image alt text](http://www.example.com/image.png)

Oder Bild unter Verwendung einer Referenzdefinition:

![image alt text][2]

[2]: http://www.example.com/image.png
```

* Ungeordnete Listen:

```
* Listenelement 1
* Listenpunkt 2
* Listenpunkt 3

oder

- Punkt 1 der Liste
- Punkt 2 der Liste
- Listenpunkt 3
```

* Geordnete Listen:

```
1. Listenpunkt 1
2. Listenpunkt 2
3. Punkt 3 auflisten

oder

1) Punkt 1 auflisten
2) Punkt 2 auflisten
3) Punkt 3 auflisten
```

* Verschachtelte Listen:

```
* Foo
* Unterpunkt von Foo
* Bar
* Bar's Unterpunkt 1
* Bar's subitem 2
* Bar's subitem 3
* Baz
* Baz' Unterpunkt 1
* Baz's Unterpunkt 2
```

* Blockquote:

```
> Ein Teil des Dokuments, dessen Zeilen mit ">" eingeleitet werden, wird als "Blockquote" behandelt.
> Blockquote behandelt.
>
> > Er kann auch verschachtelte Anführungszeichen oder Listen enthalten:
> 
> * Listenelement 1
> * Aufzählungspunkt 2
```

* Inline-Code (auch bekannt als Code-Spanne):

```
Shell-Befehl `ls` ausführen, um den Inhalt des aktuellen Verzeichnisses zu ermitteln.
```

* Code-Block:

```
Satz von Linien, die in einer Linie eingeschlossen sind, die aus drei (oder mehr) aufeinanderfolgenden Linien besteht
Backticks (```) oder Tilden (~~~) werden als Codeblock erkannt.

Alternativ kann der Code auch nur um vier oder mehr Leerzeichen eingerückt werden:

  $ ./configure
  $ make
  $ make install

Nach dem Öffnungscodezaun kann ein [Info-String] bereitgestellt werden.
Obwohl diese Spezifikation keine besondere Behandlung von vorschreibt
Der Info-String, das erste Wort wird normalerweise zur Angabe verwendet
die Sprache des Codeblocks. Bei der HTML-Ausgabe ist die Sprache
normalerweise durch Hinzufügen einer Klasse zum Element "Code" angegeben, bestehend aus
von `language-`, gefolgt vom Namen der Sprache.

Beispiel:

~~~ruby
def foo(x)
  return 3
end
~~~

wird zu:

<pre><code class="language-ruby">def foo(x)
  return 3
end
</code></pre>