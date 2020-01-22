/*****************************************
  *                                      *
  * Ahmert - Serial Monitor - 21.07.2016 *
  *                                      *
  *****************************************/

/**
  * String manipulation functions
  *
  */

#ifndef STRINGPROCESSOR_H
#define STRINGPROCESSOR_H

#include <QString>


class StringProcessor
{
public:
    StringProcessor();

    QString convertFromAsciiView(QString ascii);
    QString convertFromHexView(QString hex);
    int convertOneFromHex(QChar hex);

    QString convertToAsciiView(QString original);
    QString convertToHexView(QString original);
    QString convertOneCharHex(int num);

    bool isCharHex(QChar ch);
};

#endif // STRINGPROCESSOR_H
