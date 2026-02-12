#include "backend.hpp"

#include <entropy8/entropy8.h>
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
    const char *color;   // "#RRGGBB"
    bool        supported;
};

static const FormatInfo kFormats[] = {
    {"7Z",    ".7z",  "#E07020", false},
    {"BZIP2", ".bz2", "#3080E0", false},
    {"E8",    ".e8",  "#4090F5", true },
    {"GZIP",  ".gz",  "#40B040", false},
    {"LZIP",  ".lz",  "#7040E0", false},
    {"TAR",   ".tar", "#C0A060", false},
    {"XZ",    ".xz",  "#D06050", false},
    {"ZIP",   ".zip", "#E09030", false},
};
static constexpr int kFormatCount = sizeof(kFormats) / sizeof(kFormats[0]);

// ═════════════════════════════════════════════════════════════════════════════
// File-stream helpers (identical to the original app.cpp ones)
// ═════════════════════════════════════════════════════════════════════════════
namespace {

struct FileCtx { FILE *fp; };

static ptrdiff_t file_read(void *ctx, void *buf, size_t size) {
    return static_cast<ptrdiff_t>(fread(buf, 1, size, static_cast<FileCtx *>(ctx)->fp));
}
static ptrdiff_t file_write(void *ctx, const void *buf, size_t size) {
    return static_cast<ptrdiff_t>(fwrite(buf, 1, size, static_cast<FileCtx *>(ctx)->fp));
}
static int64_t file_seek(void *ctx, int64_t offset, int origin) {
    FILE *fp = static_cast<FileCtx *>(ctx)->fp;
#ifdef _WIN32
    _fseeki64(fp, offset, origin);
    return _ftelli64(fp);
#else
    fseeko(fp, static_cast<off_t>(offset), origin);
    return static_cast<int64_t>(ftello(fp));
#endif
}
static int file_flush(void *ctx) {
    return fflush(static_cast<FileCtx *>(ctx)->fp);
}
static int file_close(void *ctx) {
    auto *fc = static_cast<FileCtx *>(ctx);
    int r = fclose(fc->fp);
    delete fc;
    return r;
}

static E8StreamVtable g_vtable = {
    file_read, file_write, file_seek, file_flush, file_close
};

static E8Stream make_stream(FILE *fp) {
    auto *fc = new FileCtx{fp};
    E8Stream s{};
    e8_stream_create(&s, &g_vtable, fc);
    return s;
}

} // anonymous namespace

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
        list.append(m);
    }
    return list;
}

int     ArchiveBackend::formatIndex()     const { return m_formatIndex; }
QString ArchiveBackend::formatName()      const { return kFormats[m_formatIndex].name; }
QString ArchiveBackend::formatColor()     const { return kFormats[m_formatIndex].color; }
bool    ArchiveBackend::formatSupported() const { return kFormats[m_formatIndex].supported; }

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
bool         ArchiveBackend::archiveOpen() const { return m_archiveOpen; }
bool         ArchiveBackend::showViewer()  const { return m_showViewer; }
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
// Engine operations
// ═════════════════════════════════════════════════════════════════════════════
bool ArchiveBackend::openArchive(const QString &path) {
    e8_codecs_init();

    std::string spath = path.toStdString();
    FILE *fp = fopen(spath.c_str(), "rb");
    if (!fp) {
        setStatus("Cannot open: " + path, true);
        return false;
    }

    E8Stream stream = make_stream(fp);
    E8Archive *ar   = e8_archive_open(&stream);
    if (!ar) {
        e8_stream_destroy(&stream);
        setStatus("Invalid archive format: " + path, true);
        return false;
    }

    m_entries.clear();
    m_totalUncompressed = 0;
    m_totalCompressed   = 0;
    size_t n = e8_archive_count(ar);

    for (size_t i = 0; i < n; ++i) {
        char pbuf[4096];
        uint64_t usize = 0;
        e8_archive_entry(ar, i, pbuf, sizeof(pbuf), &usize);

        QVariantMap entry;
        entry["path"]             = QString::fromUtf8(pbuf);
        entry["uncompressedSize"] = static_cast<quint64>(usize);
        entry["compressedSize"]   = static_cast<quint64>(usize); // placeholder
        entry["codecId"]          = 0;
        entry["codecName"]        = "Store";
        entry["sizeStr"]          = formatSize(usize);

        m_entries.append(entry);
        m_totalUncompressed += usize;
    }

    e8_archive_close(ar);

    m_archivePath = path;
    m_archiveOpen = true;
    setStatus(QString("Opened: %1 (%2 entries)").arg(path).arg(n), false);
    emit archivePathChanged();
    emit archiveOpenChanged();
    emit entriesChanged();
    return true;
}

bool ArchiveBackend::createArchive(const QStringList &files) {
    e8_codecs_init();

    if (files.isEmpty()) {
        setStatus("No files to add.", true);
        return false;
    }

    const auto &fmt = kFormats[m_formatIndex];
    if (!fmt.supported) {
        setStatus(QString("%1 format is not yet supported.").arg(fmt.name), true);
        return false;
    }

    // Auto-generate output path
    QFileInfo fi(files.first());
    QString outputPath = fi.absolutePath() + "/" + fi.completeBaseName() + fmt.extension;

    std::string sout = outputPath.toStdString();
    FILE *out_fp = fopen(sout.c_str(), "wb");
    if (!out_fp) {
        setStatus("Cannot create: " + outputPath, true);
        return false;
    }

    E8Stream out_stream = make_stream(out_fp);
    E8Archive *ar = e8_archive_create(&out_stream);
    if (!ar) {
        e8_stream_destroy(&out_stream);
        setStatus("Failed to create archive.", true);
        return false;
    }

    int added = 0;
    for (const auto &fpath : files) {
        std::string sfp = fpath.toStdString();
        FILE *in_fp = fopen(sfp.c_str(), "rb");
        if (!in_fp) continue;
        E8Stream in_stream = make_stream(in_fp);

        QFileInfo info(fpath);
        std::string name = info.fileName().toStdString();

        int r = e8_archive_add(ar, name.c_str(), &in_stream, nullptr, nullptr);
        e8_stream_destroy(&in_stream);
        if (r == 0) added++;
    }

    e8_archive_close(ar);

    setStatus(QString("Created %1 (%2 files)").arg(outputPath).arg(added), false);

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

        if (local.endsWith(".e8", Qt::CaseInsensitive)) {
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
    e8_codecs_init();

    QString outputDir = folderUrl.toLocalFile();
    if (outputDir.isEmpty()) {
        setStatus("No output directory selected.", true);
        return;
    }

    if (!m_archiveOpen) {
        setStatus("No archive open.", true);
        return;
    }

    std::string spath = m_archivePath.toStdString();
    FILE *fp = fopen(spath.c_str(), "rb");
    if (!fp) {
        setStatus("Cannot reopen: " + m_archivePath, true);
        return;
    }

    E8Stream stream = make_stream(fp);
    E8Archive *ar   = e8_archive_open(&stream);
    if (!ar) {
        e8_stream_destroy(&stream);
        setStatus("Failed to reopen archive.", true);
        return;
    }

    size_t n = e8_archive_count(ar);
    int extracted = 0;

    for (size_t i = 0; i < n; ++i) {
        char pbuf[4096];
        uint64_t usize = 0;
        e8_archive_entry(ar, i, pbuf, sizeof(pbuf), &usize);

        // Use just the filename
        QString name = QFileInfo(QString::fromUtf8(pbuf)).fileName();
        QString outpath = outputDir + "/" + name;

        std::string sout = outpath.toStdString();
        FILE *out_fp = fopen(sout.c_str(), "wb");
        if (!out_fp) continue;

        E8Stream out_stream = make_stream(out_fp);
        int r = e8_archive_extract(ar, i, &out_stream, nullptr, nullptr);
        e8_stream_destroy(&out_stream);
        if (r == 0) extracted++;
    }

    e8_archive_close(ar);

    setStatus(QString("Extracted %1 file(s) to %2").arg(extracted).arg(outputDir), false);
}
