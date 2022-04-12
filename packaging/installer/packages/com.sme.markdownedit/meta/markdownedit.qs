function Component()
{

}

Component.prototype.createOperations = function()
{
    // call default implementation to actually install README.txt!
    component.createOperations();

    if (installer.value("os") === "win") {
        component.addOperation("CreateShortcut", "@TargetDir@/MarkdownEdit.exe", "@StartMenuDir@/MarkdownEdit.lnk",
                               "workingDirectory=@TargetDir@", "iconPath=%SystemRoot%/system32/SHELL32.dll",
                               "iconId=2", "description=Open SME Manager");
    }
    if (installer.value("os") === "x11") {
        component.addOperation("CreateDesktopEntry", "MarkdownEdit.desktop", "Name=MarkdownEdit\nGenericName=MarkdownEdit\nComment=Start MarkdownEdit\nComment[de]=Starte MarkdownEdit\nExec=@TargetDir@/MarkdownEdit %f\nIcon=text-markdown\nTerminal=false\nType=Application\nCategories=Utility\nMimeType=text/markdown");
    }
}

