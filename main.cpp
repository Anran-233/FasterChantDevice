#include "mainwindow.h"

#include <QApplication>
#include <QLockFile>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    // 文件锁，防止程序重复启动
    QLockFile lockFile(QDir::tempPath() + "/FasterChantDevice_onlyone.lock");
    if (lockFile.tryLock(200))
    {
        w.server();
        w.init();
        w.show();
    }
    else w.socket();
    return a.exec();
}
