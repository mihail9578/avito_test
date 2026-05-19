#include "file.h"
#include <cstring>
#include <stdexcept>

#include <fcntl.h>
#include <fstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

void WriteToFile(const char *path,
                 const std::vector<std::pair<std::string_view, int>> &data) {
    std::ofstream out{std::string(path)};
    if (!out) {
        throw std::runtime_error("Cannot open file for writing");
    }

    for (const auto &[str, num] : data) {
        out << str << ' ' << num << '\n';
    }
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
        data_ = std::string_view(static_cast<const char *>(mapped_), size_);
    } else {
        mapped_ = nullptr;
        data_ = std::string_view();
    }
}

FileReader::~FileReader() noexcept {
    if (mapped_ != nullptr) {
        munmap(mapped_, size_);
    }
}