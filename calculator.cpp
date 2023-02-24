#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstdio>

using namespace std;

const bool DEBUG = false;

enum SymbolType {
    INTEGER,
    ADDITION,
    SUBTRACTION,
    MULTIPLICATION,
    DICE,
    LESSTHAN,
    GREATERTHAN,
    OPENING,
    CLOSING,
    NONE
};

enum class Priority {
    INPARENTHESIS,
    MULTIPLICATION,
    SUMM,
    COMPARISON
};

double calculateDiceProbability(const size_t val) {
    return 1.0 / (double) val;
}

class Token {
private:
    SymbolType type;
public:
    int value = 0;
    bool highPriority = false;
    vector<int> diceValues;
    vector<double> diceProbabilities;
    Token() { }

    Token(const int v)
    : value{v}, type{INTEGER} { }

    Token(const int v, const SymbolType &t)
    : value{v}, type{t} {
        if (t == DICE) {
            for (size_t i = 1; i <= v; i++)
                diceValues.push_back(i);

            double probability = calculateDiceProbability(v);
            for (size_t i = 1; i <= v; i++)
                diceProbabilities.push_back(probability);
        }
    }

    Token(const SymbolType &t, bool high = false)
    : type{t}, highPriority{high} { }

    Token(const vector<int> &values)
    : type{DICE}, value{static_cast<int>(values.size())} {
        for (size_t i = 0; i < values.size(); i++)
            diceValues.push_back(values[i]);
    }

    SymbolType getType() const noexcept {
        return type;
    }

    const char *getTypeAsText() const noexcept {
        return getSymbolTypeAsText(type);
    }

    bool isOperator() const noexcept {
        return (
            getType() == MULTIPLICATION ||
            getType() == ADDITION ||
            getType() == SUBTRACTION ||
            getType() == LESSTHAN ||
            getType() == GREATERTHAN
        );
    }

    string format() const {
        if (getType() == DICE) {
            stringstream ss;
            for (size_t i {}; i < diceValues.size(); i++)
                ss << diceValues[i]
                    << ((i != diceValues.size() - 1) ? " " : "");
            return ss.str();
        }
        else
            return to_string(value);
    }

    static const char *getSymbolTypeAsText(const SymbolType &type) noexcept {
        switch(type) {
            case INTEGER:
                return "Integer";
            case ADDITION:
                return "Addition operator";
            case SUBTRACTION:
                return "Subtraction operator";
            case MULTIPLICATION:
                return "Multiplication operator";
            case DICE:
                return "Dice";
            case LESSTHAN:
                return "Less than operator";
            case GREATERTHAN:
                return "Greater than operator";
            case OPENING:
                return "Opening parentheses";
            case CLOSING:
                return "Closing parentheses";
            case NONE:
                return "None";
            default:
                return "Undefined";
        }
    }
};

class Operation {
public:
    const SymbolType action;
    pair<Token, Token> operands;
    pair<SymbolType, SymbolType> types;
    Operation(const SymbolType &_action, const Token &a, const Token &b)
    : action{_action}, operands(a, b) {
        types = pair<SymbolType, SymbolType>(
            a.getType(),
            b.getType()
        );
    }

    Token evaluate() const {
        if (types.first == INTEGER && types.second == INTEGER) {
            Token result(INTEGER);
            if (action == ADDITION) {
                result.value = operands.first.value + operands.second.value;
            }
            else if (action == SUBTRACTION) {
                result.value = operands.first.value - operands.second.value;
            }
            else if (action == MULTIPLICATION) {
                result.value = operands.first.value * operands.second.value;
            }
            else if (action == LESSTHAN) {
                result.value = ((operands.first.value < operands.second.value) ? 1 : 0);
            }
            else if (action == GREATERTHAN) {
                result.value = ((operands.first.value > operands.second.value) ? 1 : 0);
            }
            if (DEBUG) printLog(result);
            return result;
        }
        else if (types.first == DICE && types.second == INTEGER) {
            Token result = operands.first;
            if (action == ADDITION) {
                for (size_t i {}; i < result.diceValues.size(); i++)
                    result.diceValues[i] += operands.second.value;
            } else if (action == SUBTRACTION) {
                for (size_t i {}; i < result.diceValues.size(); i++)
                    result.diceValues[i] -= operands.second.value;
            } else if (action == LESSTHAN) {
                vector<int> newres {0, 1};
                int comp = operands.second.value;
                result = newres;
                result.diceProbabilities.push_back(0);
                result.diceProbabilities.push_back(0);
                for (size_t i {}; i < operands.first.diceValues.size(); i++) {
                    if (comp > operands.first.diceValues[i]) {
                        result.diceProbabilities[1] += operands.first.diceProbabilities[i];
                    }
                    else {
                        result.diceProbabilities[0] += operands.first.diceProbabilities[i];
                    }
                }
            } else if (action == GREATERTHAN) {
                vector<int> newres {0, 1};
                int comp = operands.second.value;
                result = newres;
                result.diceProbabilities.push_back(0);
                result.diceProbabilities.push_back(0);
                for (size_t i {}; i < operands.first.diceValues.size(); i++) {
                    if (comp < operands.first.diceValues[i]) {
                        result.diceProbabilities[1] += operands.first.diceProbabilities[i];
                    }
                    else {
                        result.diceProbabilities[0] += operands.first.diceProbabilities[i];
                    }
                }
            } else if (action == MULTIPLICATION) {
                int comp = operands.second.value;
                for (size_t i {}; i < operands.first.diceValues.size(); i++) {
                    result.diceValues[i] *= comp;
                }
            }
            if (DEBUG) printLog(result);
            return result;
        }
        else if (types.first == INTEGER && types.second == DICE) {
            Token result = operands.second;
            if (action == ADDITION) {
                for (size_t i {}; i < result.diceValues.size(); i++)
                    result.diceValues[i] += operands.first.value;
            } else if (action == SUBTRACTION) {
                for (size_t i {}; i < result.diceValues.size(); i++)
                    result.diceValues[i] = operands.first.value - result.diceValues[i];
            } else if (action == LESSTHAN) {
                vector<int> newres {0, 1};
                int comp = operands.first.value;
                result = newres;
                result.diceProbabilities.push_back(0);
                result.diceProbabilities.push_back(0);
                for (size_t i {}; i < operands.second.diceValues.size(); i++) {
                    if (comp < operands.second.diceValues[i]) {
                        result.diceProbabilities[1] += operands.second.diceProbabilities[i];
                    }
                    else {
                        result.diceProbabilities[0] += operands.second.diceProbabilities[i];
                    }
                }
            } else if (action == GREATERTHAN) {
                vector<int> newres {0, 1};
                int comp = operands.first.value;
                result = newres;
                result.diceProbabilities.push_back(0);
                result.diceProbabilities.push_back(0);
                for (size_t i {}; i < operands.second.diceValues.size(); i++) {
                    if (comp > operands.second.diceValues[i]) {
                        result.diceProbabilities[1] += operands.second.diceProbabilities[i];
                    }
                    else {
                        result.diceProbabilities[0] += operands.second.diceProbabilities[i];
                    }
                }
            } else if (action == MULTIPLICATION) {
                int comp = operands.first.value;
                for (size_t i {}; i < operands.second.diceValues.size(); i++) {
                    result.diceValues[i] *= comp;
                }
            }
            if (DEBUG) printLog(result);
            return result;
        }
        else {  // DICE && DICE
            Token result = operands.first;
            vector<int> possibleNumbers;
            vector<double> numbersProbabilities;
            for (size_t i {}; i < operands.first.diceValues.size(); i++) {
                auto dice1 = operands.first.diceValues[i];
                auto prob1 = operands.first.diceProbabilities[i];
                for (size_t j {}; j < operands.second.diceValues.size(); j++) {
                    auto dice2 = operands.second.diceValues[j];
                    auto prob2 = operands.second.diceProbabilities[j];
                    int summ = Operation(action, Token(dice1), Token(dice2)).evaluate().value;
                    bool numberIsInTheList = false;

                    size_t k{}, diceInd{};
                    for (auto number: possibleNumbers) {
                        if (number == summ) {
                            numberIsInTheList = true;
                            diceInd = k;
                            break;
                        }
                        k++;
                    }
                    if (!numberIsInTheList) {
                        possibleNumbers.push_back(summ);
                        numbersProbabilities.push_back(prob1 * prob2);
                    }
                    else
                        numbersProbabilities[diceInd] += (prob1 * prob2);
                }
            }
            result = possibleNumbers;
            for (auto prob: numbersProbabilities)
                result.diceProbabilities.push_back(prob);
            if (DEBUG) printLog(result);
            return result;
        }
    }

private:
    void printLog(const Token &R) const {
        cerr << "Operation: " << Token::getSymbolTypeAsText(action)
            << ", operands: " << operands.first.format()
            << " and " << operands.second.format() << ". Result = "
            << R.format() << endl;
    }
};

bool doesOperatorSuitPriority(const Token &token, const Priority &level) {
    const SymbolType operationType = token.getType();

    if (level == Priority::INPARENTHESIS && token.highPriority)
        return true;

    else if (level == Priority::MULTIPLICATION && operationType == MULTIPLICATION)
        return true;

    else if (level == Priority::COMPARISON && (operationType == LESSTHAN || operationType == GREATERTHAN))
        return true;

    else if (level == Priority::SUMM && (operationType == SUBTRACTION || operationType == ADDITION))
        return true;

    else
        return false;
}

vector<Priority> prioritiesOrder {
    Priority::INPARENTHESIS,
    Priority::MULTIPLICATION,
    Priority::SUMM,
    Priority::COMPARISON
};

class Expression {
private:
    vector<Token> entities;
    vector<Token> operators;
    vector<pair<int, double>> probabilities;

public:
    Expression(const string &input) {
        size_t size = input.size();
        size_t i {};
        SymbolType previous = NONE;
        bool isInsideOfParentheses = false;
        vector<size_t> digits;

        while(i <= size) {
            char c = input[i];
            if (c >= '0' && c <= '9') {
                int number = c - '0';
                digits.push_back(number);
                previous = INTEGER;
                i++;
            }
            else {
                if (previous == INTEGER) {
                    int tempNumber = 0;
                    int rank = digits.size() - 1;
                    for (auto digit: digits)
                        tempNumber += pow(10, rank--) * digit;
                    
                    digits.clear();
                    addToken(Token(tempNumber));
                }

                if (c == 'd') {
                    previous = DICE;
                    int number = input[i + 1] - '0';
                    addToken(Token(number, DICE)); i += 2;
                }
                else if (c == '+') {
                    previous = ADDITION;
                    addToken(Token(previous, isInsideOfParentheses)); i++;
                }
                else if (c == '-') {
                    previous = SUBTRACTION;
                    addToken(Token(previous, isInsideOfParentheses)); i++;
                }
                else if (c == '*') {
                    previous = MULTIPLICATION;
                    addToken(Token(previous, isInsideOfParentheses)); i++;
                }
                else if (c == '(') {
                    previous = OPENING;
                    isInsideOfParentheses = true;
                    addToken(Token(previous)); i++;
                }
                else if (c == ')') {
                    previous = CLOSING;
                    isInsideOfParentheses = false;
                    addToken(Token(previous)); i++;
                }
                else if (c == '<') {
                    previous = LESSTHAN;
                    addToken(Token(previous, isInsideOfParentheses)); i++;
                }
                else if (c == '>') {
                    previous = GREATERTHAN;
                    addToken(Token(previous, isInsideOfParentheses)); i++;
                }
                else
                    i++;
            }
        }
    }

    void addToken(const Token &token) {
        if (token.getType() == INTEGER || token.getType() == DICE)
            entities.push_back(token);
        else if (token.isOperator())
            operators.push_back(token);
    }

    void printTokens(const vector<Token> &tokens) const {
        for (auto token: tokens) {
            if (token.value)
                cerr << "Token: " << token.getTypeAsText() << ", value = " << token.value << endl;
            else
                cerr << "Token: " << token.getTypeAsText() << ", high priority: " << (token.highPriority ? "true" : "false") << endl;
        }
    }

    void printEntities() const {
        printTokens(entities);
    }

    void printOperators() const {
        printTokens(operators);
    }

    void evaluate() {
        size_t j {};
        while (j < prioritiesOrder.size()) {
            Priority currentPriority = prioritiesOrder[j];
            while (hasOperatorsOf(currentPriority)) {
                size_t index {};
                for (auto _operator: operators) {
                    if (doesOperatorSuitPriority(_operator, currentPriority)) {
                        performOperation(
                            Operation(
                                _operator.getType(),
                                entities[index],
                                entities[index + 1]
                            ), index
                        );
                        break;
                    }
                    index++;
                }
            }
            j++;
        }
        Token answer = entities[0];
        if (answer.getType() == INTEGER)
            probabilities.push_back(pair<int, double>(answer.value, 100));
        else if (answer.getType() == DICE) {
            for (size_t i {}; i < answer.diceValues.size(); i++)
                probabilities.push_back(
                    pair<int, double>(
                        answer.diceValues[i],
                        answer.diceProbabilities[i] * 100
                    )
                );
        }
    }

    void printAnswer() const {
        const size_t SIZE = probabilities.size();
        int elements[SIZE];
        for (size_t i {}; i < SIZE; i++)
            elements[i] = probabilities[i].first;

        // Sorting
        for (size_t j = 0; j < SIZE; j++) {
            for (size_t i = 1; i < SIZE - j; i++) {
                if (elements[i - 1] > elements[i]) {
                    auto t = elements[i - 1];
                    elements[i - 1] = elements[i];
                    elements[i] = t;
                }
            }
        }

        // Printing according to sorted order
        for (size_t i {}; i < SIZE; i++) {
            for (size_t j {}; j < SIZE; j++) {
                if (probabilities[j].first == elements[i])
                    printf("%d %.2f\n", probabilities[j].first, probabilities[j].second);
            }
        }
    }
private:
    bool hasOperatorsOf(const Priority &priority) const {
        bool flag = false;
        for (auto token: operators) {
            if (doesOperatorSuitPriority(token, priority))
                flag = true;
        }
        return flag;
    }

    void performOperation(const Operation &operation, const size_t index) {
        entities[index] = operation.evaluate();

        for (size_t i = index + 1; i < entities.size() - 1; i++)
            entities[i] = entities[i + 1];
        entities.pop_back();

        for (size_t i = index; i < operators.size() - 1; i++)
            operators[i] = operators[i + 1];
        operators.pop_back();
    }
};

int main(int argc, char **argv) {
    string line;
    getline(cin, line);

    const int size = line.size();
    if (size < 2)
        return -1;

    Expression expression(line);
    expression.evaluate();
    expression.printAnswer();
}
