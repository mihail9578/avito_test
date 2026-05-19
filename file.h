#include <string_view>
#include <vector>

void WriteToFile(const char* path, const std::vector<std::pair<std::string_view, int>>&data);

class FileManager {
public:
    explicit FileManager(const char *path);

    std::string_view GetData() {
        return data_;
    }

    ~FileManager();

    FileManager() = delete;
    FileManager(const FileManager &fm) = delete;
    FileManager &operator=(const FileManager &fm) = delete;
    FileManager(FileManager &&fm) = delete;
    FileManager &operator=(FileManager &&fm) = delete;
private:
    size_t size_;
    void* mapped_ = nullptr;
    std::string_view data_;
};