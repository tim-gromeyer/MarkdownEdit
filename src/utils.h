#pragma once

#include <functional>

class QRunnable;

namespace threading {
QRunnable *runFunction(std::function<void()>);
}
