#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include <QQuickWindow>
#include "jkitqmlengine.h"

using namespace std;
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("jkit");
    QQuickWindow::setSceneGraphBackend(QSGRendererInterface::OpenGLRhi);

    QGuiApplication app(argc, argv);

    JKitQmlEngine engine;
    if(!engine.run()) {
        qCritical() << "Failed to run engine";
        return -1;
    }
    return app.exec();
}
