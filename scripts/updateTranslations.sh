# change directory to the scripts parent directory
# cd $( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )/..
cd $(dirname "$0")
cd ..

# Update translations using lupdate
lupdate src/*.cpp ui/mainwindow.ui -ts translations/MarkdownEdit_de_DE.ts -noobsolete
