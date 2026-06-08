#include "file.h"
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <stdexcept>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static constexpr size_t BUF_SIZE = 1 << 20;
static constexpr size_t STR_SIZE = 32;

void WriteToFile(const char *path,
                 const std::vector<std::pair<std::string_view, int>> &data) {
    std::ofstream out{path};
    if (!out) {
        throw std::runtime_error("Cannot open file for writing");
    }

    std::vector<char> buf(BUF_SIZE);
    out.rdbuf()->pubsetbuf(buf.data(), buf.size());

    std::string output;

    output.reserve(data.size() * STR_SIZE);
    for (const auto &[str, num] : data) {
        output.append(std::to_string(num));
        output += ' ';
        output.append(str);
        output += '\n';
    }
    out.write(output.data(), output.size());
}

FileReader::FileReader(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error(std::string("FileReader: open failed: ") +
                                 strerror(errno));
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        close(fd);
        throw std::runtime_error(std::string("FileReader: fstat failed: ") +
                                 strerror(errno));
    }

    size_ = sb.st_size;
    if (size_ > 0) {
        mapped_ = mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);
        if (mapped_ == MAP_FAILED) {
            throw std::runtime_error(std::string("FileReader: mmap failed: ") +
                                     strerror(errno));
        }
        madvise(mapped_, size_, MADV_SEQUENTIAL);
    } else {
        close(fd);
        mapped_ = nullptr;
    }
}

FileReader::~FileReader() noexcept {
    if (mapped_ != nullptr) {
        munmap(mapped_, size_);
    }
}
