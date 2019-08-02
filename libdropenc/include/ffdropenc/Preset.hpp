#pragma once

#include <filesystem>

#include <QString>
#include <QStringList>

#include <nlohmann/json.hpp>

namespace ffdropenc
{

using json = nlohmann::json;

class Preset
{
public:
    using Pointer = std::shared_ptr<Preset>;

    explicit Preset(const QString& path);
    static Pointer New(const QString& path);
    size_t numberOfOutputs() { return cfg_["outputs"].size(); }
    [[nodiscard]] QString getListName() const;
    [[nodiscard]] QString getConsoleName() const;
    QStringList getSettings(size_t index);
    std::string getSuffix(size_t index);
    std::string getExtension(size_t index);

private:
    enum class ScaleMode { SquarePixel = 0, SizeLimited };

    // JSON rep of preset data
    json cfg_;

    static QStringList construct_filter_graph_(json filters);
};

}  // namespace ffdropenc