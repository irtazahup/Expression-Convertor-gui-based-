#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QString>
#include <string>
#include <stack>
#include <cctype>
#include <stdexcept>

using namespace std;

// TreeNode and Stack classes remain the same
struct TreeNode {
    string data;
    TreeNode* left;
    TreeNode* right;

    TreeNode(string value) {
        data = value;
        left = nullptr;
        right = nullptr;
    }
};

class Stack {
private:
    struct Node {
        TreeNode* treeNode;
        Node* next;

        Node(TreeNode* tn) {
            treeNode = tn;
            next = nullptr;
        }
    };
    Node* topNode;

public:
    Stack() {
        topNode = nullptr;
    }

    ~Stack() {
        while (!isEmpty()) {
            pop();
        }
    }

    void push(TreeNode* treeNode) {
        Node* newNode = new Node(treeNode);
        newNode->next = topNode;
        topNode = newNode;
    }

    void pop() {
        if (topNode != nullptr) {
            Node* temp = topNode;
            topNode = topNode->next;
            delete temp;
        }
    }

    TreeNode* top() {
        if (topNode != nullptr) {
            return topNode->treeNode;
        }
        return nullptr;
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }
};

class PostfixToInfixCalculator {
public:
    string postfixToInfix(const string& postfix) {
        Stack s;
        for (char c : postfix) {
            if (isOperator(c)) {
                if (s.isEmpty()) throw invalid_argument("Invalid postfix expression");
                TreeNode* operand2 = s.top();
                s.pop();
                if (s.isEmpty()) throw invalid_argument("Invalid postfix expression");
                TreeNode* operand1 = s.top();
                s.pop();
                TreeNode* newNode = new TreeNode("(" + operand1->data + " " + c + " " + operand2->data + ")");
                newNode->left = operand1;
                newNode->right = operand2;
                s.push(newNode);
            } else if (isspace(c)) {
                continue;
            } else if (isalnum(c)) {
                s.push(new TreeNode(string(1, c)));
            } else {
                throw invalid_argument("Invalid character in postfix expression");
            }
        }
        if (s.isEmpty() || s.top()->left == nullptr || s.top()->right == nullptr)
            throw invalid_argument("Invalid postfix expression");
        return s.top()->data;
    }

private:
    bool isOperator(char c) {
        return (c == '+' || c == '-' || c == '*' || c == '/');
    }
};

class InfixToPostfixCalculator {
public:
    string infixToPostfix(const string& infix) {
        stack<char> s;
        string postfix;
        for (char c : infix) {
            if (isalnum(c)) {
                postfix += c;
            } else if (c == '(') {
                s.push(c);
            } else if (c == ')') {
                while (!s.empty() && s.top() != '(') {
                    postfix += s.top();
                    s.pop();
                }
                if (!s.empty()) s.pop();
            } else if (isOperator(c)) {
                while (!s.empty() && precedence(s.top()) >= precedence(c)) {
                    postfix += s.top();
                    s.pop();
                }
                s.push(c);
            } else if (isspace(c)) {
                continue;
            } else {
                throw invalid_argument("Invalid character in infix expression");
            }
        }
        while (!s.empty()) {
            postfix += s.top();
            s.pop();
        }
        return postfix;
    }

private:
    bool isOperator(char c) {
        return (c == '+' || c == '-' || c == '*' || c == '/');
    }

    int precedence(char c) {
        if (c == '+' || c == '-')
            return 1;
        if (c == '*' || c == '/')
            return 2;
        return 0;
    }
};

// Additional calculators for other conversions can be added similarly

class CalculatorWidget : public QWidget {
    Q_OBJECT

public:
    CalculatorWidget(QWidget *parent = nullptr)
        : QWidget(parent) {
        QVBoxLayout *layout = new QVBoxLayout;

        inputLineEdit = new QLineEdit;
        outputLabel = new QLabel;
        convertButton = new QPushButton("Convert");

        inputTypeComboBox = new QComboBox;
        inputTypeComboBox->addItem("Postfix");
        inputTypeComboBox->addItem("Infix");

        conversionTypeComboBox = new QComboBox;
        conversionTypeComboBox->addItem("Postfix to Infix");
        conversionTypeComboBox->addItem("Infix to Postfix");

        layout->addWidget(new QLabel("Enter expression:"));
        layout->addWidget(inputLineEdit);
        layout->addWidget(new QLabel("Input type:"));
        layout->addWidget(inputTypeComboBox);
        layout->addWidget(new QLabel("Conversion type:"));
        layout->addWidget(conversionTypeComboBox);
        layout->addWidget(convertButton);
        layout->addWidget(new QLabel("Converted expression:"));
        layout->addWidget(outputLabel);

        setLayout(layout);

        connect(convertButton, &QPushButton::clicked, this, &CalculatorWidget::convertExpression);
    }

public slots:
    void convertExpression() {
        try {
            QString input = inputLineEdit->text();
            QString inputType = inputTypeComboBox->currentText();
            QString conversionType = conversionTypeComboBox->currentText();

            string result;

            if (conversionType == "Postfix to Infix") {
                PostfixToInfixCalculator calculator;
                result = calculator.postfixToInfix(input.toStdString());
            } else if (conversionType == "Infix to Postfix") {
                InfixToPostfixCalculator calculator;
                result = calculator.infixToPostfix(input.toStdString());
            }

            outputLabel->setText(QString::fromStdString(result));
        } catch (const std::exception &e) {
            outputLabel->setText("Error: " + QString::fromStdString(e.what()));
        }
    }

private:
    QLineEdit *inputLineEdit;
    QLabel *outputLabel;
    QPushButton *convertButton;
    QComboBox *inputTypeComboBox;
    QComboBox *conversionTypeComboBox;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    CalculatorWidget calculator;
    calculator.setWindowTitle("Expression Converter");
    calculator.show();

    return app.exec();
}

#include "main.moc"
