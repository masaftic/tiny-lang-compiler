#include <QApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QFont>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>
#include <iostream>
#include <sstream>

#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "token.h"
#include <string>

class CodeEditor : public QWidget {
public:
    CodeEditor()
    {
        editor = new QTextEdit(this);
        editor->setPlaceholderText("Insert text here...");
        // Use a monospace font for the editor
        QFont editorFont("Monospace", 15);
        editorFont.setStyleHint(QFont::TypeWriter);
        editor->setFont(editorFont);
        editor->setStyleSheet("font-family: Jetbrains mono;");

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
        outputArea->setStyleSheet("font-family: Jetbrains mono;");

        QLabel* outputAreaLabel = new QLabel("Output");
        outputAreaLabel->setStyleSheet("font-weight: bold; color: gray;");
        outputAreaLabel->setAlignment(Qt::AlignCenter);

        QPushButton* scanButton = new QPushButton("Scan");
        QPushButton* runButton = new QPushButton("Run");
        QPushButton* parseButton = new QPushButton("Parse");

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        buttonLayout->addWidget(scanButton);
        buttonLayout->addWidget(runButton);
        buttonLayout->addWidget(parseButton);

        // --- Layout Setup using QSplitter ---

        // Left side widget (editor + label)
        QWidget* leftWidget = new QWidget;
        QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget); // Set layout on the widget
        leftLayout->addWidget(fileNameLabel);
        leftLayout->addWidget(editor);
        leftLayout->setContentsMargins(0, 0, 0, 0); // Remove margins if desired

        // Right side widget (output + label + buttons)
        QWidget* rightWidget = new QWidget;
        QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget); // Set layout on the widget
        rightLayout->addWidget(outputAreaLabel);
        rightLayout->addWidget(outputArea);
        rightLayout->addLayout(buttonLayout);
        rightLayout->setContentsMargins(0, 0, 0, 0); // Remove margins if desired

        // Create the splitter
        QSplitter* splitter = new QSplitter(Qt::Horizontal);
        splitter->addWidget(leftWidget);
        splitter->addWidget(rightWidget);
        splitter->setStretchFactor(0, 2); // Initial stretch factor for left
        splitter->setStretchFactor(1, 1); // Initial stretch factor for right
        splitter->setHandleWidth(5); // Set handle width for better visibility

        // Menu Bar
        QMenuBar* menuBar = new QMenuBar(); // Create menu bar without parent first
        QMenu* fileMenu = menuBar->addMenu("File");

        QAction* openAction = new QAction("Open", this);
        QAction* saveAction = new QAction("Save", this);
        QAction* saveAsAction = new QAction("Save As", this);
        QAction* exitAction = new QAction("Exit", this);

        fileMenu->addAction(openAction);
        fileMenu->addAction(saveAction);
        fileMenu->addAction(saveAsAction);
        fileMenu->addAction(exitAction);

        // Main layout for the CodeEditor widget
        QVBoxLayout* mainVLayout = new QVBoxLayout(this); // Use QVBoxLayout to hold menu and splitter
        mainVLayout->setMenuBar(menuBar); // Add menu bar to the layout
        mainVLayout->addWidget(splitter); // Add splitter below the menu bar
        mainVLayout->setContentsMargins(5, 5, 5, 5);

        // Status bar to display cursor position
        statusBar = new QLabel(this);
        statusBar->setStyleSheet("font-size: 18px; color: gray; padding: 2px;");
        statusBar->setAlignment(Qt::AlignLeft);
        statusBar->setFixedHeight(25); // Set a fixed height for the status bar
        updateCursorPosition(); // Initialize cursor position display

        // Add status bar to the main layout
        mainVLayout->addWidget(statusBar);

        // Connect actions and buttons
        connect(openAction, &QAction::triggered, this, &CodeEditor::openFile);
        connect(saveAction, &QAction::triggered, this, &CodeEditor::saveFile);
        connect(saveAsAction, &QAction::triggered, this, &CodeEditor::saveFileAs);
        connect(exitAction, &QAction::triggered, this, &QWidget::close);

        // Connect scan button to the new slot
        connect(scanButton, &QPushButton::clicked, this, &CodeEditor::scanSource);
        connect(parseButton, &QPushButton::clicked, this, &CodeEditor::parseSource);
        connect(runButton, &QPushButton::clicked, this, &CodeEditor::runSource);
        // Connect cursor position change signal
        connect(editor, &QTextEdit::cursorPositionChanged, this, &CodeEditor::updateCursorPosition);
    }

private slots:
    void openFile()
    {
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

    void saveFile()
    {
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

    void saveFileAs()
    {
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

    void scanSource()
    {
        outputArea->clear();
        QString sourceCode = editor->toPlainText();
        std::string sourceStdString = sourceCode.toStdString();

        if (sourceStdString.empty()) {
            outputArea->setText("Editor is empty.");
            return;
        }

        try {
            Lexer lexer(sourceStdString);
            QTextCursor cursor = editor->textCursor();
            cursor.movePosition(QTextCursor::Start); // Start from the beginning of the text

            QStringList lines = sourceCode.split('\n'); // Split the source code into lines
            string outputTokens;

            while (!lexer.isAtEnd()) {
                Token token = lexer.nextToken();
                outputTokens += token.toString() + "\n";

                // Calculate the start index based on line and column
                int startIndex = 0;
                for (int i = 0; i < token.start_line - 1; ++i) {
                    startIndex += lines[i].length() + 1; // Add 1 for the newline character
                }
                startIndex += token.start_column - 1;

                // Set the format based on token type
                QTextCharFormat format;
                switch (token.type) {
                case Token::Type::READ:
                case Token::Type::REPEAT:
                case Token::Type::UNTIL:
                case Token::Type::WRITE:
                case Token::Type::IF:
                case Token::Type::ELSE:
                case Token::Type::END:
                case Token::Type::THEN:
                    format.setForeground(Qt::blue);
                    break;
                case Token::Type::IDENTIFIER:
                    format.setForeground(Qt::black);
                    break;
                case Token::Type::NUMBER:
                    format.setForeground(Qt::darkGreen);
                    break;
                case Token::Type::LITERAL:
                    format.setForeground(Qt::darkRed);
                    break;
                case Token::Type::MULTIPLY:
                case Token::Type::PLUS:
                case Token::Type::MINUS:
                case Token::Type::DIVIDE:
                case Token::Type::LESS_THAN:
                case Token::Type::GREATER_THAN:
                case Token::Type::LESS_EQUAL:
                case Token::Type::GREATER_EQUAL:
                case Token::Type::NOT_EQUAL:
                case Token::Type::EQUAL:
                case Token::Type::ASSIGNMENT:
                    format.setForeground(Qt::darkMagenta);
                    break;
                default:
                    format.setForeground(Qt::black);
                    break;
                }

                // Apply the format to the token's range
                cursor.setPosition(startIndex);
                cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, token.lexeme.length() + 2);
                cursor.setCharFormat(format);
            }

            outputArea->append("--- LEXER OUTPUT ---");
            outputArea->append(QString::fromStdString(outputTokens));
        } catch (const std::exception& e) {
            outputArea->append("\n--- LEXER ERROR ---");
            outputArea->append(QString("Error: %1").arg(e.what()));
        } catch (...) {
            outputArea->append("\n--- UNKNOWN LEXER ERROR ---");
        }
    }

    void parseSource()
    {
        outputArea->clear();
        QString sourceCode = editor->toPlainText();
        std::string sourceStdString = sourceCode.toStdString();

        if (sourceStdString.empty()) {
            outputArea->setText("Editor is empty.");
            return;
        }

        try {
            Lexer lexer(sourceStdString);
            Parser parser(lexer);
            auto statements = parser.parse();
            if (parser.isError()) {
                outputArea->append("--- PARSER ERROR ---\n");
                for (const auto& error : parser.getErrors()) {
                    outputArea->append(QString::fromStdString(error));
                }
                return;
            }
            string output;
            for (const auto& stmt : statements) {
                output += stmt->toString(0);
            }
            outputArea->append("--- PARSER OUTPUT ---\n");
            outputArea->append(QString::fromStdString(output));
        } catch (const std::exception& e) {
            outputArea->append("--- PARSER ERROR ---\n");
            outputArea->append(QString("Error: %1").arg(e.what()));
        } catch (...) {
            outputArea->append("--- UNKNOWN PARSER ERROR ---\n");
        }
    }

    void runSource()
    {
        outputArea->clear();
        QString sourceCode = editor->toPlainText();
        std::string sourceStdString = sourceCode.toStdString();

        if (sourceStdString.empty()) {
            outputArea->setText("Editor is empty.");
            return;
        }

        try {
            Lexer lexer(sourceStdString);
            Parser parser(lexer);
            auto statements = parser.parse();

            // Custom input and output streams
            std::istringstream inputStream;
            std::ostringstream outputStream;

            // Block execution and prompt user for input
            QString userInput = QInputDialog::getMultiLineText(this, "Input Required",
                "Provide input for the program:");
            inputStream.str(userInput.toStdString());

            // Pass custom streams to the interpreter
            Interpreter interpreter(inputStream, outputStream);
            interpreter.interpret(statements);

            // Display the output in the outputArea
            outputArea->append(QString::fromStdString(outputStream.str()));
            outputArea->append("\nInterpreter finished.");
        } catch (const std::exception& e) {
            outputArea->append("\n--- INTERPRETER ERROR ---");
            outputArea->append(QString("Error: %1").arg(e.what()));
        } catch (...) {
            outputArea->append("\n--- UNKNOWN INTERPRETER ERROR ---");
        }
    }

    void updateCursorPosition()
    {
        QTextCursor cursor = editor->textCursor();
        int line = cursor.blockNumber() + 1; // Line numbers are 0-based
        int column = cursor.columnNumber() + 1; // Column numbers are 0-based
        statusBar->setText(QString("Line: %1, Column: %2").arg(line).arg(column));
    }

private:
    QTextEdit* editor;
    QLabel* fileNameLabel;
    QString curentFilePath;
    QTextEdit* outputArea;
    QLabel* statusBar;
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    CodeEditor codeEditor;

    codeEditor.setWindowTitle("Compiler IDE");
    codeEditor.resize(800, 600);
    codeEditor.show();

    return app.exec();
}
