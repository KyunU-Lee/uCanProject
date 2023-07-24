#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QFont>
#include "totalmanager.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QFont font;
    font.setFamily("qrc:D2Coding.ttf");
    QGuiApplication::setFont(font);

    QQmlApplicationEngine engine;
    qmlRegisterType<TotalManager>("TotalManager", 1,0,"TotalManager");


//    TotalManager tm;

//    engine.rootContext()->setContextProperty("myModel", &tm);

    app.setOrganizationName("PINE");

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
