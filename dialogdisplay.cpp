#include "dialogdisplay.h"
#include "ui_dialogdisplay.h"

#include <QPainter>

DialogDisplay::DialogDisplay(const int& id, const QString &strDirPath, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDisplay)
{
    ui->setupUi(this);

    // 设置焦点穿透并置顶
    this->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    this->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);

    m_id = id;
    m_strDirPath = strDirPath;
    ui->stackedWidget->setCurrentIndex(0);

    m_pLabelList[0][0] = ui->label_title_0;
    m_pLabelList[1][0] = ui->label_title_1;
    m_pLabelList[2][0] = ui->label_title_2;
    m_pLabelList[3][0] = ui->label_title_3;
    m_pLabelList[4][0] = ui->label_title_4;
    m_pLabelList[5][0] = ui->label_title_5;
    m_pLabelList[6][0] = ui->label_title_6;
    m_pLabelList[7][0] = ui->label_title_7;
    m_pLabelList[8][0] = ui->label_title_8;
    m_pLabelList[9][0] = ui->label_title_9;

    m_pLabelList[0][1] = ui->label_list_0;
    m_pLabelList[1][1] = ui->label_list_1;
    m_pLabelList[2][1] = ui->label_list_2;
    m_pLabelList[3][1] = ui->label_list_3;
    m_pLabelList[4][1] = ui->label_list_4;
    m_pLabelList[5][1] = ui->label_list_5;
    m_pLabelList[6][1] = ui->label_list_6;
    m_pLabelList[7][1] = ui->label_list_7;
    m_pLabelList[8][1] = ui->label_list_8;
    m_pLabelList[9][1] = ui->label_list_9;
}

DialogDisplay::~DialogDisplay()
{
    delete ui;
}

const int &DialogDisplay::id() const
{
    return m_id;
}

void DialogDisplay::updateId(const int &newid)
{
    m_id = newid;
    updateTitle();
}

void DialogDisplay::updateTitle()
{
    if (!m_id) return;
    SStyle&& style = readStyle(QString::fromLocal8Bit("标题"));
    const QString& str = "#" + ToIdStr(m_id) + " - " + g_scheme[m_id][0][0];
    const int& size = 24;
    const int& width = m_width - 18;
    QLabel* qLabel = ui->label_title_0;
    if(style.strokeSize)
    {
        QFont font;
        font.setFamily(style.fontFamily);
        font.setPixelSize(size);
        font.setBold(style.fontBold);
        QFontMetrics metrics(font);
        const int& fontHeight = metrics.height() + metrics.leading();
        const int& fontAscent = metrics.ascent() + metrics.leading();
        QImage img(width * style.scaleLevel, (fontHeight + style.strokeSize * 2) * style.scaleLevel, QImage::Format_ARGB32);
        img.fill(QColor(0,0,0,0));
        font.setPixelSize(font.pixelSize() * style.scaleLevel);
        QPainterPath path;
        path.addText(style.strokeSize * style.scaleLevel, (fontAscent + style.strokeSize) * style.scaleLevel, font, str);
        QPainter(&img).strokePath(path, QPen(style.strokeColor, style.strokeSize * style.scaleLevel * 2));
        QPainter(&img).fillPath(path, style.fontColor);
        qLabel->clear();
        qLabel->setPixmap(QPixmap::fromImage(img.scaled(img.size() / style.scaleLevel, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    }
    else
    {
        qLabel->clear();
        qLabel->setText(str);
        qLabel->setStyleSheet("font:" + QString::number(size) + "px \"" + style.fontFamily
                              + "\";color:rgb(" + QString::number(style.fontColor.red())
                              + "," + QString::number(style.fontColor.green())
                              + "," + QString::number(style.fontColor.blue())
                              + (style.fontBold ? ");font-weight:bold;" : ");"));
    }
}

void DialogDisplay::updateTip()
{
    if (!m_id) return;
    SStyle&& style = readStyle(QString::fromLocal8Bit("标题"));
    const QString& str = QString::fromLocal8Bit(g_scheme[m_id].type ? "(按0键切换方案) (二级面板已启用)" : "(按0键切换方案) (二级面板已禁用)");
    const int& size = 13;
    const int& width = m_width - 18;
    QLabel* qLabel = ui->label_tip_0;
    if(style.strokeSize)
    {
        QFont font;
        font.setFamily(style.fontFamily);
        font.setPixelSize(size);
        font.setBold(style.fontBold);
        QFontMetrics metrics(font);
        const int& fontHeight = metrics.height() + metrics.leading();
        const int& fontAscent = metrics.ascent() + metrics.leading();
        QImage img(width * style.scaleLevel, (fontHeight + style.strokeSize * 2) * style.scaleLevel, QImage::Format_ARGB32);
        img.fill(QColor(0,0,0,0));
        font.setPixelSize(font.pixelSize() * style.scaleLevel);
        QPainterPath path;
        path.addText(style.strokeSize * style.scaleLevel, (fontAscent + style.strokeSize) * style.scaleLevel, font, str);
        QPainter(&img).strokePath(path, QPen(style.strokeColor, style.strokeSize * style.scaleLevel * 2));
        QPainter(&img).fillPath(path, style.fontColor);
        qLabel->clear();
        qLabel->setPixmap(QPixmap::fromImage(img.scaled(img.size() / style.scaleLevel, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    }
    else
    {
        qLabel->clear();
        qLabel->setText(str);
        qLabel->setStyleSheet("font:" + QString::number(size) + "px \"" + style.fontFamily
                              + "\";color:rgb(" + QString::number(style.fontColor.red())
                              + "," + QString::number(style.fontColor.green())
                              + "," + QString::number(style.fontColor.blue())
                              + (style.fontBold ? ");font-weight:bold;" : ");"));
    }
}

void DialogDisplay::updateAll()
{
    if (!m_id) return;

    // 背景
    ui->background->setStyleSheet("#background{background-color:rgba(" + ReadProfileStr(L"面板", L"背景颜色", L"0,0,0,255", (m_strDirPath + "/FCDConfig/config.ini").toStdWString().c_str()) + ");}");

    m_width = ReadProfileInt(L"面板", L"面板宽度", 400, (m_strDirPath + "/FCDConfig/config.ini").toStdWString().c_str());
    this->setFixedWidth(m_width);
    const int& width = m_width - 18;
    // 标题
    {
        // 一级面板
        updateTitle();
        updateTip();

        // 二级面板
        SStyle&& style = readStyle(QString::fromLocal8Bit("标题"));
        QFont font;
        font.setFamily(style.fontFamily);
        font.setPixelSize(24);
        font.setBold(style.fontBold);
        QFontMetrics metrics(font);
        const int& fontHeight = metrics.height() + metrics.leading();
        const int& fontAscent = metrics.ascent() + metrics.leading();
        for (int &&i{ 1 }; i <= 9; ++i)
        {
            const QString& str = "#" + QString::number(i) + " - " + g_scheme[m_id][0][i];
            QLabel* qLabel = m_pLabelList[i][0];
            if(style.strokeSize)
            {
                QImage img(width * style.scaleLevel, (fontHeight + style.strokeSize * 2) * style.scaleLevel, QImage::Format_ARGB32);
                img.fill(QColor(0,0,0,0));
                font.setPixelSize(font.pixelSize() * style.scaleLevel);
                QPainterPath path;
                path.addText(style.strokeSize * style.scaleLevel, (fontAscent + style.strokeSize) * style.scaleLevel, font, str);
                font.setPixelSize(font.pixelSize() / style.scaleLevel);
                QPainter(&img).strokePath(path, QPen(style.strokeColor, style.strokeSize * style.scaleLevel * 2));
                QPainter(&img).fillPath(path, style.fontColor);
                qLabel->clear();
                qLabel->setPixmap(QPixmap::fromImage(img.scaled(img.size() / style.scaleLevel, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
            }
            else
            {
                qLabel->clear();
                qLabel->setText(str);
                qLabel->setStyleSheet("font:24px \"" + style.fontFamily
                                      + "\";color:rgb(" + QString::number(style.fontColor.red())
                                      + "," + QString::number(style.fontColor.green())
                                      + "," + QString::number(style.fontColor.blue())
                                      + (style.fontBold ? ");font-weight:bold;" : ");"));
            }
        }
    }
    // 内容
    {
        SStyle&& style = readStyle(QString::fromLocal8Bit("内容"));
        QFont font;
        font.setFamily(style.fontFamily);
        font.setPixelSize(style.fontSize);
        font.setBold(style.fontBold);
        QFontMetrics metrics(font);
        const int& fontHeight = metrics.height() + metrics.leading();
        const int& fontAscent = metrics.ascent() + metrics.leading();
        const int& fontInterval = fontHeight / 2;
        for (int &&i{ 0 }; i <= 9; ++i)
        {
            int&& nSize{ i ? 10 : 9 };
            if (i) { for (; nSize >= 1; --nSize) if (g_scheme[m_id][i][nSize - 1].size() > 0) break; }
            else { for (; nSize >= 1; --nSize) if (g_scheme[m_id][i][nSize].size() > 0) break; }
            QImage img(width * style.scaleLevel, ((fontHeight + style.strokeSize * 2) * (i ? 10 : 9) + fontInterval * (i ? 9 : 8)) * style.scaleLevel, QImage::Format_ARGB32);
            img.fill(QColor(0,0,0,0));
            font.setPixelSize(font.pixelSize() * style.scaleLevel);
            QPainterPath path;
            if (i) for (int &&j{ 0 }; j < nSize; j++)
                path.addText(style.strokeSize * style.scaleLevel,
                             ((fontHeight + style.strokeSize * 2 + fontInterval) * j + fontAscent + style.strokeSize) * style.scaleLevel,
                             font, QString::number(j) + ". " + g_scheme[m_id][i][j]);
            else for (int &&j{ 1 }; j <= nSize; ++j)
                path.addText(style.strokeSize * style.scaleLevel,
                             ((fontHeight + style.strokeSize * 2 + fontInterval) * (j - 1) + fontAscent + style.strokeSize) * style.scaleLevel,
                             font, QString::number(j) + ". " + g_scheme[m_id][i][j]);
            font.setPixelSize(font.pixelSize() / style.scaleLevel);
            QPainter(&img).strokePath(path, QPen(style.strokeColor, style.strokeSize * style.scaleLevel * 2));
            QPainter(&img).fillPath(path, style.fontColor);
            m_pLabelList[i][1]->clear();
            m_pLabelList[i][1]->setPixmap(QPixmap::fromImage(img.scaled(img.size() / style.scaleLevel, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
        }
    }
}

// 切换方案面板初始化
void DialogDisplay::lookupInit()
{   
    const int& width = m_width - 18;
    // 标题
    {
        SStyle&& style = readStyle(QString::fromLocal8Bit("标题"));
        const QString& strTitle = QString::fromLocal8Bit("#00 - 切换预设方案");
        const QString& strTipId = QString::fromLocal8Bit("#1 输入数字键按方案编号搜索");
        const QString& strTipSpell = QString::fromLocal8Bit("#2 输入字母键按名称拼写搜索");
        const QString& strEdit(" ");
        if(style.strokeSize)
        {
            QFont font;
            font.setFamily(style.fontFamily);
            font.setBold(style.fontBold);
            auto lam_line = [&width, &style, &font](QLabel* pLabel, const int& size, const QString& str) {
                font.setPixelSize(size);
                QFontMetrics metrics(font);
                const int& fontHeight = metrics.height() + metrics.leading();
                const int& fontAscent = metrics.ascent() + metrics.leading();
                QImage img(width * style.scaleLevel, (fontHeight + style.strokeSize * 2) * style.scaleLevel, QImage::Format_ARGB32);
                img.fill(QColor(0,0,0,0));
                font.setPixelSize(font.pixelSize() * style.scaleLevel);
                QPainterPath path;
                path.addText(style.strokeSize * style.scaleLevel, (fontAscent + style.strokeSize) * style.scaleLevel, font, str);
                font.setPixelSize(font.pixelSize() / style.scaleLevel);
                QPainter(&img).strokePath(path, QPen(style.strokeColor, style.strokeSize * style.scaleLevel * 2));
                QPainter(&img).fillPath(path, style.fontColor);
                pLabel->clear();
                pLabel->setPixmap(QPixmap::fromImage(img.scaled(img.size() / style.scaleLevel, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
            };

            lam_line(ui->label_lookup_title, 24, strTitle);
            lam_line(ui->label_lookup_tip_id, 20, strTipId);
            lam_line(ui->label_lookup_tip_spell, 20, strTipSpell);
            lam_line(ui->label_lookup_edit, 40, strEdit);
        }
        else
        {
            auto lam_css = [&style](QLabel* pLabel, const int& size, const QString& str) {
                pLabel->clear();
                pLabel->setText(str);
                pLabel->setStyleSheet("font:" + QString::number(size) + "px \"" + style.fontFamily
                                      + "\";color:rgb(" + QString::number(style.fontColor.red())
                                      + "," + QString::number(style.fontColor.green())
                                      + "," + QString::number(style.fontColor.blue())
                                      + (style.fontBold ? ");font-weight:bold;" : ");"));
            };
            lam_css(ui->label_lookup_title, 24, strTitle);
            lam_css(ui->label_lookup_tip_id, 20, strTipId);
            lam_css(ui->label_lookup_tip_spell, 20, strTipSpell);
            lam_css(ui->label_lookup_edit, 40, strEdit);
        }
    }
    // 列表
    {
        SStyle&& style = readStyle(QString::fromLocal8Bit("内容"));
        QFont font;
        font.setFamily(style.fontFamily);
        font.setPixelSize(style.fontSize);
        font.setBold(style.fontBold);
        const int& fontHeight = QFontMetrics(font).height();
        const int& fontInterval = fontHeight / 2;
        QImage img(width, (fontHeight + style.strokeSize * 2) * 10 + fontInterval * 9, QImage::Format_ARGB32);
        img.fill(QColor(0,0,0,0));
        ui->label_lookup_list->setPixmap(QPixmap::fromImage(std::move(img)));
    }

    // UI
    ui->label_lookup_title->setVisible(1);
    ui->label_lookup_tip_id->setVisible(1);
    ui->label_lookup_tip_spell->setVisible(1);
    ui->label_lookup_edit->setVisible(0);
    ui->label_lookup_list->setVisible(1);
    ui->frame_interval_lookup->setMinimumHeight(10 + ui->label_lookup_edit->height() - ui->label_lookup_tip_spell->height());
}

// 切换方案面板 按编号搜索方案
void DialogDisplay::lookupList(const int &id_frist, const QList<const SSchemeData *> &list)
{
    // UI
    ui->label_lookup_tip_spell->setVisible(0);
    ui->label_lookup_edit->setVisible(1);
    ui->frame_interval_lookup->setMinimumHeight(10);

    // style
    const int& width = m_width - 18;
    SStyle&& style = readStyle(QString::fromLocal8Bit("内容"));
    QFont font;
    font.setFamily(style.fontFamily);
    font.setBold(style.fontBold);
    // edit
    {
        const QString& str = QString::number(id_frist);
        if(style.strokeSize)
        {
            font.setPixelSize(40);
            QFontMetrics metrics(font);
            const int& fontHeight = metrics.height() + metrics.leading();
            const int& fontAscent = metrics.ascent() + metrics.leading();
            QImage img(width * style.scaleLevel, (fontHeight + style.strokeSize * 2) * style.scaleLevel, QImage::Format_ARGB32);
            img.fill(QColor(0,0,0,0));
            font.setPixelSize(font.pixelSize() * style.scaleLevel);
            QPainterPath path;
            path.addText(style.strokeSize * style.scaleLevel, (fontAscent + style.strokeSize) * style.scaleLevel, font, str);
            font.setPixelSize(font.pixelSize() / style.scaleLevel);
            QPainter(&img).strokePath(path, QPen(style.strokeColor, style.strokeSize * style.scaleLevel * 2));
            QPainter(&img).fillPath(path, style.fontColor);
            ui->label_lookup_edit->clear();
            ui->label_lookup_edit->setPixmap(QPixmap::fromImage(img.scaled(img.size() / style.scaleLevel, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
        }
        else
        {
            ui->label_lookup_edit->clear();
            ui->label_lookup_edit->setText(str);
            ui->label_lookup_edit->setStyleSheet("font:40px \"" + style.fontFamily
                                  + "\";color:rgb(" + QString::number(style.fontColor.red())
                                  + "," + QString::number(style.fontColor.green())
                                  + "," + QString::number(style.fontColor.blue())
                                  + (style.fontBold ? ");font-weight:bold;" : ");"));
        }
    }
    // list
    {
        font.setPixelSize(style.fontSize);
        QFontMetrics metrics(font);
        const int& fontHeight = metrics.height() + metrics.leading();
        const int& fontAscent = metrics.ascent() + metrics.leading();
        const int& fontInterval = fontHeight / 2;
        int&& nSize{ list.size() };
        QImage img(width * style.scaleLevel, ((fontHeight + style.strokeSize * 2) * 10 + fontInterval * 9) * style.scaleLevel, QImage::Format_ARGB32);
        img.fill(QColor(0,0,0,0));
        font.setPixelSize(font.pixelSize() * style.scaleLevel);
        QPainterPath path;
        for (int &&i{ 0 }; i < nSize; i++)
            path.addText(style.strokeSize * style.scaleLevel,
                         ((fontHeight + style.strokeSize * 2 + fontInterval) * i + fontAscent + style.strokeSize) * style.scaleLevel,
                         font, QString::number(list[i]->id[0]) + QString::number(list[i]->id[1]) + ". " + list[i]->list[0][0]);
        font.setPixelSize(font.pixelSize() / style.scaleLevel);
        QPainter(&img).strokePath(path, QPen(style.strokeColor, style.strokeSize * style.scaleLevel * 2));
        QPainter(&img).fillPath(path, style.fontColor);
        ui->label_lookup_list->clear();
        ui->label_lookup_list->setPixmap(QPixmap::fromImage(img.scaled(img.size() / style.scaleLevel, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    }
}

// 切换方案面板 按拼写搜索方案
void DialogDisplay::lookupList(const QString &strSpell, const QList<const SSchemeData *> &list)
{
    // UI
    ui->label_lookup_tip_id->setVisible(0);
    ui->label_lookup_edit->setVisible(1);
    ui->frame_interval_lookup->setMinimumHeight(10);

    // style
    const int& width = m_width - 18;
    SStyle&& style = readStyle(QString::fromLocal8Bit("内容"));
    QFont font;
    font.setFamily(style.fontFamily);
    font.setBold(style.fontBold);
    // edit
    {
        const QString& str = strSpell;
        if(style.strokeSize)
        {
            font.setPixelSize(40);
            QFontMetrics metrics(font);
            const int& fontHeight = metrics.height() + metrics.leading();
            const int& fontAscent = metrics.ascent() + metrics.leading();
            QImage img(width * style.scaleLevel, (fontHeight + style.strokeSize * 2) * style.scaleLevel, QImage::Format_ARGB32);
            img.fill(QColor(0,0,0,0));
            font.setPixelSize(font.pixelSize() * style.scaleLevel);
            QPainterPath path;
            path.addText(style.strokeSize * style.scaleLevel, (fontAscent + style.strokeSize) * style.scaleLevel, font, str);
            font.setPixelSize(font.pixelSize() / style.scaleLevel);
            QPainter(&img).strokePath(path, QPen(style.strokeColor, style.strokeSize * style.scaleLevel * 2));
            QPainter(&img).fillPath(path, style.fontColor);
            ui->label_lookup_edit->clear();
            ui->label_lookup_edit->setPixmap(QPixmap::fromImage(img.scaled(img.size() / style.scaleLevel, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
        }
        else
        {
            ui->label_lookup_edit->clear();
            ui->label_lookup_edit->setText(str);
            ui->label_lookup_edit->setStyleSheet("font:40px \"" + style.fontFamily
                                  + "\";color:rgb(" + QString::number(style.fontColor.red())
                                  + "," + QString::number(style.fontColor.green())
                                  + "," + QString::number(style.fontColor.blue())
                                  + (style.fontBold ? ");font-weight:bold;" : ");"));
        }
    }
    // list
    {
        font.setPixelSize(style.fontSize);
        QFontMetrics metrics(font);
        const int& fontHeight = metrics.height() + metrics.leading();
        const int& fontAscent = metrics.ascent() + metrics.leading();
        const int& fontInterval = fontHeight / 2;
        int&& nSize{ list.size() };
        QImage img(width * style.scaleLevel, ((fontHeight + style.strokeSize * 2) * 10 + fontInterval * 9) * style.scaleLevel, QImage::Format_ARGB32);
        img.fill(QColor(0,0,0,0));
        font.setPixelSize(font.pixelSize() * style.scaleLevel);
        QPainterPath path;
        for (int &&i{ 0 }; i < nSize; i++)
            path.addText(style.strokeSize * style.scaleLevel,
                         ((fontHeight + style.strokeSize * 2 + fontInterval) * i + fontAscent + style.strokeSize) * style.scaleLevel,
                         font, QString::number(i) + ". " + list[i]->list[0][0]);
        font.setPixelSize(font.pixelSize() / style.scaleLevel);
        QPainter(&img).strokePath(path, QPen(style.strokeColor, style.strokeSize * style.scaleLevel * 2));
        QPainter(&img).fillPath(path, style.fontColor);
        ui->label_lookup_list->clear();
        ui->label_lookup_list->setPixmap(QPixmap::fromImage(img.scaled(img.size() / style.scaleLevel, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
    }
}

// 切换页面
void DialogDisplay::switchPage(int page)
{
    ui->stackedWidget->setCurrentIndex(page);
}

DialogDisplay::SStyle DialogDisplay::readStyle(const QString &strName)
{
    const std::wstring& wstrDirPath = (m_strDirPath + "/FCDConfig/config.ini").toStdWString();
    const std::wstring& strAppName = strName.toStdWString();
    SStyle style;
    style.fontFamily = ReadProfileStr(L"面板", L"字体族类", L"微软雅黑", wstrDirPath.c_str());
    style.scaleLevel = ReadProfileInt(L"面板", L"描边质量", 2, wstrDirPath.c_str());
    style.fontSize = ReadProfileInt(strAppName.c_str(), L"字体大小", 16, wstrDirPath.c_str());
    style.fontBold = ReadProfileInt(strAppName.c_str(), L"字体加粗", 0, wstrDirPath.c_str());
    style.strokeSize = ReadProfileInt(strAppName.c_str(), L"描边大小", 0, wstrDirPath.c_str());
    QStringList&& fontList = ReadProfileStr(strAppName.c_str(), L"字体颜色", L"0,0,0", wstrDirPath.c_str()).split(',');
    QStringList&& strokeList = ReadProfileStr(strAppName.c_str(), L"描边颜色", L"0,0,0,155", wstrDirPath.c_str()).split(',');
    style.fontColor = (fontList.size() < 3) ? QColor(0, 0, 0) : QColor(fontList[0].toInt(), fontList[1].toInt(), fontList[2].toInt());
    style.strokeColor = (strokeList.size() < 4) ? QColor(0, 0, 0, 155) : QColor(strokeList[0].toInt(), strokeList[1].toInt(), strokeList[2].toInt(), strokeList[3].toInt());
    return style;
}

void DialogDisplay::paintEvent(QPaintEvent *)
{
    QPainter(this).fillRect(rect(), QColor(0, 0, 0, 0));
}
