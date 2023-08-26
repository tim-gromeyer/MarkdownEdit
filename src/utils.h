#pragma once

#include "settings.h"

#include <QString>

#include <functional>

namespace threading {
void runFunction(const std::function<void()> &);
} // namespace threading

inline bool mightBeLink(const QString &text)
{
    static const QLatin1String supportedSchemes[]
        = {L1("http://"), L1("https://"),  L1("file://"),  L1("www."),   L1("ftp://"),
           L1("mailto:"), L1("tel:"),      L1("sms:"),     L1("smsto:"), L1("data:"),
           L1("irc://"),  L1("gopher://"), L1("spotify:"), L1("steam:"), L1("bitcoin:"),
           L1("magnet:"), L1("ed2k://"),   L1("news:"),    L1("ssh://"), L1("note://")};

    for (const QLatin1String &scheme : supportedSchemes) {
        if (text.startsWith(scheme)) {
            return true;
        }
    }

    return false;
}
