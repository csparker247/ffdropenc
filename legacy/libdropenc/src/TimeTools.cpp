#include "ffdropenc/TimeTools.hpp"

#include <chrono>
#include <exception>

#include <QDebug>
#include <QStringList>
#include <QRegularExpression>

using namespace ffdropenc;

float ffdropenc::DurationStringToSeconds(const QString& str)
{
    auto dur = str.split(QRegularExpression("(:|\\.)"));
    if (dur.size() != 4) {
        qDebug() << dur;
        throw std::runtime_error("Duration string missing element");
    }
    std::chrono::duration<float> s = std::chrono::hours(dur[0].toInt()) +
                                     std::chrono::minutes(dur[1].toInt()) +
                                     std::chrono::seconds(dur[2].toInt()) +
                                     std::chrono::milliseconds(dur[3].toInt());
    return s.count();
}