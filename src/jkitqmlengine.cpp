#include "jkitqmlengine.h"
#include "StreamParser/StreamParser.h"
#include "StreamParser/BinaryStream.h"
JKitQmlEngine::JKitQmlEngine(QObject *parent)
    : QQmlApplicationEngine(parent)
{
    qmlRegisterType<StreamParser>("com.jkit.app", 1, 0, "StreamParser");
    qmlRegisterType<BinaryStream>("com.jkit.app", 1, 0, "BinaryStream");

}

bool JKitQmlEngine::run()
{
    this->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (this->rootObjects().isEmpty()) {
        return false;
    }
    return true;
}
