#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

class FormScheme;
class DialogDisplay;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void server();
    void socket();
    void init();

private:
    Ui::MainWindow *ui;
    QString m_strDirPatch;              /// 应用程序所在目录
    std::wstring m_strConfig;           /// 配置文件路径
    int m_id{ 0 };                      /// 当前编辑方案
    DialogDisplay* m_pDisplay{ NULL };  /// 显示面板
    QMap<int, FormScheme*> m_pFormList; /// 方案面板列表
    bool m_bSwiachLock{ false };        /// 切换方案锁
    QPoint m_posMouse;                  /// 鼠标按下位置
    bool m_bMouse{ false };             /// 鼠标是否按下

    virtual bool eventFilter(QObject *obj, QEvent *event) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
    void readStyle();
    void writeStyle();
    void updateStyle(const int& val);
    void bindEventOfFromScheme(const int& id);
    void bindEventOfSystem();
    void bindEventOfHand();
    void bindEventOfStyle();
    void bindEventOfKey();
};
#endif // MAINWINDOW_H
