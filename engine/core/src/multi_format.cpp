/**
 * Multi-format archive dispatcher.
 * Routes operations to libarchive (ZIP, TAR, 7Z, GZ, BZ2, XZ, LZIP) or E8 engine.
 */
#include <entropy8/multi_format.h>
#include <entropy8/entropy8.h>
#include <entropy8/io.h>
#include <entropy8/codec.h>

#include <archive.h>
#include <archive_entry.h>

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>

#ifdef _WIN32
#  include <direct.h>
#  define MKDIR(p) _mkdir(p)
#else
#  include <sys/stat.h>
#  define MKDIR(p) mkdir(p, 0755)
#endif

/* ─── Helpers ─────────────────────────────────────────────────────────────── */

static const struct {
    enum E8Format fmt;
    const char   *ext;
    bool          encrypts;
} kFmtTable[] = {
    { E8_FMT_7Z,    ".7z",  true  },
    { E8_FMT_BZIP2, ".bz2", false },
    { E8_FMT_E8,    ".e8",  true  },
    { E8_FMT_GZIP,  ".gz",  false },
    { E8_FMT_LZIP,  ".lz",  false },
    { E8_FMT_TAR,   ".tar", false },
    { E8_FMT_XZ,    ".xz",  false },
    { E8_FMT_ZIP,   ".zip", true  },
};

static bool str_ends_with_i(const char *str, const char *suffix) {
    size_t slen = std::strlen(str);
    size_t xlen = std::strlen(suffix);
    if (xlen > slen) return false;
    for (size_t i = 0; i < xlen; i++) {
        char a = str[slen - xlen + i];
        char b = suffix[i];
        if (a >= 'A' && a <= 'Z') a += 32;
        if (b >= 'A' && b <= 'Z') b += 32;
        if (a != b) return false;
    }
    return true;
}

/* Get just the filename from a path */
static std::string basename_of(const char *path) {
    std::string s(path);
    auto pos = s.find_last_of("/\\");
    return (pos != std::string::npos) ? s.substr(pos + 1) : s;
}

/* Ensure directory exists (create recursively) */
static void ensure_dir(const std::string &path) {
    std::string cur;
    for (size_t i = 0; i < path.size(); i++) {
        if (path[i] == '/' || path[i] == '\\') {
            if (!cur.empty()) MKDIR(cur.c_str());
        }
        cur += path[i];
    }
    if (!cur.empty()) MKDIR(cur.c_str());
}

/* ─── E8 stream helpers (same as in backend.cpp) ─────────────────────────── */

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

/* ─── libarchive: create ──────────────────────────────────────────────────── */

static int la_create(const char *output_path, enum E8Format format,
                     const char **files, size_t file_count,
                     const char *password, int level) {
    struct archive *a = archive_write_new();
    if (!a) return -1;

    /* Set format + compression based on E8Format */
    switch (format) {
    case E8_FMT_ZIP:
        archive_write_set_format_zip(a);
        if (password && password[0]) {
            archive_write_set_options(a, "zip:encryption=aes256");
            archive_write_set_passphrase(a, password);
        }
        break;
    case E8_FMT_7Z:
        archive_write_set_format_7zip(a);
        if (password && password[0]) {
            archive_write_set_passphrase(a, password);
        }
        break;
    case E8_FMT_TAR:
        archive_write_set_format_pax_restricted(a);
        break;
    case E8_FMT_GZIP:
        archive_write_set_format_pax_restricted(a);
        archive_write_add_filter_gzip(a);
        break;
    case E8_FMT_BZIP2:
        archive_write_set_format_pax_restricted(a);
        archive_write_add_filter_bzip2(a);
        break;
    case E8_FMT_XZ:
        archive_write_set_format_pax_restricted(a);
        archive_write_add_filter_xz(a);
        break;
    case E8_FMT_LZIP:
        archive_write_set_format_pax_restricted(a);
        archive_write_add_filter_lzip(a);
        break;
    default:
        archive_write_free(a);
        return -1;
    }

    if (archive_write_open_filename(a, output_path) != ARCHIVE_OK) {
        archive_write_free(a);
        return -1;
    }

    int added = 0;
    for (size_t i = 0; i < file_count; i++) {
        FILE *fp = fopen(files[i], "rb");
        if (!fp) continue;

        /* Get file size */
        fseek(fp, 0, SEEK_END);
        int64_t fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        struct archive_entry *entry = archive_entry_new();
        std::string name = basename_of(files[i]);
        archive_entry_set_pathname(entry, name.c_str());
        archive_entry_set_size(entry, fsize);
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);

        if (archive_write_header(a, entry) == ARCHIVE_OK) {
            char buf[65536];
            size_t n;
            while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
                archive_write_data(a, buf, n);
            }
            added++;
        }

        archive_entry_free(entry);
        fclose(fp);
    }

    archive_write_close(a);
    archive_write_free(a);
    return added;
}

/* ─── libarchive: extract ─────────────────────────────────────────────────── */

static int la_extract(const char *archive_path, const char *output_dir,
                      const char *password) {
    struct archive *a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_filter_all(a);

    if (password && password[0]) {
        archive_read_add_passphrase(a, password);
    }

    if (archive_read_open_filename(a, archive_path, 65536) != ARCHIVE_OK) {
        archive_read_free(a);
        return -1;
    }

    int extracted = 0;
    struct archive_entry *entry;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char *name = archive_entry_pathname(entry);
        if (!name) { archive_read_data_skip(a); continue; }

        std::string outpath = std::string(output_dir) + "/" + basename_of(name);

        /* Ensure parent directory exists */
        auto slash = outpath.find_last_of("/\\");
        if (slash != std::string::npos) {
            ensure_dir(outpath.substr(0, slash));
        }

        FILE *fp = fopen(outpath.c_str(), "wb");
        if (!fp) { archive_read_data_skip(a); continue; }

        const void *buf;
        size_t size;
        la_int64_t offset;
        while (archive_read_data_block(a, &buf, &size, &offset) == ARCHIVE_OK) {
            fwrite(buf, 1, size, fp);
        }
        fclose(fp);
        extracted++;
    }

    archive_read_close(a);
    archive_read_free(a);
    return extracted;
}

/* ─── libarchive: list ────────────────────────────────────────────────────── */

static int la_list(const char *archive_path, const char *password,
                   char ***paths, uint64_t **sizes, size_t *count) {
    struct archive *a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_filter_all(a);

    if (password && password[0]) {
        archive_read_add_passphrase(a, password);
    }

    if (archive_read_open_filename(a, archive_path, 65536) != ARCHIVE_OK) {
        archive_read_free(a);
        return -1;
    }

    std::vector<std::string> names;
    std::vector<uint64_t> file_sizes;

    struct archive_entry *entry;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char *name = archive_entry_pathname(entry);
        names.push_back(name ? name : "");
        file_sizes.push_back(static_cast<uint64_t>(archive_entry_size(entry)));
        archive_read_data_skip(a);
    }

    archive_read_close(a);
    archive_read_free(a);

    *count = names.size();
    *paths = (char **)std::malloc(names.size() * sizeof(char *));
    if (sizes) *sizes = (uint64_t *)std::malloc(names.size() * sizeof(uint64_t));

    for (size_t i = 0; i < names.size(); i++) {
        (*paths)[i] = (char *)std::malloc(names[i].size() + 1);
        std::strcpy((*paths)[i], names[i].c_str());
        if (sizes && *sizes) (*sizes)[i] = file_sizes[i];
    }

    return 0;
}

/* ─── E8 native: create ──────────────────────────────────────────────────── */

static int e8_create(const char *output_path, const char **files, size_t file_count,
                     const char *password, int level) {
    e8_codecs_init();

    FILE *out_fp = fopen(output_path, "wb");
    if (!out_fp) return -1;

    E8Stream out_stream = make_stream(out_fp);
    E8Archive *ar = e8_archive_create(&out_stream, password);
    if (!ar) {
        e8_stream_destroy(&out_stream);
        return -1;
    }

    int added = 0;
    for (size_t i = 0; i < file_count; i++) {
        FILE *in_fp = fopen(files[i], "rb");
        if (!in_fp) continue;
        E8Stream in_stream = make_stream(in_fp);

        std::string name = basename_of(files[i]);
        int r = e8_archive_add(ar, name.c_str(), &in_stream, nullptr, nullptr);
        e8_stream_destroy(&in_stream);
        if (r == 0) added++;
    }

    e8_archive_close(ar);
    return added;
}

/* ─── E8 native: extract ─────────────────────────────────────────────────── */

static int e8_native_extract(const char *archive_path, const char *output_dir,
                             const char *password) {
    e8_codecs_init();

    FILE *fp = fopen(archive_path, "rb");
    if (!fp) return -1;

    E8Stream stream = make_stream(fp);
    E8Archive *ar = e8_archive_open(&stream, password);
    if (!ar) {
        e8_stream_destroy(&stream);
        return -1;
    }

    ensure_dir(output_dir);

    size_t n = e8_archive_count(ar);
    int extracted = 0;

    for (size_t i = 0; i < n; i++) {
        char pbuf[4096];
        uint64_t usize = 0;
        e8_archive_entry(ar, i, pbuf, sizeof(pbuf), &usize);

        std::string outpath = std::string(output_dir) + "/" + basename_of(pbuf);
        FILE *out_fp = fopen(outpath.c_str(), "wb");
        if (!out_fp) continue;

        E8Stream out_stream = make_stream(out_fp);
        int r = e8_archive_extract(ar, i, &out_stream, nullptr, nullptr);
        e8_stream_destroy(&out_stream);
        if (r == 0) extracted++;
    }

    e8_archive_close(ar);
    return extracted;
}

/* ─── E8 native: list ─────────────────────────────────────────────────────── */

static int e8_native_list(const char *archive_path, const char *password,
                          char ***paths, uint64_t **sizes, size_t *count) {
    e8_codecs_init();

    FILE *fp = fopen(archive_path, "rb");
    if (!fp) return -1;

    E8Stream stream = make_stream(fp);
    E8Archive *ar = e8_archive_open(&stream, password);
    if (!ar) {
        e8_stream_destroy(&stream);
        return -1;
    }

    size_t n = e8_archive_count(ar);
    *count = n;
    *paths = (char **)std::malloc(n * sizeof(char *));
    if (sizes) *sizes = (uint64_t *)std::malloc(n * sizeof(uint64_t));

    for (size_t i = 0; i < n; i++) {
        char pbuf[4096];
        uint64_t usize = 0;
        e8_archive_entry(ar, i, pbuf, sizeof(pbuf), &usize);
        (*paths)[i] = (char *)std::malloc(std::strlen(pbuf) + 1);
        std::strcpy((*paths)[i], pbuf);
        if (sizes && *sizes) (*sizes)[i] = usize;
    }

    e8_archive_close(ar);
    return 0;
}

/* ═════════════════════════════════════════════════════════════════════════════
 * Public API
 * ═════════════════════════════════════════════════════════════════════════════ */

extern "C" {

enum E8Format e8_mf_detect_format(const char *path) {
    if (!path) return E8_FMT_COUNT;
    for (int i = 0; i < E8_FMT_COUNT; i++) {
        if (str_ends_with_i(path, kFmtTable[i].ext)) {
            return kFmtTable[i].fmt;
        }
    }
    /* Check for .tar.gz, .tar.bz2, .tar.xz, .tar.lz */
    if (str_ends_with_i(path, ".tar.gz") || str_ends_with_i(path, ".tgz"))
        return E8_FMT_GZIP;
    if (str_ends_with_i(path, ".tar.bz2") || str_ends_with_i(path, ".tbz2"))
        return E8_FMT_BZIP2;
    if (str_ends_with_i(path, ".tar.xz") || str_ends_with_i(path, ".txz"))
        return E8_FMT_XZ;
    if (str_ends_with_i(path, ".tar.lz"))
        return E8_FMT_LZIP;
    return E8_FMT_COUNT;
}

int e8_mf_supports_encryption(enum E8Format format) {
    if (format < 0 || format >= E8_FMT_COUNT) return 0;
    return kFmtTable[format].encrypts ? 1 : 0;
}

const char *e8_mf_extension(enum E8Format format) {
    if (format < 0 || format >= E8_FMT_COUNT) return "";
    return kFmtTable[format].ext;
}

int e8_mf_create(const char *output_path, enum E8Format format,
                  const char **files, size_t file_count,
                  const char *password, int level,
                  E8MFProgressFn progress, void *progress_user) {
    if (!output_path || !files || file_count == 0) return -1;

    if (format == E8_FMT_E8) {
        return e8_create(output_path, files, file_count, password, level);
    }

    /* All other formats via libarchive */
    return la_create(output_path, format, files, file_count, password, level);
}

int e8_mf_extract(const char *archive_path, const char *output_dir,
                   const char *password,
                   E8MFProgressFn progress, void *progress_user) {
    if (!archive_path || !output_dir) return -1;

    enum E8Format fmt = e8_mf_detect_format(archive_path);

    if (fmt == E8_FMT_E8) {
        return e8_native_extract(archive_path, output_dir, password);
    }

    /* All other formats via libarchive */
    return la_extract(archive_path, output_dir, password);
}

int e8_mf_list(const char *archive_path, const char *password,
                char ***paths, uint64_t **sizes, size_t *count) {
    if (!archive_path || !paths || !count) return -1;

    enum E8Format fmt = e8_mf_detect_format(archive_path);

    if (fmt == E8_FMT_E8) {
        return e8_native_list(archive_path, password, paths, sizes, count);
    }

    return la_list(archive_path, password, paths, sizes, count);
}

void e8_mf_free_list(char **paths, uint64_t *sizes, size_t count) {
    if (paths) {
        for (size_t i = 0; i < count; i++) {
            std::free(paths[i]);
        }
        std::free(paths);
    }
    if (sizes) std::free(sizes);
}

} /* extern "C" */
