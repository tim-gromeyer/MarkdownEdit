#include "utils.h"

#include <QRunnable>
#include <QThreadPool>

class FunctionRunnable : public QRunnable
{
    std::function<void()> m_functionToRun;

public:
    explicit FunctionRunnable(std::function<void()> functionToRun)
        : m_functionToRun(std::move(functionToRun))
    {}
    void run() override { m_functionToRun(); }
};

namespace threading {
QRunnable *runFunction(std::function<void()> f)
{
    if (!f)
        return nullptr;

    QRunnable *runnable = new FunctionRunnable(std::move(f));
    runnable->setAutoDelete(false);
    QThreadPool::globalInstance()->start(runnable);
    return runnable;
}
} // namespace threading
