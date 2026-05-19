#include <string_view>
#include <vector>

void WriteToFile(const char *path,
                 const std::vector<std::pair<std::string_view, int>> &data);

class FileReader {
public:
    explicit FileReader(const char *path);

    std::string_view GetData() {
        return data_;
    }

    ~FileReader();

    FileReader() = delete;
    FileReader(const FileReader &fm) = delete;
    FileReader &operator=(const FileReader &fm) = delete;
    FileReader(FileReader &&fm) = delete;
    FileReader &operator=(FileReader &&fm) = delete;

private:
    size_t size_;
    void *mapped_ = nullptr;
    std::string_view data_;
};