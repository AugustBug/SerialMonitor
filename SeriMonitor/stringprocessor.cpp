/*****************************************
  *                                      *
  * Ahmert - Serial Monitor - 21.07.2016 *
  *                                      *
  *****************************************/

#include "stringprocessor.h"

StringProcessor::StringProcessor()
{

}

QString StringProcessor::convertToAsciiView(QString original) {
    QString converted = "";
    for(int i=0; i<original.size(); i++) {
        converted.append(original.at(i));
        converted.append(" |");
    }
    return converted;
}

QString StringProcessor::convertToHexView(QString original) {

    QString converted = "";
    for(int i=0; i<original.size(); i++) {
        converted.append(convertOneCharHex(original.data()[i].unicode() / 16));
        converted.append(convertOneCharHex(original.data()[i].unicode() % 16));
        converted.append("|");
    }
    return converted;
}

QString StringProcessor::convertOneCharHex(int num) {
    if(num >= 0 && num < 10) {
        return QString::number(num);
    }
    switch (num) {
    case 10:
        return "A";
    case 11:
        return "B";
    case 12:
        return "C";
    case 13:
        return "D";
    case 14:
        return "E";
    case 15:
        return "F";
    default:
        return "%"; // error char
    }
}

bool StringProcessor::isCharHex(QChar ch) {
    return (convertOneFromHex(ch) >= 0);
}

int StringProcessor::convertOneFromHex(QChar hex) {

    switch (hex.toLatin1()) {
    case '0':
        return 0;
    case '1':
        return 1;
    case '2':
        return 2;
    case '3':
        return 3;
    case '4':
        return 4;
    case '5':
        return 5;
    case '6':
        return 6;
    case '7':
        return 7;
    case '8':
        return 8;
    case '9':
        return 9;
    case 'A':
    case 'a':
        return 10;
    case 'B':
    case 'b':
        return 11;
    case 'C':
    case 'c':
        return 12;
    case 'D':
    case 'd':
        return 13;
    case 'E':
    case 'e':
        return 14;
    case 'F':
    case 'f':
        return 15;
    default:
        return -1; // error val
    }
}

QString StringProcessor::convertFromAsciiView(QString ascii) {
    QString converted = "";

    for(int i = 0; i<ascii.size(); i++) {
        if(i%3 == 0) {
            converted.append(ascii.at(i));
        } else if(i%3 == 1) {
            if(!(ascii.at(i) == ' ')) {
                return converted;
            }
        } else if(i%3 == 2) {
            if(!(ascii.at(i) == '|')) {
                return converted;
            }
        }
    }

    return converted;
}

QString StringProcessor::convertFromHexView(QString hex) {
    QString converted = "";
    char keyCode = 0;

    for(int i = 0; i<hex.size(); i++) {
        if(i%3 == 0) {
            keyCode = 16 * convertOneFromHex(hex.at(i));
        } else if(i%3 == 1) {
            keyCode += convertOneFromHex(hex.at(i));
            converted.append(QChar(keyCode));
        } else if(i%3 == 2) {
            if(!(hex.at(i) == '|')) {
                return converted;
            }
        }
    }

    return converted;
}
