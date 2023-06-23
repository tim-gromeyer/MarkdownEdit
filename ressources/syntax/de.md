# Markdown Syntax Guide (GitHub Flavor)

Markdown ist eine leichtgewichtige Auszeichnungssprache, die es Ihnen ermöglicht, formatierten Text mithilfe einer einfachen Textsyntax zu schreiben.
Sie wird weit verbreitet auf Plattformen wie Reddit zur Formatierung von Kommentaren oder auf GitHub zur Erstellung von Dokumentationen, README-Dateien und anderen textbasierten Inhalten verwendet.

Diese Anleitung bietet eine umfassende Übersicht über die Markdown-Syntax mit Beispielen.


## Inhaltsverzeichnis

- [Überschriften](#überschriften)
- [Textformatierung](#textformatierung)
- [Listen](#listen)
- [Links](#links)
- [Bilder](#bilder)
- [Tabellen](#tabellen)
- [Codeblöcke](#codeblöcke)
- [Zitate](#zitate)
- [Horizontale Linien](#horizontale-linien)


## Überschriften

Überschriften in Markdown werden mit Hashtags (#) erstellt, gefolgt von einem Leerzeichen und dem Überschriftentext. Die Anzahl der Hashtags bestimmt das Überschriftenlevel, wobei ein Hashtag das höchste Level (H1) darstellt und sechs Hashtags das niedrigste Level (H6).

Beispiel:
```
# Überschrift 1
## Überschrift 2
### Überschrift 3
```


## Textformatierung

Sie können verschiedene Formatierungsoptionen auf Ihren Text anwenden:

- **Fett**: Umgeben Sie den Text mit doppelten Sternchen oder doppelten Unterstrichen (`**fetter Text**` oder `__fetter Text__`).
- *Kursiv*: Umgeben Sie den Text mit einfachen Sternchen oder einfachen Unterstrichen (`*kursiver Text*` oder `_kursiver Text_`).
- ~~Durchgestrichen~~: Verwenden Sie zwei Tilde-Symbole (`~~durchgestrichener Text~~`).


## Listen

Es gibt zwei Arten von Listen in Markdown: geordnete und ungeordnete Listen.

**Geordnete Liste:**

1. Eintrag 1
2. Eintrag 2
3. Eintrag 3

Um eine geordnete Liste zu erstellen, beginnen Sie jede Zeile mit einer Zahl, gefolgt von einem Punkt.

**Ungeordnete Liste:**

- Eintrag 1
- Eintrag 2
- Eintrag 3

Um eine ungeordnete Liste zu erstellen, beginnen Sie jede Zeile mit einem Bindestrich (-), Pluszeichen (+) oder einem Sternchen (*).


## Links

Sie können Links zu anderen Webseiten oder Abschnitten im Dokument erstellen.

[MarkdownEdit](https://github.com/tim-gromeyer/MarkdownEdit)

## Bilder

Sie können auch Bilder in Ihre Markdown-Dateien einfügen.

![Alternativer Text](image.jpg)


## Tabellen

Markdown ermöglicht es Ihnen, Tabellen mit Spalten und Zeilen zu erstellen.

| Überschrift 1 | Überschrift 2 |
| ------------- | ------------- |
| Zelle 1       | Zelle 2       |
| Zelle 3       | Zelle 4       |

Um eine Tabelle zu erstellen, verwenden Sie Pipe-Zeichen (|), um Spalten zu trennen, und Bindestriche (-), um die Tabellenüberschrift zu definieren.


## Codeblöcke

Sie können Codeausschnitte oder -blöcke mithilfe von Backticks anzeigen.

**Inline-Code:** Verwenden Sie einzelne Backticks (`code`).

**Codeblock:**

```js
function grüßen() {
  console.log("Hallo, Welt!");
}
```


## Zitate

Zitate werden verwendet, um zitierten Inhalt anzuzeigen.

> Dies ist ein Zitat.
> Es kann sich über mehrere Zeilen erstrecken.

Um ein Zitat zu erstellen, fügen Sie jedem Zeilenvorlauf ein größer-als-Symbol (>) und ein Leerzeichen hinzu.


## Horizontale Linien

Sie können horizontale Linien hinzufügen, um Abschnitte optisch voneinander zu trennen.

---
___

Verwenden Sie drei Bindestriche (-), Unterstriche (_) oder Sternchen (*) in einer eigenen Zeile, um eine horizontale Linie zu erstellen.

---

Diese Anleitung behandelt die grundlegende Syntax von Markdown. Sie sollte Ihnen einen guten Ausgangspunkt für die Erstellung schön formatierter Dokumente auf GitHub oder anderen Plattformen, die Markdown unterstützen, bieten.

Für fortgeschrittene Funktionen lesen Sie die offizielle Markdown-Dokumentation nach.~~~
