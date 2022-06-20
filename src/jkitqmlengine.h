#ifndef JKITQMLENGINE_H
#define JKITQMLENGINE_H

#include <QQmlApplicationEngine>

class JKitQmlEngine : public QQmlApplicationEngine
{
    Q_OBJECT
public:
    JKitQmlEngine(QObject *parent=nullptr);
    bool run(void);
};

#endif // JKITQMLENGINE_H
