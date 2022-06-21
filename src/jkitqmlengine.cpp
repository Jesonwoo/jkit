#include "jkitqmlengine.h"
#include "StreamParser/StreamParser.h"
JKitQmlEngine::JKitQmlEngine(QObject *parent)
    : QQmlApplicationEngine(parent)
{
    qmlRegisterType<StreamParser>("com.jkit.app", 1, 0, "StreamParser");
}

bool JKitQmlEngine::run()
{
    this->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (this->rootObjects().isEmpty()) {
        return false;
    }
    return true;
}
