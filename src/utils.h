#pragma once

#include <functional>

class QRunnable;

namespace threading {
void runFunction(const std::function<void()> &);
} // namespace threading
