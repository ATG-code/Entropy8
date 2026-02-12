#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariantList>
#include <QUrl>
#include <cstdint>

/**
 * C++ backend that bridges the entropy8 multi-format engine with the QML front-end.
 * Every piece of UI state is exposed as a Q_PROPERTY so QML can bind to it.
 */
class ArchiveBackend : public QObject {
    Q_OBJECT

    // ── Format ───────────────────────────────────────────────────────────
    Q_PROPERTY(QVariantList formats       READ formats       CONSTANT)
    Q_PROPERTY(int          formatIndex   READ formatIndex   WRITE setFormatIndex   NOTIFY formatIndexChanged)
    Q_PROPERTY(QString      formatName    READ formatName    NOTIFY formatIndexChanged)
    Q_PROPERTY(QString      formatColor   READ formatColor   NOTIFY formatIndexChanged)
    Q_PROPERTY(bool         formatSupported READ formatSupported NOTIFY formatIndexChanged)
    Q_PROPERTY(bool         supportsEncryption READ supportsEncryption NOTIFY formatIndexChanged)

    // ── Method / codec ───────────────────────────────────────────────────
    Q_PROPERTY(int methodIndex READ methodIndex WRITE setMethodIndex NOTIFY methodIndexChanged)
    Q_PROPERTY(int codecIndex  READ codecIndex  WRITE setCodecIndex  NOTIFY codecIndexChanged)

    // ── Split ────────────────────────────────────────────────────────────
    Q_PROPERTY(QString splitValue READ splitValue WRITE setSplitValue NOTIFY splitValueChanged)

    // ── Password ─────────────────────────────────────────────────────────
    Q_PROPERTY(QString password       READ password       WRITE setPassword       NOTIFY passwordChanged)
    Q_PROPERTY(QString passwordRepeat READ passwordRepeat WRITE setPasswordRepeat NOTIFY passwordRepeatChanged)

    // ── Options ──────────────────────────────────────────────────────────
    Q_PROPERTY(bool encryptFilenames  READ encryptFilenames  WRITE setEncryptFilenames  NOTIFY encryptFilenamesChanged)
    Q_PROPERTY(bool solidArchive      READ solidArchive      WRITE setSolidArchive      NOTIFY solidArchiveChanged)
    Q_PROPERTY(bool selfExtracting    READ selfExtracting    WRITE setSelfExtracting    NOTIFY selfExtractingChanged)
    Q_PROPERTY(bool verifyIntegrity   READ verifyIntegrity   WRITE setVerifyIntegrity   NOTIFY verifyIntegrityChanged)
    Q_PROPERTY(bool deleteAfter       READ deleteAfter       WRITE setDeleteAfter       NOTIFY deleteAfterChanged)
    Q_PROPERTY(bool archiveSeparately READ archiveSeparately WRITE setArchiveSeparately NOTIFY archiveSeparatelyChanged)

    // ── Archive viewer ───────────────────────────────────────────────────
    Q_PROPERTY(bool         archiveOpen READ archiveOpen NOTIFY archiveOpenChanged)
    Q_PROPERTY(bool         showViewer  READ showViewer  WRITE setShowViewer NOTIFY showViewerChanged)
    Q_PROPERTY(QString      archivePath READ archivePath NOTIFY archivePathChanged)
    Q_PROPERTY(QVariantList entries     READ entries     NOTIFY entriesChanged)
    Q_PROPERTY(int          entryCount  READ entryCount  NOTIFY entriesChanged)
    Q_PROPERTY(QString      totalSize   READ totalSize   NOTIFY entriesChanged)

    // ── Status ───────────────────────────────────────────────────────────
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusChanged)
    Q_PROPERTY(bool    statusError   READ statusError   NOTIFY statusChanged)

public:
    explicit ArchiveBackend(QObject *parent = nullptr);

    // ── Getters ──────────────────────────────────────────────────────────
    QVariantList formats() const;
    int          formatIndex() const;
    QString      formatName() const;
    QString      formatColor() const;
    bool         formatSupported() const;
    bool         supportsEncryption() const;

    int     methodIndex() const;
    int     codecIndex() const;
    QString splitValue() const;
    QString password() const;
    QString passwordRepeat() const;

    bool encryptFilenames() const;
    bool solidArchive() const;
    bool selfExtracting() const;
    bool verifyIntegrity() const;
    bool deleteAfter() const;
    bool archiveSeparately() const;

    bool         archiveOpen() const;
    bool         showViewer() const;
    QString      archivePath() const;
    QVariantList entries() const;
    int          entryCount() const;
    QString      totalSize() const;

    QString statusMessage() const;
    bool    statusError() const;

    // ── Setters ──────────────────────────────────────────────────────────
    void setFormatIndex(int v);
    void setMethodIndex(int v);
    void setCodecIndex(int v);
    void setSplitValue(const QString &v);
    void setPassword(const QString &v);
    void setPasswordRepeat(const QString &v);
    void setEncryptFilenames(bool v);
    void setSolidArchive(bool v);
    void setSelfExtracting(bool v);
    void setVerifyIntegrity(bool v);
    void setDeleteAfter(bool v);
    void setArchiveSeparately(bool v);
    void setShowViewer(bool v);

    // ── Operations (callable from QML) ───────────────────────────────────
    Q_INVOKABLE void handleDroppedUrls(const QList<QUrl> &urls);
    Q_INVOKABLE void extractAll(const QUrl &folderUrl);
    Q_INVOKABLE QString formatSize(quint64 bytes) const;

signals:
    void formatIndexChanged();
    void methodIndexChanged();
    void codecIndexChanged();
    void splitValueChanged();
    void passwordChanged();
    void passwordRepeatChanged();
    void encryptFilenamesChanged();
    void solidArchiveChanged();
    void selfExtractingChanged();
    void verifyIntegrityChanged();
    void deleteAfterChanged();
    void archiveSeparatelyChanged();
    void archiveOpenChanged();
    void showViewerChanged();
    void archivePathChanged();
    void entriesChanged();
    void statusChanged();

private:
    bool openArchive(const QString &path);
    bool createArchive(const QStringList &files);
    void setStatus(const QString &msg, bool error);

    // State
    int     m_formatIndex   = 2;   // E8
    int     m_methodIndex   = 2;   // Normal
    int     m_codecIndex    = 3;   // Zstd
    QString m_splitValue;
    QString m_password;
    QString m_passwordRepeat;

    bool m_encryptFilenames  = false;
    bool m_solidArchive      = true;
    bool m_selfExtracting    = false;
    bool m_verifyIntegrity   = false;
    bool m_deleteAfter       = false;
    bool m_archiveSeparately = false;

    bool         m_archiveOpen = false;
    bool         m_showViewer  = false;
    QString      m_archivePath;
    QVariantList m_entries;
    quint64      m_totalUncompressed = 0;
    quint64      m_totalCompressed   = 0;

    QString m_statusMessage;
    bool    m_statusError = false;
};
