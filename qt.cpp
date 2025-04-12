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
#include <QHBoxLayout>
#include <QFileInfo> // Include QFileInfo for file name extraction
#include <QSplitter> // Include QSplitter
#include <QFont>     // Include QFont

// Include Lexer and Token headers
#include "lexer.h"
#include "token.h"
#include <string> // Include for std::string conversion


class CodeEditor : public QWidget {
public:
    CodeEditor() {
        editor = new QTextEdit(this);
        editor->setPlaceholderText("Insert text here...");
        // Use a monospace font for the editor
        QFont editorFont("Monospace", 15);
        editorFont.setStyleHint(QFont::TypeWriter);
        editor->setFont(editorFont);


        fileNameLabel = new QLabel(this);
        fileNameLabel->setText("Untitled");
        fileNameLabel->setStyleSheet("font-weight: bold; color: gray;");
        fileNameLabel->setAlignment(Qt::AlignCenter);

        // Make outputArea a member variable
        outputArea = new QTextEdit(this);
        outputArea->setReadOnly(true);
        // Use a monospace font for the output area as well
        QFont outputFont("Monospace");
        outputFont.setStyleHint(QFont::TypeWriter);
        outputArea->setFont(outputFont);

        QLabel *outputAreaLabel = new QLabel("Output");
        outputAreaLabel->setStyleSheet("font-weight: bold; color: gray;");
        outputAreaLabel->setAlignment(Qt::AlignCenter);


        QPushButton *scanButton = new QPushButton("Scan");
        QPushButton *runButton = new QPushButton("Run");
        QPushButton *parseButton = new QPushButton("Parse");

        QHBoxLayout *buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(scanButton);
        buttonLayout->addWidget(runButton);
        buttonLayout->addWidget(parseButton);


        // --- Layout Setup using QSplitter ---

        // Left side widget (editor + label)
        QWidget *leftWidget = new QWidget;
        QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget); // Set layout on the widget
        leftLayout->addWidget(fileNameLabel);
        leftLayout->addWidget(editor);
        leftLayout->setContentsMargins(0,0,0,0); // Remove margins if desired

        // Right side widget (output + label + buttons)
        QWidget *rightWidget = new QWidget;
        QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget); // Set layout on the widget
        rightLayout->addWidget(outputAreaLabel);
        rightLayout->addWidget(outputArea);
        rightLayout->addLayout(buttonLayout);
        rightLayout->setContentsMargins(0,0,0,0); // Remove margins if desired

        // Create the splitter
        QSplitter *splitter = new QSplitter(Qt::Horizontal);
        splitter->addWidget(leftWidget);
        splitter->addWidget(rightWidget);
        splitter->setStretchFactor(0, 2); // Initial stretch factor for left
        splitter->setStretchFactor(1, 1); // Initial stretch factor for right
        splitter->setHandleWidth(10); // Set handle width for better visibility

        // Menu Bar
        QMenuBar *menuBar = new QMenuBar(); // Create menu bar without parent first
        QMenu *fileMenu = menuBar->addMenu("File");

        QAction *openAction = new QAction("Open", this);
        QAction *saveAction = new QAction("Save", this);
        QAction *saveAsAction = new QAction("Save As", this);
        QAction *exitAction = new QAction("Exit", this);

        fileMenu->addAction(openAction);
        fileMenu->addAction(saveAction);
        fileMenu->addAction(saveAsAction);
        fileMenu->addAction(exitAction);

        // Main layout for the CodeEditor widget
        QVBoxLayout *mainVLayout = new QVBoxLayout(this); // Use QVBoxLayout to hold menu and splitter
        mainVLayout->setMenuBar(menuBar); // Add menu bar to the layout
        mainVLayout->addWidget(splitter); // Add splitter below the menu bar
        mainVLayout->setContentsMargins(5,0,5,5); // Adjust margins as needed (top is 0 due to menubar)


        // Connect actions and buttons
        connect(openAction, &QAction::triggered, this, &CodeEditor::openFile);
        connect(saveAction, &QAction::triggered, this, &CodeEditor::saveFile);
        connect(saveAsAction, &QAction::triggered, this, &CodeEditor::saveFileAs);
        connect(exitAction, &QAction::triggered, this, &QWidget::close);

        // Connect scan button to the new slot
        connect(scanButton, &QPushButton::clicked, this, &CodeEditor::scanSource);
        // TODO: Connect runButton and parseButton later
    }

private slots: // Make methods intended as slots private slots
    void openFile() {
        QString fileName = QFileDialog::getOpenFileName(this, "Open File");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream in(&file);
                editor->setText(in.readAll());
                file.close();

                curentFilePath = fileName;
                QFileInfo fileInfo(fileName);
                fileNameLabel->setText(fileInfo.fileName());
            } else {
                 QMessageBox::warning(this, "Error", "Could not open file for reading.");
            }
        }
    }

    void saveFile() {
        if (curentFilePath.isEmpty()) {
            saveFileAs();
            return;
        }

        QFile file(curentFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
            QTextStream out(&file);
            out << editor->toPlainText();
            file.close();
        } else {
            QMessageBox::warning(this, "Error", "Could not write to file: " + curentFilePath);
        }
    }

    void saveFileAs() {
        QString fileName = QFileDialog::getSaveFileName(this, "Save File As");
        if (!fileName.isEmpty()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)) {
                QTextStream out(&file);
                out << editor->toPlainText();
                file.close();
                curentFilePath = fileName;
                QFileInfo fileInfo(fileName);
                fileNameLabel->setText(fileInfo.fileName());
            } else {
                 QMessageBox::warning(this, "Error", "Could not save file to: " + fileName);
            }
        }
    }

    // New slot for scanning the source code
    void scanSource() {
        outputArea->clear(); // Clear previous output
        QString sourceCode = editor->toPlainText();
        std::string sourceStdString = sourceCode.toStdString(); // Convert to std::string

        if (sourceStdString.empty()) {
            outputArea->setText("Editor is empty.");
            return;
        }

        try {
            Lexer lexer(sourceStdString);
            Token token = lexer.nextToken();
            while (token.type != Token::Type::ENDOFFILE) {
                // Append token information to the output area
                outputArea->append(QString::fromStdString(token.toString()));
                token = lexer.nextToken();
            }
            // Append the EOF token as well
            outputArea->append(QString::fromStdString(token.toString()));
            outputArea->append("\nLexer finished.");
        } catch (const std::exception& e) {
            outputArea->append("\n--- LEXER ERROR ---");
            outputArea->append(QString("Error: %1").arg(e.what()));
        } catch (...) {
            outputArea->append("\n--- UNKNOWN LEXER ERROR ---");
        }
    }


private:
    QTextEdit *editor;
    QLabel *fileNameLabel;
    QString curentFilePath;
    QTextEdit *outputArea; // Moved outputArea here
};


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CodeEditor codeEditor;

    codeEditor.setWindowTitle("Compiler IDE");
    codeEditor.resize(800, 600);
    codeEditor.show();

    return app.exec();
}
