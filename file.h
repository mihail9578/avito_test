#include <string>
#include <vector>

void WriteToFile(const char *path,
                 const std::vector<std::pair<std::string_view, int>> &data);

class FileReader {
public:
    explicit FileReader(const char *path);

    std::string GetData() {
        return size_ > 0
                   ? std::string(static_cast<const char *>(mapped_), size_)
                   : std::string();
    }

    ~FileReader();

    FileReader() = delete;
    FileReader(const FileReader &fr) = delete;
    FileReader &operator=(const FileReader &fr) = delete;
    FileReader(FileReader &&fr) = delete;
    FileReader &operator=(FileReader &&fr) = delete;

private:
    size_t size_;
    void *mapped_ = nullptr;
};
