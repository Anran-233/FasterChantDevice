#ifndef DIALOGDISPLAY_H
#define DIALOGDISPLAY_H

#include "define.h"
#include <QDialog>

class QLabel;

namespace Ui {
class DialogDisplay;
}

class DialogDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDisplay(const int& id, const QString &strDirPath, QWidget *parent = nullptr);
    ~DialogDisplay();

    struct SStyle {
        QString fontFamily;     // 字体族类
        int fontSize = 16;      // 字体大小
        int fontBold = 0;       // 字体加粗
        QColor fontColor;       // 字体颜色
        int strokeSize = 0;     // 描边大小
        QColor strokeColor;     // 描边颜色
        int scaleLevel = 2;     // 缩放等级
    };

    const int& id() const;
    void updateId(const int& newid);
    void updateTitle();
    void updateTip();
    void updateAll();
    void lookupInit();
    void lookupList(const int& id, const QList<const SSchemeData*>& list);
    void lookupList(const QString& strSpell, const QList<const SSchemeData*>& list);
    void switchPage(int page);

private:
    Ui::DialogDisplay *ui;
    int m_id{ 0 };
    int m_width{ 400 };
    QLabel* m_pLabelList[10][2];
    QString m_strDirPath;

    SStyle readStyle(const QString& strName);
    virtual void paintEvent(QPaintEvent *) override;
};

#endif // DIALOGDISPLAY_H
