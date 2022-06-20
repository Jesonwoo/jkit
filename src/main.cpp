#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>
#include "jkitqmlengine.h"
#include "qdebug.h"
#include <time.h>
#include "../libmedia/MediaParsing/H264Parser.h"

using namespace std;
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setOrganizationName("jkit");
    QGuiApplication app(argc, argv);

    JKitQmlEngine engine;
    if(!engine.run()) {
        qCritical() << "Failed to run engine";
        return -1;
    }

//    jkit::H264Parser p("D:\\develop\\tool\\ffmpeg\\ffmpeg-static\\bin\\long-out.h264");
//    clock_t start = clock();
//    p.init();
//    clock_t end = clock();
//    qDebug() << (double)(end-start)/CLOCKS_PER_SEC;
////    p.showNalus();
//    p.showStreamInfo();
    return app.exec();
}
