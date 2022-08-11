#ifndef DEFINE_H
#define DEFINE_H

#include <QString>
#include <QMap>
#include <QWidget>
#include <QDebug>

/// 是否仅限三百英雄时启用
extern bool g_bOnly300;
/// 是否在公共频道发言
extern bool g_bPublic;
/// 是否在左侧显示面板
extern int g_bLeft;
/// 启动面板按键的键值1
extern int g_keyValue1;
/// 启动面板按键的键值2
extern int g_keyValue2;
/// 是否屏蔽热键
extern bool g_bSeniorShield;
/// 是否自动切回首页
extern bool g_bSeniorBack;
/// 是否使用聊天模式
extern bool g_bSeniorMode;
/// 连发模式
extern int g_seniorContinuity;

/// 设置面板按键
void SetKeyValue(const int& key);
/// 获取面板按键
int GetKeyValue();
/// 获取面板按键列表
QSet<int> GetKeyList();

/// 获取错误信息
int GetErrorInfo();

/// 转换为拼音首字母拼写
QString ToChineseSpell(const QString& strText);
/// 取出css中的颜色值
QString ToCssColor(const QString& strCss);
/// 取出css中的颜色值
QColor ToCssQColor(const QString& strCss);
/// ID转QString
QString ToIdStr(const int& id);

/// 读取ini配置文件（字符串）
QString ReadProfileStr(const wchar_t* lpAppName, const wchar_t* lpKeyName, const wchar_t* lpDefault, const wchar_t* lpFileName);
/// 读取ini配置文件（整数）
int ReadProfileInt(const wchar_t* lpAppName, const wchar_t* lpKeyName, const int& nDefault, const wchar_t* lpFileName);
/// 写入ini配置文件
bool WriteProfileStr(const wchar_t* lpAppName, const wchar_t* lpKeyName, const QString& str, const wchar_t* lpFileName);

/// 显示主窗口
void ShowMain(WId wid);

/// 预设方案数据
struct SSchemeData {
    int id[2]   { 0, 0 };   // 方案编号
    int type    { 0 };      // 是否启用二级面板
    QString list[10][10];   // [0][0]方案名称; [0][1~9]一级面板1~9内容; [1~9][0~9]二级面板0~9内容
    QString spell;
    auto & operator[](const int& i) { return list[i]; }
    auto & operator[](const int& i) const { return list[i]; }
    void updateSpell() { spell = ToChineseSpell(list[0][0]); };
};

/// 数据锁
class CLock
{
public:
    CLock(bool* pState) { m_pState = pState; *m_pState = true; }
    ~CLock() { *m_pState = false; }
private:
    bool* m_pState{ NULL };
};

/// 预设方案管理类
class CScheme
{
public:
    /// 初始化,读取预设方案配置(strDirPath 储存数据目录)
    bool init(const QString& strDirPath);
    /// 更新 预设方案(id 需要更新的预设方案编号)
    bool update(const int& id) const;
    /// 更新 预设方案(scheme 需要更新的预设方案)
    bool update(const SSchemeData& scheme) const;
    /// 导入 预设方案(strFilePath 导入文件路径)
    int import(const QString& strFilePath);
    /// 导出 预设方案(strFilePath 导出文件路径, id 需要导出的预设方案编号)
    bool output(const QString& strFilePath, const int& id) const;
    /// 重命名 预设方案(oldid 旧方案编号, newid 新方案编号)
    bool rename(const int& oldid, const int& newid);
    /// 删除 预设方案(id 需要删除的预设方案编号)
    bool remove(const int& id);
    /// 新增 预设方案(id 需要新增的预设方案编号)
    SSchemeData& append(const int& id);

    /// 重载运算符[] 返回对应ID的预设方案
    SSchemeData& operator[](const int& id);
    /// 查询id编号的预设方案是否存在
    bool contains(const int& id) const;
    /// 返回一个未被使用的空编号，如果为0，表示编号已满
    int getBlankId() const;
    /// 返回最靠前的方案编号
    int frontId() const;
    /// 查找编号十位符合条件的预设方案，并返回查找到的列表
    QList<const SSchemeData*> find(const int& id_frist) const;
    /// 查找名称拼写符合条件的预设方案，并返回查找到的列表
    QList<const SSchemeData*> find(const QString& strSpell) const;
    /// 返回所有方案
    QMap<int, SSchemeData>& list();

private:
    QString m_strDirPath;
    QMap<int, SSchemeData> m_schemeList;

    bool readDataFile(const int &id, const QString& strFilePath);
};
/// 预设方案全局列表
extern CScheme g_scheme;

#endif // DEFINE_H
