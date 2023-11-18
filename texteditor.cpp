#include "texteditor.h"
#include "ui_texteditor.h"

TextEditor::TextEditor(const QString &fileName, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TextEditor)
{
    ui->setupUi(this);
    setWindowTitle("TextEditor[*]");

    QFont Font = QSettings(this).value("viewFont").value<QFont>();
    ui->textEdit->setFont(Font);

    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->actionCopy->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionRedo->setEnabled(false);

    QObject::connect(ui->actionClose, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->actionExit, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    QObject::connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(documentModified()));

    QObject::connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    QObject::connect(ui->actionCut, SIGNAL(triggered()), ui->textEdit, SLOT(cut()));
    QObject::connect(ui->actionCopy, SIGNAL(triggered()), ui->textEdit, SLOT(copy()));
    QObject::connect(ui->actionPaste, SIGNAL(triggered()), ui->textEdit, SLOT(paste()));
    QObject::connect(ui->actionUndo, SIGNAL(triggered()), ui->textEdit, SLOT(undo()));
    QObject::connect(ui->actionRedo, SIGNAL(triggered()), ui->textEdit, SLOT(redo()));

    QObject::connect(ui->textEdit, SIGNAL(copyAvailable(bool)), ui->actionCopy, SLOT(setEnabled(bool)));
    QObject::connect(ui->textEdit, SIGNAL(copyAvailable(bool)), ui->actionCut, SLOT(setEnabled(bool)));
    QObject::connect(ui->textEdit, SIGNAL(undoAvailable(bool)), ui->actionUndo, SLOT(setEnabled(bool)));
    QObject::connect(ui->textEdit, SIGNAL(redoAvailable(bool)), ui->actionRedo, SLOT(setEnabled(bool)));

    QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
    QObject::connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveFileAs()));

    loadFile(fileName);
}

TextEditor::~TextEditor()
{
    delete ui;
}

void TextEditor::on_actionNew_triggered()
{
    TextEditor* newWindow = new TextEditor();
    newWindow->show();
}

void TextEditor::documentModified()
{
    setWindowModified(true);
}

void TextEditor::closeEvent(QCloseEvent *e)
{
    if (isWindowModified())
    {
        switch (QMessageBox::warning(this, "Документ изменён",
                "Этот документ был изменён, желаете сохранить его? Все несохранённые изменения будут утеряны.",
                QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
                QMessageBox::Cancel))
        {
        case QMessageBox::Yes:
            saveFile();
            e->ignore();
            break;
        case QMessageBox::No:
            e->accept();
            break;
        case QMessageBox::Cancel:
            e->ignore();
            break;
        default:
            break;
        }
    }
    else
    {
        e->accept();
    }
}

void TextEditor::on_actionSelectFont_triggered()
{
    bool ok;
    QFont newFont = QFontDialog::getFont(&ok);

    if (ok)
    {
        ui->textEdit->setFont(newFont);
        QSettings* settings = new QSettings(this);
        settings->setValue("viewFont", newFont);
    }
}

void TextEditor::on_actionAbout_triggered()
{
    QMessageBox::about(this, "Справочная информация", "Данная программа была написана для лабораторной работы №9 для предмета ООТПиСП.");
}

void TextEditor::loadFile(const QString &fileName)
{
    if (fileName == "")
    {
        setFileName(QString());
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл.");
        setFileName(QString());
        return;
    }

    QTextStream stream(&file);
    ui->textEdit->setText(stream.readAll());
    file.close();
    setFileName(fileName);
    setWindowModified(false);
}

void TextEditor::setFileName(const QString &fileName)
{
    this->m_fileName = fileName;
    setWindowTitle(QString("%1[*] - %2")
                       .arg(m_fileName.isNull() ? "untitled" : QFileInfo(m_fileName).fileName())
                       .arg(QApplication::applicationName())
    );
}

void TextEditor::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Выберите файл", QDir::currentPath(), "Текстовый файл (*.txt)");
    if (fileName.isNull())
    {
        return;
    }

    if (this->m_fileName.isNull() && !isWindowModified())
    {
        loadFile(fileName);
    }
    else
    {
        TextEditor* newWindow = new TextEditor(fileName);
        newWindow->show();
    }
}

bool TextEditor::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить файл",
                                                    m_fileName.isNull() ? QDir::currentPath() : m_fileName, "Текстовый файл (*.txt)");
    if (fileName.isNull())
    {
        return false;
    }

    setFileName(fileName);
    return saveFile();
}

bool TextEditor::saveFile()
{
    if (m_fileName.isNull())
    {
        return saveFileAs();
    }
    else
    {
        QFile file(m_fileName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл.");
            setFileName(QString());
            return false;
        }

        QTextStream stream(&file);
        stream << ui->textEdit->toPlainText();
        file.close();
        setFileName(m_fileName);
        setWindowModified(false);
        return true;
    }
}
