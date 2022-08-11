#ifndef FORMSCHEME_H
#define FORMSCHEME_H

#include <QWidget>

class QLineEdit;

namespace Ui {
class FormScheme;
}

class FormScheme : public QWidget
{
    Q_OBJECT

public:
    explicit FormScheme(const int& id, QWidget *parent = nullptr);
    ~FormScheme();

    const int& id() const;

private:
    Ui::FormScheme *ui;
    int m_id;
    QLineEdit* m_pLineEditID;
    QLineEdit* m_pLineEditList[10][10];
    bool m_bFinished{ false };

    void save();
    void cancel();

signals:
    void sendErorrID(int newid);
    void sendUpdateID(int oldid, int newid);
    void sendUpdateTitle();
    void sendUpdateTip();
    void sendUpdateAll();
};

#endif // FORMSCHEME_H
