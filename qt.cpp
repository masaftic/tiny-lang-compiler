#include <QApplication>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QTextEdit>
#include <iostream>
#include <QFileDialog>
#include <QTextStream>
#include <QMenuBar>
#include <QMessageBox>


class MenuBarExample : public QWidget {
public:
    MenuBarExample() {
        editor = new QTextEdit(this);
        editor->setPlaceholderText("Insert text here...");

        currentFileName = new QLabel(this);
        currentFileName->setText("Untitled");
        currentFileName->setStyleSheet("font-weight: bold; color: light-gray;");
        currentFileName->setAlignment(Qt::AlignCenter);

        QVBoxLayout *layout = new QVBoxLayout(this);

        layout->addWidget(currentFileName);
        layout->addWidget(editor);
        this->setLayout(layout);

        QMenuBar *menuBar = new QMenuBar(this);
        QMenu *fileMenu = menuBar->addMenu("File");
        QMenu *anotherMenu = menuBar->addMenu("another one");

        QAction *openAction = new QAction("Open", this);
        QAction *saveAction = new QAction("Save", this);
        QAction *saveAsAction = new QAction("Save As", this);
        QAction *exitAction = new QAction("Exit", this);

        fileMenu->addAction(openAction);
        fileMenu->addAction(saveAction);
        fileMenu->addAction(saveAsAction);
        fileMenu->addAction(exitAction);

        // Connect actions to functions
        connect(openAction, &QAction::triggered, this, &MenuBarExample::openFile);
        connect(saveAction, &QAction::triggered, this, &MenuBarExample::saveFile);
        connect(saveAsAction, &QAction::triggered, this, &MenuBarExample::saveFileAs);
        connect(exitAction, &QAction::triggered, this, &MenuBarExample::close);

        // Set the menu bar for the window
        layout->setMenuBar(menuBar);
    }

private:
    QTextEdit *editor;
    QLabel *currentFileName;
    QString curentFilePath;

    void openFile() {
        QString fileName = QFileDialog::getOpenFileName(this, "Open File");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                editor->setText(in.readAll());
                file.close();

                curentFilePath = fileName;
                currentFileName->setText(fileName.split("/").back());
            }
        }

    }

    void saveFile() {
        if (curentFilePath.isEmpty()) {
            QMessageBox::warning(this, "Error", "No file opened to save!");
            return;
        }

        QFile file(curentFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << editor->toPlainText();
            file.close();
        } else {
            QMessageBox::warning(this, "Error", "Could not write to file.");
        }
    }

    void saveFileAs() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save File");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream out(&file);
                out << editor->toPlainText();
                file.close();
            }
        }
    }
};


class CompilerExample : public QWidget {
public:
    CompilerExample() {
        QTextEdit *textEdit = new QTextEdit(this);

        QTextEdit *outputArea = new QTextEdit(this);
        outputArea->setReadOnly(true);

        QPushButton *scanButton = new QPushButton("Scan");
        QPushButton *runButton = new QPushButton("Run");
        QPushButton *parseButton = new QPushButton("Parse");
       
        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(scanButton);
        buttonLayout->addWidget(runButton);
        buttonLayout->addWidget(parseButton);

        QVBoxLayout *rightLayout = new QVBoxLayout();
        rightLayout->addWidget(outputArea);
        rightLayout->addLayout(buttonLayout);

        // Main layout: left (text editor) and right (output + buttons)
        QHBoxLayout *mainLayout = new QHBoxLayout();
        mainLayout->addWidget(textEdit, 2);     // stretch factor 2
        mainLayout->addLayout(rightLayout, 1);  // stretch factor 1
        setLayout(mainLayout);
    }
};

// what, event, to what, action
// what, event, action

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CompilerExample example;

    example.setWindowTitle("Menu Bar Example");
    example.resize(600, 400);
    example.show();

    return app.exec();
}
