#ifndef KEY_H
#define KEY_H
#include "define.h"
#include <QObject>

/// 是否已安装键盘钩子
extern bool g_bHook;

/// 安装钩子
void SetHook();
/// 卸载钩子
void UnHook();

class CKey : public QObject {
    Q_OBJECT
signals:
    int getid();
    void sendRect(int x, int y, int w, int h);
    void show();
    void hide();
    void switchPage(int page);
    void switchId(int id);
    void lookupId(const int& id, const QList<const SSchemeData*>& list);
    void lookupName(const QString& strSpell, const QList<const SSchemeData*>& list);
    void clipboard(const QString& str);
    void setContinuity(int index);
public:
    const int& id();
    /// 初始化
    bool init();
    /// 设置监测器
    void setMonitor();
    /// 获取按键输入
    void get(int key);
    /// 发送消息
    void sendMessage(const QString& str);
private:
    /// 当前方案ID
    int m_id = 0;
    /// 当前显示页面：0 首页；1~9 二级面板; -1 搜索页面
    int m_page = 0;
    /// 当前输入
    QString m_input;
    /// 当前搜索列表
    QList<const SSchemeData*> m_list;
};
/// 用来发送信号的对象
extern CKey g_key;

#endif // KEY_H
