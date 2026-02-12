#include "backend.hpp"

#include <entropy8/entropy8.h>
#include <entropy8/multi_format.h>
#include <entropy8/io.h>
#include <entropy8/codec.h>

#include <QVariantMap>
#include <QFileInfo>
#include <QDir>

#include <cstdio>
#include <cstring>

// ═════════════════════════════════════════════════════════════════════════════
// Format definitions (RGB hex colours for QML)
// ═════════════════════════════════════════════════════════════════════════════
struct FormatInfo {
    const char *name;
    const char *extension;
    const char *color;        // "#RRGGBB"
    bool        supported;
    bool        encrypts;     // supports encryption
    enum E8Format e8fmt;
};

static const FormatInfo kFormats[] = {
    {"7Z",    ".7z",  "#E07020", true,  true,  E8_FMT_7Z   },
    {"BZIP2", ".bz2", "#3080E0", true,  false, E8_FMT_BZIP2},
    {"E8",    ".e8",  "#4090F5", true,  true,  E8_FMT_E8   },
    {"GZIP",  ".gz",  "#40B040", true,  false, E8_FMT_GZIP },
    {"LZIP",  ".lz",  "#7040E0", true,  false, E8_FMT_LZIP },
    {"TAR",   ".tar", "#C0A060", true,  false, E8_FMT_TAR  },
    {"XZ",    ".xz",  "#D06050", true,  false, E8_FMT_XZ   },
    {"ZIP",   ".zip", "#E09030", true,  true,  E8_FMT_ZIP  },
};
static constexpr int kFormatCount = sizeof(kFormats) / sizeof(kFormats[0]);

// ═════════════════════════════════════════════════════════════════════════════
// Constructor
// ═════════════════════════════════════════════════════════════════════════════
ArchiveBackend::ArchiveBackend(QObject *parent)
    : QObject(parent)
{}

// ═════════════════════════════════════════════════════════════════════════════
// Format helpers
// ═════════════════════════════════════════════════════════════════════════════
QVariantList ArchiveBackend::formats() const {
    QVariantList list;
    for (int i = 0; i < kFormatCount; ++i) {
        QVariantMap m;
        m["name"]      = kFormats[i].name;
        m["extension"] = kFormats[i].extension;
        m["color"]     = kFormats[i].color;
        m["supported"] = kFormats[i].supported;
        m["encrypts"]  = kFormats[i].encrypts;
        list.append(m);
    }
    return list;
}

int     ArchiveBackend::formatIndex()     const { return m_formatIndex; }
QString ArchiveBackend::formatName()      const { return kFormats[m_formatIndex].name; }
QString ArchiveBackend::formatColor()     const { return kFormats[m_formatIndex].color; }
bool    ArchiveBackend::formatSupported() const { return kFormats[m_formatIndex].supported; }
bool    ArchiveBackend::supportsEncryption() const { return kFormats[m_formatIndex].encrypts; }

void ArchiveBackend::setFormatIndex(int v) {
    if (v < 0 || v >= kFormatCount || v == m_formatIndex) return;
    m_formatIndex = v;
    emit formatIndexChanged();
}

// ═════════════════════════════════════════════════════════════════════════════
// Simple property getters / setters
// ═════════════════════════════════════════════════════════════════════════════
int     ArchiveBackend::methodIndex()     const { return m_methodIndex; }
void    ArchiveBackend::setMethodIndex(int v)   { if (v == m_methodIndex) return; m_methodIndex = v; emit methodIndexChanged(); }

int     ArchiveBackend::codecIndex()      const { return m_codecIndex; }
void    ArchiveBackend::setCodecIndex(int v)    { if (v == m_codecIndex) return; m_codecIndex = v; emit codecIndexChanged(); }

QString ArchiveBackend::splitValue()      const { return m_splitValue; }
void    ArchiveBackend::setSplitValue(const QString &v) { if (v == m_splitValue) return; m_splitValue = v; emit splitValueChanged(); }

QString ArchiveBackend::password()        const { return m_password; }
void    ArchiveBackend::setPassword(const QString &v) { if (v == m_password) return; m_password = v; emit passwordChanged(); }

QString ArchiveBackend::passwordRepeat()  const { return m_passwordRepeat; }
void    ArchiveBackend::setPasswordRepeat(const QString &v) { if (v == m_passwordRepeat) return; m_passwordRepeat = v; emit passwordRepeatChanged(); }

bool ArchiveBackend::encryptFilenames()  const { return m_encryptFilenames; }
void ArchiveBackend::setEncryptFilenames(bool v) { if (v == m_encryptFilenames) return; m_encryptFilenames = v; emit encryptFilenamesChanged(); }

bool ArchiveBackend::solidArchive()      const { return m_solidArchive; }
void ArchiveBackend::setSolidArchive(bool v) { if (v == m_solidArchive) return; m_solidArchive = v; emit solidArchiveChanged(); }

bool ArchiveBackend::selfExtracting()    const { return m_selfExtracting; }
void ArchiveBackend::setSelfExtracting(bool v) { if (v == m_selfExtracting) return; m_selfExtracting = v; emit selfExtractingChanged(); }

bool ArchiveBackend::verifyIntegrity()   const { return m_verifyIntegrity; }
void ArchiveBackend::setVerifyIntegrity(bool v) { if (v == m_verifyIntegrity) return; m_verifyIntegrity = v; emit verifyIntegrityChanged(); }

bool ArchiveBackend::deleteAfter()       const { return m_deleteAfter; }
void ArchiveBackend::setDeleteAfter(bool v) { if (v == m_deleteAfter) return; m_deleteAfter = v; emit deleteAfterChanged(); }

bool ArchiveBackend::archiveSeparately() const { return m_archiveSeparately; }
void ArchiveBackend::setArchiveSeparately(bool v) { if (v == m_archiveSeparately) return; m_archiveSeparately = v; emit archiveSeparatelyChanged(); }

// ── Viewer ───────────────────────────────────────────────────────────────────
bool         ArchiveBackend::archiveOpen()      const { return m_archiveOpen; }
bool         ArchiveBackend::archiveEncrypted() const { return m_archiveEncrypted; }
bool         ArchiveBackend::showViewer()       const { return m_showViewer; }
QString      ArchiveBackend::archivePath() const { return m_archivePath; }
QVariantList ArchiveBackend::entries()     const { return m_entries; }
int          ArchiveBackend::entryCount()  const { return m_entries.size(); }

QString ArchiveBackend::totalSize() const {
    return formatSize(m_totalUncompressed);
}

void ArchiveBackend::setShowViewer(bool v) {
    if (v == m_showViewer) return;
    m_showViewer = v;
    emit showViewerChanged();
}

// ── Status ───────────────────────────────────────────────────────────────────
QString ArchiveBackend::statusMessage() const { return m_statusMessage; }
bool    ArchiveBackend::statusError()   const { return m_statusError; }

void ArchiveBackend::setStatus(const QString &msg, bool error) {
    m_statusMessage = msg;
    m_statusError   = error;
    emit statusChanged();
}

// ═════════════════════════════════════════════════════════════════════════════
// Utility
// ═════════════════════════════════════════════════════════════════════════════
QString ArchiveBackend::formatSize(quint64 bytes) const {
    if (bytes < 1024)
        return QString::number(bytes) + " B";
    if (bytes < 1024ULL * 1024)
        return QString::number(bytes / 1024.0, 'f', 1) + " KB";
    if (bytes < 1024ULL * 1024 * 1024)
        return QString::number(bytes / (1024.0 * 1024.0), 'f', 1) + " MB";
    return QString::number(bytes / (1024.0 * 1024.0 * 1024.0), 'f', 2) + " GB";
}

// ═════════════════════════════════════════════════════════════════════════════
// Engine operations (using multi_format API)
// ═════════════════════════════════════════════════════════════════════════════

static const QStringList kArchiveExtensions = {
    ".e8", ".7z", ".zip", ".tar", ".gz", ".bz2", ".xz", ".lz",
    ".tgz", ".tbz2", ".txz", ".tar.gz", ".tar.bz2", ".tar.xz", ".tar.lz"
};

static bool isArchiveFile(const QString &path) {
    QString lower = path.toLower();
    for (const auto &ext : kArchiveExtensions) {
        if (lower.endsWith(ext)) return true;
    }
    return false;
}

/**
 * Detect if a file is an encrypted archive by checking magic bytes.
 * E8 encrypted: "E8AE", 7Z/ZIP encryption detected by libarchive on open.
 */
static bool detectEncryptedE8(const QString &path) {
    std::string spath = path.toStdString();
    FILE *fp = fopen(spath.c_str(), "rb");
    if (!fp) return false;
    uint8_t mag[4] = {0};
    fread(mag, 1, 4, fp);
    fclose(fp);
    return (mag[0] == 'E' && mag[1] == '8' && mag[2] == 'A' && mag[3] == 'E');
}

bool ArchiveBackend::openArchive(const QString &path) {
    std::string spath = path.toStdString();

    /* Use password as a persistent std::string to avoid dangling pointer */
    std::string pw_str = m_password.toStdString();
    const char *pw_ptr = pw_str.empty() ? nullptr : pw_str.c_str();

    /* Detect if archive is encrypted (E8 magic byte check) */
    bool isEncryptedE8 = detectEncryptedE8(path);

    char **paths = nullptr;
    uint64_t *sizes = nullptr;
    size_t count = 0;

    int r = e8_mf_list(spath.c_str(), pw_ptr, &paths, &sizes, &count);
    if (r != 0) {
        if (isEncryptedE8 && pw_str.empty()) {
            setStatus("Encrypted archive. Enter password and drop again.", true);
        } else if (isEncryptedE8 && !pw_str.empty()) {
            setStatus("Wrong password or corrupt archive.", true);
        } else if (pw_str.empty()) {
            setStatus("Cannot open archive (may be encrypted). Try with a password.", true);
        } else {
            setStatus("Cannot open archive: " + path, true);
        }
        return false;
    }

    m_entries.clear();
    m_totalUncompressed = 0;
    m_archiveEncrypted = isEncryptedE8 || (!pw_str.empty() && count > 0);

    for (size_t i = 0; i < count; ++i) {
        QVariantMap entry;
        entry["path"]             = QString::fromUtf8(paths[i]);
        entry["uncompressedSize"] = static_cast<quint64>(sizes ? sizes[i] : 0);
        entry["compressedSize"]   = static_cast<quint64>(sizes ? sizes[i] : 0);
        entry["codecId"]          = 0;
        entry["codecName"]        = "—";
        entry["sizeStr"]          = formatSize(sizes ? sizes[i] : 0);

        m_entries.append(entry);
        if (sizes) m_totalUncompressed += sizes[i];
    }

    e8_mf_free_list(paths, sizes, count);

    m_archivePath = path;
    m_archiveOpen = true;

    QString statusMsg = QString("Opened: %1 (%2 entries)").arg(path).arg(count);
    if (m_archiveEncrypted)
        statusMsg += QString::fromUtf8("  \xF0\x9F\x94\x92");
    setStatus(statusMsg, false);

    emit archivePathChanged();
    emit archiveOpenChanged();
    emit entriesChanged();
    return true;
}

bool ArchiveBackend::createArchive(const QStringList &files) {
    if (files.isEmpty()) {
        setStatus("No files to add.", true);
        return false;
    }

    const auto &fmt = kFormats[m_formatIndex];

    // Build output path
    QFileInfo fi(files.first());
    QString outputPath = fi.absolutePath() + "/" + fi.completeBaseName() + fmt.extension;

    // Prepare file list for C API
    std::vector<std::string> file_strs;
    std::vector<const char*> file_ptrs;
    for (const auto &f : files) {
        file_strs.push_back(f.toStdString());
        file_ptrs.push_back(file_strs.back().c_str());
    }

    // Password (only if format supports encryption and password is set)
    std::string pw_str = m_password.toStdString();
    const char *pw_ptr = nullptr;
    if (fmt.encrypts && !pw_str.empty()) {
        pw_ptr = pw_str.c_str();
    }

    int r = e8_mf_create(outputPath.toStdString().c_str(),
                         fmt.e8fmt,
                         file_ptrs.data(),
                         file_ptrs.size(),
                         pw_ptr,
                         m_methodIndex,
                         nullptr, nullptr);

    if (r < 0) {
        setStatus(QString("Failed to create %1 archive.").arg(fmt.name), true);
        return false;
    }

    setStatus(QString("Created %1 (%2 files)").arg(outputPath).arg(r), false);

    // Auto-open the new archive
    openArchive(outputPath);
    m_showViewer = true;
    emit showViewerChanged();
    return true;
}

// ═════════════════════════════════════════════════════════════════════════════
// QML-invokable operations
// ═════════════════════════════════════════════════════════════════════════════
void ArchiveBackend::handleDroppedUrls(const QList<QUrl> &urls) {
    QStringList archivePaths;
    QStringList filePaths;

    for (const auto &url : urls) {
        QString local = url.toLocalFile();
        if (local.isEmpty()) continue;

        if (isArchiveFile(local)) {
            archivePaths.append(local);
        } else {
            filePaths.append(local);
        }
    }

    // Open the first archive found
    if (!archivePaths.isEmpty()) {
        openArchive(archivePaths.first());
        m_showViewer = true;
        emit showViewerChanged();
    }

    // Compress any non-archive files
    if (!filePaths.isEmpty()) {
        createArchive(filePaths);
    }
}

void ArchiveBackend::extractAll(const QUrl &folderUrl) {
    QString outputDir = folderUrl.toLocalFile();
    if (outputDir.isEmpty()) {
        setStatus("No output directory selected.", true);
        return;
    }

    if (!m_archiveOpen) {
        setStatus("No archive open.", true);
        return;
    }

    std::string pw_str = m_password.toStdString();
    const char *pw_ptr = pw_str.empty() ? nullptr : pw_str.c_str();

    int r = e8_mf_extract(m_archivePath.toStdString().c_str(),
                          outputDir.toStdString().c_str(),
                          pw_ptr,
                          nullptr, nullptr);

    if (r < 0) {
        QString msg;
        if (m_archiveEncrypted && pw_str.empty())
            msg = "Extraction failed. Enter password for encrypted archive.";
        else if (m_archiveEncrypted)
            msg = "Extraction failed. Wrong password?";
        else
            msg = "Extraction failed.";
        setStatus(msg, true);
        emit extractionFinished(false, msg);
        return;
    }

    QString msg = QString("Extracted %1 file(s) to %2").arg(r).arg(outputDir);
    setStatus(msg, false);
    emit extractionFinished(true, msg);
}

void ArchiveBackend::openFileFromPath(const QString &filePath) {
    if (filePath.isEmpty()) return;

    // Normalize path separators
    QString path = QDir::toNativeSeparators(filePath);

    if (isArchiveFile(path)) {
        openArchive(path);
        m_showViewer = true;
        emit showViewerChanged();
    } else {
        // Non-archive file: add to compression queue
        createArchive(QStringList{path});
    }
}
