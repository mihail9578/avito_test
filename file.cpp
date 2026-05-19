#include "file.h"
#include <stdexcept>
#include <cstring>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

FileManager::FileManager(const char *path) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error(std::string("FileManager: open failed: ") + strerror(errno));
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        close(fd);
        throw std::runtime_error(std::string("FileManager: fstat failed: ") + strerror(errno));
    }

    size_ = sb.st_size;
    mapped_ = mmap(nullptr, size_, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (mapped_ == MAP_FAILED){
        throw std::runtime_error(std::string("FileManager: mmap failed: ") + strerror(errno));
    }
    data_ = std::string_view(static_cast<const char*>(mapped_), size_);
}

FileManager::~FileManager() noexcept {
    if (mapped_ != nullptr) {
        munmap(mapped_, size_);
    }
}