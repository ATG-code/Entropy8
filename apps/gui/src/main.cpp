#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QFontDatabase>
#include <QFont>
#include <QQuickStyle>

#include "backend.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName("Entropy8");
    app.setOrganizationName("Entropy8");

    // Use "Basic" style so our QML fully controls visuals
    QQuickStyle::setStyle("Basic");

    // ── Load InterTight font family from embedded resources ──────────────
    const QStringList fontFiles = {
        QStringLiteral(":/fonts/InterTight-Thin.ttf"),
        QStringLiteral(":/fonts/InterTight-ExtraLight.ttf"),
        QStringLiteral(":/fonts/InterTight-Light.ttf"),
        QStringLiteral(":/fonts/InterTight-Regular.ttf"),
        QStringLiteral(":/fonts/InterTight-Medium.ttf"),
        QStringLiteral(":/fonts/InterTight-SemiBold.ttf"),
        QStringLiteral(":/fonts/InterTight-Bold.ttf"),
        QStringLiteral(":/fonts/InterTight-ExtraBold.ttf"),
        QStringLiteral(":/fonts/InterTight-Black.ttf"),
    };

    for (const auto &f : fontFiles)
        QFontDatabase::addApplicationFont(f);

    // Set application-wide default font
    QFont defaultFont("Inter Tight");
    defaultFont.setPixelSize(13);
    defaultFont.setHintingPreference(QFont::PreferNoHinting);
    app.setFont(defaultFont);

    // ── Backend ──────────────────────────────────────────────────────────
    ArchiveBackend backend;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("backend", &backend);

    engine.load(QUrl(QStringLiteral("qrc:/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
