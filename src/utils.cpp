#include "utils.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QRunnable>
#include <QThreadPool>

#ifdef CONCURRENT
#include <QtConcurrent/QtConcurrentRun>
#else

class FunctionRunnable : public QRunnable
{
    std::function<void()> m_functionToRun;

public:
    explicit FunctionRunnable(std::function<void()> functionToRun)
        : m_functionToRun(std::move(functionToRun))
    {}
    void run() override { m_functionToRun(); }
};
#endif

namespace threading {
void runFunction(const std::function<void()> &f)
{
    if (!f)
        return;

#ifdef CONCURRENT
    std::ignore = QtConcurrent::run(f);
    return;
#else
    QRunnable *runnable = new FunctionRunnable(std::move(f));
    runnable->setAutoDelete(false);
    QThreadPool::globalInstance()->start(runnable);
#endif
}
} // namespace threading

void loadIcon(const QString &name, QAction *a)
{
#ifndef FLATPAK
    a->setIcon(QIcon::fromTheme(name,
                                QIcon(STR(":/icons/%1/%2.svg")
                                          .arg(settings::isDarkMode() ? QStringLiteral("dark")
                                                                      : QStringLiteral("light"),
                                               name))));
#else
    a->setIcon(QIcon::fromTheme(name));
#endif
}

void loadIcon(const QString &name, QMenu *m)
{
#ifndef FLATPAK
    m->setIcon(QIcon::fromTheme(name,
                                QIcon(STR(":/icons/%1/%2.svg")
                                          .arg(settings::isDarkMode() ? QStringLiteral("dark")
                                                                      : QStringLiteral("light"),
                                               name))));
#else
    m->setIcon(QIcon::fromTheme(name));
#endif
}
