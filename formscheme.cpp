#include "formscheme.h"
#include "ui_formscheme.h"

#include "define.h"

FormScheme::FormScheme(const int& id, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormScheme)
{
    ui->setupUi(this);

    // ui
    ui->frame_second->move(1,1);
    setGeometry(0, 0, 500, 300);

    // 方案编号
    m_id = id;
    m_pLineEditID = ui->lineEdit_id;
    m_pLineEditID->setText(ToIdStr(m_id));

    // 方案名称[0][0]
    m_pLineEditList[0][0] = ui->lineEdit_00;
    m_pLineEditList[0][0]->setText(g_scheme[m_id][0][0]);

    // 是否启用二级面板
    ui->checkBox->setChecked(g_scheme[m_id].type);

    // 一级面板[0][1~9]
    m_pLineEditList[0][1] = ui->lineEdit_01;
    m_pLineEditList[0][2] = ui->lineEdit_02;
    m_pLineEditList[0][3] = ui->lineEdit_03;
    m_pLineEditList[0][4] = ui->lineEdit_04;
    m_pLineEditList[0][5] = ui->lineEdit_05;
    m_pLineEditList[0][6] = ui->lineEdit_06;
    m_pLineEditList[0][7] = ui->lineEdit_07;
    m_pLineEditList[0][8] = ui->lineEdit_08;
    m_pLineEditList[0][9] = ui->lineEdit_09;
    for (int &&i{ 1 }; i <= 9; ++i) m_pLineEditList[0][i]->setText(g_scheme[m_id][0][i]), m_pLineEditList[0][i]->setCursorPosition(0);

    // 二级面板[1~9][0~9]
    for (int &&i{ 1 }; i <= 9; ++i)
    {
        QWidget* pWidget = ui->stackedWidget->widget(i);
        QLabel* pLabelTitle = new QLabel(QString::fromLocal8Bit("二级面板 - ") + QString::number(i), pWidget);
        pLabelTitle->setGeometry(180, 0, 80, 20);
        pLabelTitle->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        for (int &&j{ 0 }; j <= 9; ++j)
        {
            QLabel* pLabel = new QLabel(QString::number(j), pWidget);
            QLineEdit* pLineEdit = new QLineEdit(g_scheme[m_id][i][j],pWidget);
            pLabel->setGeometry(0, j * 26 + 20, 20, 20);
            pLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            pLineEdit->setGeometry(20, j * 26 + 20, 430, 20);
            pLineEdit->setMaxLength(60);
            m_pLineEditList[i][j] = pLineEdit;
            m_pLineEditList[i][j]->setCursorPosition(0);
        }
    }

    // 初始化
    {
        ui->pushButton_save->setEnabled(0);
        ui->pushButton_cancel->setEnabled(0);
        ui->frame_hand->setEnabled(1);
        ui->frame_frist->setEnabled(1);
        ui->frame_second->setVisible(0);

        // lineEdit ID
        m_pLineEditID->setStyleSheet("");
        connect(ui->lineEdit_id, &QLineEdit::editingFinished, this, [&]{
            if (m_bFinished) return;
            CLock lock(&m_bFinished);
            QLineEdit* pLineEdit = qobject_cast<QLineEdit*>(sender());
            const int& number = pLineEdit->text().toInt();
            if (!number || number == m_id) {
                pLineEdit->setText(ToIdStr(m_id));
                return;
            }
            pLineEdit->setText(ToIdStr(number));
            pLineEdit->setStyleSheet("background-color:#CFC;");
            ui->pushButton_save->setEnabled(1);
            ui->pushButton_cancel->setEnabled(1);
        });

        // 启用二级面板
        connect(ui->checkBox, &QCheckBox::stateChanged, this, [&]{
            ui->pushButton_save->setEnabled(1);
            ui->pushButton_cancel->setEnabled(1);
        });

        // lineEdit[0~9][0~9]
        for (int &&i{ 0 }; i <= 9; ++i)
        {
            for (int &&j{ 0 }; j <= 9; ++j)
            {
                m_pLineEditList[i][j]->setStyleSheet("");
                connect(m_pLineEditList[i][j], &QLineEdit::textChanged, this, [&]{
                    if (m_bFinished) return;
                    CLock lock(&m_bFinished);
                    (qobject_cast<QLineEdit*>(sender()))->setStyleSheet("background-color:#CFC;");
                    ui->pushButton_save->setEnabled(1);
                    ui->pushButton_cancel->setEnabled(1);
                });
            }
        }

        // 打开二级面板按钮
        connect(ui->pushButton_01, &QPushButton::clicked, this, [&]{ ui->frame_hand->setEnabled(0); ui->frame_frist->setEnabled(0); ui->stackedWidget->setCurrentIndex(1); ui->frame_second->setVisible(1); });
        connect(ui->pushButton_02, &QPushButton::clicked, this, [&]{ ui->frame_hand->setEnabled(0); ui->frame_frist->setEnabled(0); ui->stackedWidget->setCurrentIndex(2); ui->frame_second->setVisible(1); });
        connect(ui->pushButton_03, &QPushButton::clicked, this, [&]{ ui->frame_hand->setEnabled(0); ui->frame_frist->setEnabled(0); ui->stackedWidget->setCurrentIndex(3); ui->frame_second->setVisible(1); });
        connect(ui->pushButton_04, &QPushButton::clicked, this, [&]{ ui->frame_hand->setEnabled(0); ui->frame_frist->setEnabled(0); ui->stackedWidget->setCurrentIndex(4); ui->frame_second->setVisible(1); });
        connect(ui->pushButton_05, &QPushButton::clicked, this, [&]{ ui->frame_hand->setEnabled(0); ui->frame_frist->setEnabled(0); ui->stackedWidget->setCurrentIndex(5); ui->frame_second->setVisible(1); });
        connect(ui->pushButton_06, &QPushButton::clicked, this, [&]{ ui->frame_hand->setEnabled(0); ui->frame_frist->setEnabled(0); ui->stackedWidget->setCurrentIndex(6); ui->frame_second->setVisible(1); });
        connect(ui->pushButton_07, &QPushButton::clicked, this, [&]{ ui->frame_hand->setEnabled(0); ui->frame_frist->setEnabled(0); ui->stackedWidget->setCurrentIndex(7); ui->frame_second->setVisible(1); });
        connect(ui->pushButton_08, &QPushButton::clicked, this, [&]{ ui->frame_hand->setEnabled(0); ui->frame_frist->setEnabled(0); ui->stackedWidget->setCurrentIndex(8); ui->frame_second->setVisible(1); });
        connect(ui->pushButton_09, &QPushButton::clicked, this, [&]{ ui->frame_hand->setEnabled(0); ui->frame_frist->setEnabled(0); ui->stackedWidget->setCurrentIndex(9); ui->frame_second->setVisible(1); });

        // 关闭二级面板按钮
        connect(ui->pushButton_return1, &QPushButton::clicked, this, [&]{ ui->frame_hand->setEnabled(1); ui->frame_frist->setEnabled(1); ui->frame_second->setVisible(0); });
        connect(ui->pushButton_return2, &QPushButton::clicked, this, [&]{ ui->frame_hand->setEnabled(1); ui->frame_frist->setEnabled(1); ui->frame_second->setVisible(0); });

        // 保存与取消
        connect(ui->pushButton_save, &QPushButton::clicked, this, &FormScheme::save);
        connect(ui->pushButton_cancel, &QPushButton::clicked, this, &FormScheme::cancel);
    }
}

FormScheme::~FormScheme()
{
    delete ui;
}

// 返回当前方案编号
const int &FormScheme::id() const
{
    return m_id;
}

// 保存修改
void FormScheme::save()
{
    // id
    if (m_pLineEditID->styleSheet().size() > 1)
    {
        const int oldid = m_id;
        const int newid = m_pLineEditID->text().toInt();
        if (oldid != newid) {
            // 检查编号是否已存在
            if (g_scheme.contains(newid))
            {
                emit sendErorrID(newid);
                return;
            }
            m_id = newid;
            g_scheme.rename(oldid, newid);
            m_pLineEditID->setStyleSheet("");
            emit sendUpdateID(oldid, newid);
        }
        else m_pLineEditID->setStyleSheet("");
    }

    int&& modifyLevel{ 0 };
    // title
    if (m_pLineEditList[0][0]->styleSheet().size() > 1)
    {
        modifyLevel |= 0b001;
        g_scheme[m_id][0][0] = m_pLineEditList[0][0]->text();
        g_scheme[m_id].updateSpell();
        m_pLineEditList[0][0]->setStyleSheet("");
        m_pLineEditList[0][0]->setCursorPosition(0);
    }
    // checkBox
    if (ui->checkBox->isChecked() != bool(g_scheme[m_id].type))
    {
        modifyLevel |= 0b010;
        g_scheme[m_id].type = ui->checkBox->isChecked();
    }
    // string
    for (int &&i{ 0 }; i <= 9; ++i)
    {
        for (int &&j{ 0 }; j <= 9; ++j)
        {
            if (i == 0 && j == 0) continue;
            if (m_pLineEditList[i][j]->styleSheet().size() > 1)
            {
                modifyLevel |= 0b100;
                g_scheme[m_id][i][j] = m_pLineEditList[i][j]->text();
                m_pLineEditList[i][j]->setStyleSheet("");
                m_pLineEditList[i][j]->setCursorPosition(0);
            }
        }
    }

    // 发送信号
    if (modifyLevel)
    {
        g_scheme.update(m_id);
        if (modifyLevel & 0b100) sendUpdateAll();
        else
        {
            if (modifyLevel & 0b001) sendUpdateTitle();
            if (modifyLevel & 0b010) sendUpdateTip();
        }
    }

    // pushButton
    ui->pushButton_save->setEnabled(0);
    ui->pushButton_cancel->setEnabled(0);
    ui->checkBox->setFocus();
}

// 取消修改
void FormScheme::cancel()
{
    CLock lock(&m_bFinished);

    // id
    if (m_pLineEditID->styleSheet().size() > 1)
    {
        m_pLineEditID->setStyleSheet("");
        m_pLineEditID->setText(ToIdStr(m_id));
    }

    // checkBox
    ui->checkBox->setChecked(g_scheme[m_id].type);

    // string
    for (int &&i{ 0 }; i <= 9; ++i)
    {
        for (int &&j{ 0 }; j <= 9; ++j)
        {
            if (m_pLineEditList[i][j]->styleSheet().size() > 1)
            {
                m_pLineEditList[i][j]->setStyleSheet("");
                m_pLineEditList[i][j]->setText(g_scheme[m_id][i][j]);
                m_pLineEditList[i][j]->setCursorPosition(0);
            }
        }
    }

    // pushButton
    ui->pushButton_save->setEnabled(0);
    ui->pushButton_cancel->setEnabled(0);
    ui->checkBox->setFocus();
}
