#include "texteditor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("TextEditor");
    a.setApplicationVersion("0.1");

    a.setOrganizationName("ExampleSoft");
    a.setOrganizationDomain("example.com");

    a.setWindowIcon(QIcon(":/icons/images/new.png"));

    TextEditor *w = new TextEditor;
    w->show();
    return a.exec();
}
