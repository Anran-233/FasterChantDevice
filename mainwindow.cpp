#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "define.h"
#include "key.h"
#include "formscheme.h"
#include "dialogdisplay.h"
#include <QColorDialog>
#include <QClipboard>
#include <QComboBox>
#include <QDir>
#include <QFileDialog>
#include <QLocalServer>
#include <QLocalSocket>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QSystemTrayIcon>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags (Qt::FramelessWindowHint);
}

MainWindow::~MainWindow()
{
    if (m_pDisplay) delete m_pDisplay;
    if (g_bHook) UnHook();
    delete ui;
}

// 创建服务端 监听重复启动的程序
void MainWindow::server()
{
    QLocalServer* pServer = new QLocalServer(this);
    connect(pServer, &QLocalServer::newConnection, this, [this]{
        QLocalServer* pServer = qobject_cast<QLocalServer*>(sender());
        if (pServer->hasPendingConnections()) {
            QLocalSocket* pSocket = pServer->nextPendingConnection();
            connect(pSocket, &QLocalSocket::readyRead, this, [this]{
                QLocalSocket* pSocket = qobject_cast<QLocalSocket*>(sender());
                if (pSocket->readAll() == "show")
                {
                    ShowMain(winId());
                    pSocket->write("quit");
                    if (pSocket->waitForBytesWritten(500)) pSocket->disconnected();
                }
            });
        }
    });
    QLocalServer::removeServer("FasterChantDevice_sever");
    pServer->listen("FasterChantDevice_sever");

}

// 创建客户端 通知服务端显示后退出程序
void MainWindow::socket()
{
    QLocalSocket* pSocket = new QLocalSocket(this);
    pSocket->connectToServer("FasterChantDevice_sever");
    if (pSocket->waitForConnected(1000))
    {
        connect(pSocket, &QLocalSocket::readyRead, qApp, &QApplication::quit);
        pSocket->write("show");
    }
    else qApp->quit();
}

// 事件过滤器
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    const auto& type = event->type();
    if (type == QEvent::MouseButtonPress) {
        if (obj == ui->hand_logo) {
            QMouseEvent *e = static_cast<QMouseEvent*>(event);
            if (e->button() == Qt::LeftButton) {
                m_bMouse = true;
                m_posMouse = e->globalPos() - this->frameGeometry().topLeft();
            }
        }
    }
    else if (type == QEvent::MouseButtonRelease) {
        QMouseEvent *e = static_cast<QMouseEvent*>(event);
        if (e->button() == Qt::LeftButton)  m_bMouse = false;
        else if (e->button() == Qt::RightButton) {
            // 创建右键菜单
            QMenu menu(this);
            menu.setStyleSheet("QMenu::item {padding:3px 7px;} QMenu::item:selected {background-color: #bbb;}");
            QAction menuButton(QString::fromWCharArray(L"获取开源代码"), this);
            connect(&menuButton, &QAction::triggered, this, [&] {
                if (QFile::exists("FasterChantDevice.zip"))
                {
                    if (!QFile::remove("FasterChantDevice.zip"))
                    {
                        QFile::setPermissions("FasterChantDevice.zip", QFileDevice::ReadOther | QFileDevice::WriteOther);
                        QFile::remove("FasterChantDevice.zip");
                    }
                }
                if (QFile::copy(":/res/FasterChantDevice.zip", "FasterChantDevice.zip"))
                    QMessageBox::about(this, QString::fromWCharArray(L"提示"), QString::fromWCharArray(L"开源代码 (FasterChantDevice.zip)\n文件已放置在应用程序目录中。"));
                else
                    QMessageBox::warning(this, QString::fromWCharArray(L"警告"), QString::fromWCharArray(L"复制文件失败 (FasterChantDevice.zip)！\n请手动删除应用程序目录下同名文件！"));
            });
            menu.addAction(&menuButton);
            menu.exec(QCursor::pos());
        }
    }
    else if (type == QEvent::MouseMove) {
        if (m_bMouse) this->move(static_cast<QMouseEvent*>(event)->globalPos() - m_posMouse);
    }
    return QMainWindow::eventFilter(obj, event);
}

// 窗口失去焦点事件
void MainWindow::focusOutEvent(QFocusEvent *)
{
    m_bMouse = false;
}

// 初始化
void MainWindow::init()
{
    // 设置系统托盘
    QSystemTrayIcon* pSysTrayIcon = new QSystemTrayIcon(QIcon(":/res/exe.ico"), this);
    pSysTrayIcon->setToolTip(QString::fromLocal8Bit("300高速咏唱装置"));
    connect(pSysTrayIcon, &QSystemTrayIcon::activated, this, [&](QSystemTrayIcon::ActivationReason reason) { if (reason == QSystemTrayIcon::Trigger) this->show(), this->raise(), this->activateWindow(); });
    pSysTrayIcon->show();

    // 为logo安装事件过滤器
    ui->hand_logo->installEventFilter(this);

    // 设置路径
    m_strDirPatch = QApplication::applicationDirPath();
    m_strConfig = (m_strDirPatch + "/FCDConfig/config.ini").toStdWString();
    m_pDisplay = new DialogDisplay(0, m_strDirPatch);

    // 检查本地配置文件
    if (!QDir().exists(m_strDirPatch + "/FCDConfig"))
    {
        QDir().mkpath(m_strDirPatch + "/FCDConfig/data");
        // config.ini
        QFile outConfig(m_strDirPatch + "/FCDConfig/config.ini");
        if (outConfig.open(QIODevice::WriteOnly)) {
            outConfig.write(QString::fromLocal8Bit(
                                "[配置]\n"
                                "仅限三百=1\n"
                                "全体发言=1\n"
                                "启动按键=126\n"
                                "显示位置=1\n"
                                "热键屏蔽=0\n"
                                "切回首页=1\n"
                                "热键模式=0\n"
                                "连发模式=0\n"
                                "启用方案=0\n"
                                "[面板]\n"
                                "面板宽度=400\n"
                                "背景颜色=0,0,0,155\n"
                                "字体族类=微软雅黑\n"
                                "描边质量=2\n"
                                "[标题]\n"
                                "字体颜色=255,255,255\n"
                                "字体加粗=0\n"
                                "描边颜色=0,0,0,155\n"
                                "描边大小=0\n"
                                "[内容]\n"
                                "字体颜色=255,255,0\n"
                                "字体大小=16\n"
                                "字体加粗=0\n"
                                "描边颜色=0,0,0,155\n"
                                "描边大小=0\n"
                                ).toLocal8Bit());
            outConfig.close();
        }
        // fcdSchemeIndex
        QFile outFcdScheme(m_strDirPatch + "/FCDConfig/data/fcdSchemeIndex");
        if (outFcdScheme.open(QIODevice::WriteOnly)) {
            outFcdScheme.write("FCDINDEX", 8);
            outFcdScheme.write(QByteArray(99, '\0').append(1));
            outFcdScheme.close();
        }
        // 说明书
        QFile outInstructions(m_strDirPatch + "/FCDConfig/data/data99.fcdscheme");
        if (outInstructions.open(QIODevice::WriteOnly)) {
            auto lam_str = [&outInstructions](const wchar_t* wstr){
                QByteArray&& data = QString::fromWCharArray(wstr).toLocal8Bit();
                outInstructions.putChar(data.size());
                outInstructions.write(data);
            };
            outInstructions.write("FCD\0", 4);
            lam_str(L"使用说明书");
            lam_str(L"方案名称的首字母缩写和编号，可以用来搜索(显示面板搜索切换方案时)");
            lam_str(L"按下【启动按键】即可呼出喊话面板，【仅限三百】启用时只能在300内呼出，并且未选中任何方案时，也不能呼出喊话面板");
            lam_str(L"呼出面板后，按下数字键，即可选中对应内容发送");
            lam_str(L"如果开启了二级面板，需要按两次数字键才能发送内容：第1次选择首页索引进入对应的二级面板；第2次选择二级面板中的内容发送");
            lam_str(L"在面板首页按【0键】，可以切换预设方案，可通过编号(数字键)或者方案名(字母键)来搜索选择要切换的方案");
            lam_str(L"本喊话为外部独立程序，不会和300或其他程序有任何关联和影响，开源代码由logo右键获取");
            lam_str(L"一键喊话的原理是模拟虚拟按键：【回车键】(打开聊天框)->【Ctrl】【V】(粘贴)->【回车键】(发送)");
            lam_str(L"【高级设置】一般用非【300英雄】的使用场景，不会的尽量别乱调，鼠标放到对应设置上一定时间后，会有帮助提示来解释说明");
            lam_str(L"不慎删除说明书并且想要再看一下说明，删除配置文件夹“FCDConfig”，会重置所有内容，并重新生成说明书");
            outInstructions.close();
        }
    }

    // 读取本地配置文件
    {
        g_scheme.init(m_strDirPatch + "/FCDConfig/data");
        g_bOnly300 = ReadProfileInt(L"配置", L"仅限三百", 1, m_strConfig.data());
        g_bPublic = ReadProfileInt(L"配置", L"全体发言", 1, m_strConfig.data());
        g_bLeft = ReadProfileInt(L"配置", L"显示位置", 1, m_strConfig.data());
        g_bSeniorShield = ReadProfileInt(L"配置", L"热键屏蔽", 0, m_strConfig.data());
        g_bSeniorBack = ReadProfileInt(L"配置", L"切回首页", 1, m_strConfig.data());
        g_bSeniorMode = ReadProfileInt(L"配置", L"热键模式", 0, m_strConfig.data());
        g_seniorContinuity = ReadProfileInt(L"配置", L"连发模式", 0, m_strConfig.data());
        SetKeyValue(ReadProfileInt(L"配置", L"启动按键", '~', m_strConfig.data()));
        m_id = ReadProfileInt(L"配置", L"启用方案", 0, m_strConfig.data());
        if (!g_scheme.contains(m_id)) m_id = g_scheme.frontId();
    }

    // 初始化方案面板
    {
        for (auto& scheme : g_scheme.list()) {
            int id = scheme.id[0] * 10 + scheme.id[1];
            m_pFormList[id] = new FormScheme(id, this);
            bindEventOfFromScheme(id);
            ui->stackedWidget->addWidget(m_pFormList[id]);
        }
    }

    // 初始化UI
    {
        ui->hand_only300->setChecked(g_bOnly300);
        ui->hand_public->setChecked(g_bPublic);
        ui->hand_key->setCurrentText(QString(1, QChar(GetKeyValue())) + QString::fromLocal8Bit(" 键"));
        ui->hand_left->setCurrentIndex(g_bLeft ? 1 : 0);
        ui->senior_shield->setChecked(g_bSeniorShield);
        ui->senior_back->setChecked(g_bSeniorBack);
        ui->senior_mode->setCurrentIndex(g_bSeniorMode ? 1 : 0);
        ui->senior_continuity->setCurrentIndex(g_seniorContinuity);
        ui->stackedWidget_hand->setCurrentIndex(0);
        ui->hand_scheme_list->clear();
        for (const auto& scheme : g_scheme.list()) {
            ui->hand_scheme_list->addItem(QString::number(scheme.id[0]) + QString::number(scheme.id[1]) + "." + scheme[0][0]);
            if (scheme.id[0] * 10 + scheme.id[1] == m_id) {
                ui->hand_scheme_list->setCurrentIndex(ui->hand_scheme_list->count() - 1);
            }
        }
        if (m_id) {
            ui->stackedWidget->setCurrentWidget(m_pFormList[m_id]);
            m_pDisplay->updateId(m_id);
            m_pDisplay->updateAll();
            ui->label_selected_name->setText("#" + ToIdStr(m_id) + " - " + g_scheme[m_id][0][0]);
        }
        else {
            ui->stackedWidget->setCurrentIndex(0);
            ui->label_selected_name->setText(QString::fromLocal8Bit("##当前没有选中的方案##"));
        }
        readStyle();
        setMinimumSize(500, 420);
        setMaximumSize(500, 420);
    }

    // 绑定事件
    bindEventOfSystem();
    bindEventOfHand();
    bindEventOfStyle();
    bindEventOfKey();

    // 安装键盘钩子
    SetHook();
}

// 读取样式
void MainWindow::readStyle()
{
    ui->currency_width->setValue(ReadProfileInt(L"面板", L"面板宽度", 400, m_strConfig.data()));
    ui->currency_background_color->setStyleSheet("background-color: rgba(" + ReadProfileStr(L"面板", L"背景颜色", L"0,0,0,255", m_strConfig.data()) + ");");
    ui->currency_font_family->setCurrentFont(QFont(ReadProfileStr(L"面板", L"字体族类", L"微软雅黑", m_strConfig.data())));
    ui->currency_stroke_level->setCurrentIndex(ReadProfileInt(L"面板", L"描边质量", 2, m_strConfig.data()) / 2);

    ui->title_font_color->setStyleSheet("background-color: rgb(" + ReadProfileStr(L"标题", L"字体颜色", L"255,255,255", m_strConfig.data()) + ");");
    ui->title_font_bold->setChecked(ReadProfileInt(L"标题", L"字体加粗", 0, m_strConfig.data()));
    ui->title_stroke_color->setStyleSheet("background-color: rgba(" + ReadProfileStr(L"标题", L"描边颜色", L"0,0,0,155", m_strConfig.data()) + ");");
    ui->title_stroke_size->setValue(ReadProfileInt(L"标题", L"描边大小", 0, m_strConfig.data()));
    updateStyle(0);

    ui->content_font_color->setStyleSheet("background-color: rgb(" + ReadProfileStr(L"内容", L"字体颜色", L"255,255,255", m_strConfig.data()) + ");");
    ui->content_font_size->setValue(ReadProfileInt(L"内容", L"字体大小", 16, m_strConfig.data()));
    ui->content_font_bold->setChecked(ReadProfileInt(L"内容", L"字体加粗", 0, m_strConfig.data()));
    ui->content_stroke_color->setStyleSheet("background-color: rgba(" + ReadProfileStr(L"内容", L"描边颜色", L"0,0,0,155", m_strConfig.data()) + ");");
    ui->content_stroke_size->setValue(ReadProfileInt(L"内容", L"描边大小", 0, m_strConfig.data()));
    updateStyle(1);
}

// 写入样式
void MainWindow::writeStyle()
{
    WriteProfileStr(L"面板", L"面板宽度", QString::number(ui->currency_width->value()), m_strConfig.data());
    WriteProfileStr(L"面板", L"背景颜色", ToCssColor(ui->currency_background_color->styleSheet()), m_strConfig.data());
    WriteProfileStr(L"面板", L"字体族类", ui->currency_font_family->currentFont().family(), m_strConfig.data());
    WriteProfileStr(L"面板", L"描边质量", ui->currency_stroke_level->currentText().left(1), m_strConfig.data());

    WriteProfileStr(L"标题", L"字体颜色", ToCssColor(ui->title_font_color->styleSheet()), m_strConfig.data());
    WriteProfileStr(L"标题", L"字体加粗", QString::number(ui->title_font_bold->isChecked() ? 1 : 0), m_strConfig.data());
    WriteProfileStr(L"标题", L"描边颜色", ToCssColor(ui->title_stroke_color->styleSheet()), m_strConfig.data());
    WriteProfileStr(L"标题", L"描边大小", QString::number(ui->title_stroke_size->value()), m_strConfig.data());

    WriteProfileStr(L"内容", L"字体颜色", ToCssColor(ui->content_font_color->styleSheet()), m_strConfig.data());
    WriteProfileStr(L"内容", L"字体大小", QString::number(ui->content_font_size->value()), m_strConfig.data());
    WriteProfileStr(L"内容", L"字体加粗", QString::number(ui->content_font_bold->isChecked() ? 1 : 0), m_strConfig.data());
    WriteProfileStr(L"内容", L"描边颜色", ToCssColor(ui->content_stroke_color->styleSheet()), m_strConfig.data());
    WriteProfileStr(L"内容", L"描边大小", QString::number(ui->content_stroke_size->value()), m_strConfig.data());
}

// 更新样式预览
void MainWindow::updateStyle(const int& val)
{
    // 标题
    if (!val) {
        const QString strText = QString::fromLocal8Bit("#00 - 标题");
        if (ui->title_stroke_size->value() == 0) {
            ui->title_label_preview->clear();
            ui->title_label_preview->setText(strText);
            ui->title_label_preview->setStyleSheet("font:24px \"" + ui->currency_font_family->currentFont().family()
                                                   + (ui->title_font_bold->isChecked() ? "\";\nfont-weight: bold;\n" : "\";\n")
                                                   + ui->title_font_color->styleSheet().mid(11) + "\n"
                                                   + ui->currency_background_color->styleSheet());
        }
        else {
            QColor&& backgroundColor = ToCssQColor(ui->currency_background_color->styleSheet());
            QColor&& fontColor = ToCssQColor(ui->title_font_color->styleSheet());
            QColor&& strokeColor = ToCssQColor(ui->title_stroke_color->styleSheet());
            QFont font(ui->currency_font_family->currentFont());
            font.setBold(ui->title_font_bold->isChecked());
            font.setPixelSize(24);
            QFontMetrics metrics(font);
            const int& fontStart = (51 -  metrics.height() -  metrics.leading()) / 2 +  metrics.leading() + metrics.ascent();
            const int& scaleLevel = ui->currency_stroke_level->currentText().left(1).toInt();
            const int& strokeSize = ui->title_stroke_size->value();
            QImage img(156 * scaleLevel, 51 * scaleLevel, QImage::Format_ARGB32);
            img.fill(backgroundColor);
            font.setPixelSize(font.pixelSize() * scaleLevel);
            QPainterPath path;
            path.addText(strokeSize * scaleLevel, fontStart * scaleLevel, font, strText);
            QPainter(&img).strokePath(path, QPen(strokeColor, strokeSize * scaleLevel * 2));
            QPainter(&img).fillPath(path, fontColor);
            ui->title_label_preview->clear();
            ui->title_label_preview->setPixmap(QPixmap::fromImage(img.scaled(img.size() / scaleLevel, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
            ui->title_label_preview->setStyleSheet("");
        }
    }
    // 内容
    else {
        const QString strText = QString::fromLocal8Bit("1. 内容预览效果");
        if (ui->content_stroke_size->value() == 0) {
            ui->content_label_preview->clear();
            ui->content_label_preview->setText(strText);
            ui->content_label_preview->setStyleSheet("font:" + QString::number(ui->content_font_size->value()) + "px \""
                                                   + ui->currency_font_family->currentFont().family()
                                                   + (ui->content_font_bold->isChecked() ? "\";\nfont-weight: bold;\n" : "\";\n")
                                                   + ui->content_font_color->styleSheet().mid(11) + "\n"
                                                   + ui->currency_background_color->styleSheet());
        }
        else {
            QColor&& backgroundColor = ToCssQColor(ui->currency_background_color->styleSheet());
            QColor&& fontColor = ToCssQColor(ui->content_font_color->styleSheet());
            QColor&& strokeColor = ToCssQColor(ui->content_stroke_color->styleSheet());
            QFont font(ui->currency_font_family->currentFont());
            font.setBold(ui->content_font_bold->isChecked());
            font.setPixelSize(ui->content_font_size->value());
            QFontMetrics metrics(font);
            const int& fontStart = (51 -  metrics.height() -  metrics.leading()) / 2 +  metrics.leading() + metrics.ascent();
            const int& scaleLevel = ui->currency_stroke_level->currentText().left(1).toInt();
            const int& strokeSize = ui->content_stroke_size->value();
            QImage img(156 * scaleLevel, 51 * scaleLevel, QImage::Format_ARGB32);
            img.fill(backgroundColor);
            font.setPixelSize(font.pixelSize() * scaleLevel);
            QPainterPath path;
            path.addText(strokeSize * scaleLevel, fontStart * scaleLevel, font, strText);
            QPainter(&img).strokePath(path, QPen(strokeColor, strokeSize * scaleLevel * 2));
            QPainter(&img).fillPath(path, fontColor);
            ui->content_label_preview->clear();
            ui->content_label_preview->setPixmap(QPixmap::fromImage(img.scaled(img.size() / scaleLevel, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
            ui->content_label_preview->setStyleSheet("");
        }
    }
}

// 绑定事件【方案面板】
void MainWindow::bindEventOfFromScheme(const int &id)
{
    // 编号已存在
    connect(m_pFormList[id], &FormScheme::sendErorrID, this, [&](int newid){
        QMessageBox::warning(this, QString::fromWCharArray(L"警告"), QString::fromWCharArray(L"编号(") + ToIdStr(newid) + QString::fromWCharArray(L")已被占用！"));
    });
    // 更新编号
    connect(m_pFormList[id], &FormScheme::sendUpdateID, this, [&](int oldid, int newid){
        m_pFormList[newid] = m_pFormList[oldid];
        m_pFormList.remove(oldid);
        m_id = newid;
        if (newid == m_pDisplay->id()) {
            m_pDisplay->updateId(newid);
            ui->label_selected_name->setText("#" + ToIdStr(newid) + " - " + g_scheme[newid][0][0]);
        }
        m_bSwiachLock = true;
        ui->hand_scheme_list->removeItem(ui->hand_scheme_list->currentIndex());
        int index = 0;
        for (auto&& it = m_pFormList.begin(), && itend = m_pFormList.end(); it != itend ; ++it, ++index) {
            if (it.key() != newid) continue;
            ui->hand_scheme_list->insertItem(index, ToIdStr(newid) + "." + g_scheme[newid][0][0]);
            ui->hand_scheme_list->setCurrentIndex(index);
            break;
        }
        m_bSwiachLock = false;
    });
    // 更新标题
    connect(m_pFormList[id], &FormScheme::sendUpdateTitle, this, [&](){
        if (m_id == m_pDisplay->id()) {
            m_pDisplay->updateTitle();
            ui->label_selected_name->setText("#" + ToIdStr(m_id) + " - " + g_scheme[m_id][0][0]);
        }
        m_bSwiachLock = true;
        const int& index = ui->hand_scheme_list->currentIndex();
        ui->hand_scheme_list->removeItem(index);
        ui->hand_scheme_list->insertItem(index, ToIdStr(m_id) + "." + g_scheme[m_id][0][0]);
        ui->hand_scheme_list->setCurrentIndex(index);
        m_bSwiachLock = false;
    });
    // 更新提示部分
    connect(m_pFormList[id], &FormScheme::sendUpdateTip, this, [&](){ if (m_id == m_pDisplay->id()) m_pDisplay->updateTip(); });
    // 更新全部
    connect(m_pFormList[id], &FormScheme::sendUpdateAll, this, [&](){
        if (m_id == m_pDisplay->id()) {
            m_pDisplay->updateAll();
            ui->label_selected_name->setText("#" + ToIdStr(m_id) + " - " + g_scheme[m_id][0][0]);
        }
        m_bSwiachLock = true;
        const int& index = ui->hand_scheme_list->currentIndex();
        ui->hand_scheme_list->removeItem(index);
        ui->hand_scheme_list->insertItem(index, ToIdStr(m_id) + "." + g_scheme[m_id][0][0]);
        ui->hand_scheme_list->setCurrentIndex(index);
        m_bSwiachLock = false;
    });
}

// 绑定事件【系统按钮】
void MainWindow::bindEventOfSystem()
{
    // 退出
    connect(ui->button_close, &QPushButton::clicked, this, [&]{ this->close(); });
    // 最小化
    connect(ui->button_min, &QPushButton::clicked, this, [&]{ this->showMinimized(); });
    // 托盘化
    connect(ui->button_tray, &QPushButton::clicked, this, [&]{ this->hide(); });
    // 显示/隐藏 高级设置面板
    connect(ui->button_senior, &QPushButton::clicked, this, [&]{ ui->stackedWidget_hand->setCurrentIndex(ui->stackedWidget_hand->currentIndex() ? 0 : 1); });
    // 显示/隐藏 样式设置面板
    connect(ui->button_style, &QPushButton::clicked, this, [&]{
        if (minimumWidth() < 600) {
            setMaximumWidth(661);
            setMinimumWidth(661);
        }
        else {
            setMinimumWidth(500);
            setMaximumWidth(500);
        }
    });
}

// 绑定事件【头部】
void MainWindow::bindEventOfHand()
{
    // 仅限三百
    connect(ui->hand_only300, &QCheckBox::stateChanged, this, [&](int state){ g_bOnly300 = state; WriteProfileStr(L"配置", L"仅限三百", g_bOnly300 ? "1" : "0", m_strConfig.data()); });
    // 全体发言
    connect(ui->hand_public, &QCheckBox::stateChanged, this, [&](int state){ g_bPublic = state; WriteProfileStr(L"配置", L"全体发言", g_bPublic ? "1" : "0", m_strConfig.data()); });
    // 启动按键
    connect(ui->hand_key, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, [&](const QString& str){
        if (str.isEmpty()) return;
        SetKeyValue(str.toLocal8Bit().at(0));
        WriteProfileStr(L"配置", L"启动按键", QString::number(GetKeyValue()), m_strConfig.data());
    });
    // 显示位置
    connect(ui->hand_left, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [&](int index){ g_bLeft = index; WriteProfileStr(L"配置", L"显示位置", g_bLeft ? "1" : "0", m_strConfig.data()); });
    // 热键屏蔽
    connect(ui->senior_shield, &QCheckBox::stateChanged, this, [&](int state){ g_bSeniorShield = state; WriteProfileStr(L"配置", L"热键屏蔽", g_bSeniorShield ? "1" : "0", m_strConfig.data()); });
    // 切回首页
    connect(ui->senior_back, &QCheckBox::stateChanged, this, [&](int state){ g_bSeniorBack = state; WriteProfileStr(L"配置", L"切回首页", g_bSeniorBack ? "1" : "0", m_strConfig.data()); });
    // 热键模式
    connect(ui->senior_mode, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [&](int index){ g_bSeniorMode = index; WriteProfileStr(L"配置", L"热键模式", g_bSeniorMode ? "1" : "0", m_strConfig.data()); });
    // 连发模式
    connect(ui->senior_continuity, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [&](int index){ g_seniorContinuity = index; WriteProfileStr(L"配置", L"连发模式", QString::number(g_seniorContinuity), m_strConfig.data()); });
    // 切换方案
    connect(ui->hand_scheme_list, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, [&](const QString& str){
        if (m_bSwiachLock) return;
        if (str.isEmpty()) return;
        m_id = str.left(2).toInt();
        ui->stackedWidget->setCurrentWidget(m_pFormList[m_id]);
    });
    // 导入方案
    connect(ui->hand_scheme_import, &QPushButton::clicked, this, [&]{
        const QString& strFileName = QFileDialog::getOpenFileName(this, QString::fromWCharArray(L"请选择要导入的方案预设"), m_strDirPatch, "Scheme File(*.fcdscheme)");
        if (!strFileName.isEmpty()) {
            const int& id = g_scheme.import(strFileName);
            if (!id) QMessageBox::warning(this, QString::fromWCharArray(L"警告"), QString::fromWCharArray(L"导入预设方案失败！"));
            else {
                m_pFormList[id] = new FormScheme(id, this);
                ui->stackedWidget->addWidget(m_pFormList[id]);
                bindEventOfFromScheme(id);
                int index = 0;
                for (auto&& it = m_pFormList.begin(), && itend = m_pFormList.end(); it != itend ; ++it, ++index) {
                    if (it.key() != id) continue;
                    ui->hand_scheme_list->insertItem(index, ToIdStr(id) + "." + g_scheme[id][0][0]);
                    ui->hand_scheme_list->setCurrentIndex(index);
                    break;
                }
                if (m_pDisplay->id() == 0) ui->hand_scheme_selected->clicked();
            }
        }
    });
    // 导出方案
    connect(ui->hand_scheme_output, &QPushButton::clicked, this, [&]{
        if (!m_id) return;
        const QString& strSavePath = QFileDialog::getSaveFileName(this, QString::fromLocal8Bit("请选择预设方案导出位置"), m_strDirPatch + "/" + g_scheme[m_id][0][0] + ".fcdscheme", "Scheme Files (*.fcdscheme)");
        if (strSavePath.isEmpty()) return;
        if (g_scheme.output(strSavePath, m_id)) QMessageBox::about(this, QString::fromWCharArray(L"提示"), QString::fromWCharArray(L"导出成功！"));
        else QMessageBox::warning(this, QString::fromWCharArray(L"警告"), QString::fromWCharArray(L"导出预设方案失败！"));
    });
    // 选中方案
    connect(ui->hand_scheme_selected, &QPushButton::clicked, this, [&]{
        if (!m_id) return;
        if (m_id == m_pDisplay->id()) return;
        WriteProfileStr(L"配置", L"启用方案", QString::number(m_id), m_strConfig.data());
        m_pDisplay->updateId(m_id);
        m_pDisplay->updateAll();
        ui->label_selected_name->setText("#" + ToIdStr(m_id) + " - " + g_scheme[m_id][0][0]);
    });
    // 新建方案
    connect(ui->hand_scheme_new, &QPushButton::clicked, this, [&]{
        const int& id = g_scheme.getBlankId();
        if (!id) {
            QMessageBox::warning(this, QString::fromWCharArray(L"警告"), QString::fromWCharArray(L"预设方案数量已满(99)！"));
            return;
        }
        g_scheme.append(id);
        m_pFormList[id] = new FormScheme(id, this);
        ui->stackedWidget->addWidget(m_pFormList[id]);
        bindEventOfFromScheme(id);
        int index = 0;
        for (auto&& it = m_pFormList.begin(), && itend = m_pFormList.end(); it != itend ; ++it, ++index) {
            if (it.key() != id) continue;
            ui->hand_scheme_list->insertItem(index, ToIdStr(id) + "." + g_scheme[id][0][0]);
            ui->hand_scheme_list->setCurrentIndex(index);
            break;
        }
        if (m_pDisplay->id() == 0) ui->hand_scheme_selected->clicked();
    });
    // 删除方案
    connect(ui->hand_scheme_delete, &QPushButton::clicked, this, [&]{
        if (!m_id) return;
        if (QMessageBox::Yes != QMessageBox::question(this, QString::fromLocal8Bit("删除方案"), QString::fromLocal8Bit("是否确认要完全删除此预设方案？"), QMessageBox::Yes | QMessageBox::No)) return;
        int oldid = m_id;
        m_pFormList.remove(m_id);
        g_scheme.remove(m_id);
        m_bSwiachLock = true;
        ui->hand_scheme_list->removeItem(ui->hand_scheme_list->currentIndex());
        if (ui->hand_scheme_list->count()) {
            m_bSwiachLock = false;
            ui->hand_scheme_list->setCurrentIndex(0);
            m_id = ui->hand_scheme_list->currentText().left(2).toInt();
            if (oldid == m_pDisplay->id()) {
                m_pDisplay->updateId(oldid);
                m_pDisplay->updateAll();
            }
        }
        else {
            m_bSwiachLock = false;
            m_id = 0;
            m_pDisplay->updateId(0);
            ui->stackedWidget->setCurrentIndex(0);
            ui->label_selected_name->setText(QString::fromLocal8Bit("##当前没有选中的方案##"));
        }
    });
}

// 绑定事件【样式面板】
void MainWindow::bindEventOfStyle()
{
    // 【通用】背景颜色
    connect(ui->currency_background_color, &QPushButton::clicked, this, [&]{
        QColor&& color = QColorDialog::getColor(ToCssQColor(ui->currency_background_color->styleSheet()), this, QString::fromLocal8Bit("请选择颜色"), QColorDialog::ShowAlphaChannel);
        if (!color.isValid()) return;
        ui->currency_background_color->setStyleSheet("background-color: rgba("
                                                     + QString::number(color.red()) + ","
                                                     + QString::number(color.green()) + ","
                                                     + QString::number(color.blue()) + ","
                                                     + QString::number(color.alpha()) + ");");
        updateStyle(0);
        updateStyle(1);
    });
    // 【通用】字体族类
    connect(ui->currency_font_family, &QFontComboBox::currentFontChanged, this, [&]{
        updateStyle(0);
        updateStyle(1);
    });
    // 【通用】描边质量
    connect(ui->currency_stroke_level, static_cast<void (QComboBox::*)(const QString&)>(&QComboBox::currentIndexChanged), this, [&](const QString&){
        if (ui->title_stroke_size->value()) updateStyle(0);
        if (ui->content_stroke_size->value()) updateStyle(1);
    });

    // 【标题】字体颜色
    connect(ui->title_font_color, &QPushButton::clicked, this, [&]{
        QColor&& color = QColorDialog::getColor(ToCssQColor(ui->title_font_color->styleSheet()), this, QString::fromLocal8Bit("请选择颜色"));
        if (!color.isValid()) return;
        ui->title_font_color->setStyleSheet("background-color: rgb("
                                            + QString::number(color.red()) + ","
                                            + QString::number(color.green()) + ","
                                            + QString::number(color.blue()) + ");");
        updateStyle(0);
    });
    // 【标题】字体加粗
    connect(ui->title_font_bold, &QCheckBox::stateChanged, this, [&]{ updateStyle(0); });
    // 【标题】描边颜色
    connect(ui->title_stroke_color, &QPushButton::clicked, this, [&]{
        QColor&& color = QColorDialog::getColor(ToCssQColor(ui->title_stroke_color->styleSheet()), this, QString::fromLocal8Bit("请选择颜色"), QColorDialog::ShowAlphaChannel);
        if (!color.isValid()) return;
        ui->title_stroke_color->setStyleSheet("background-color: rgba("
                                              + QString::number(color.red()) + ","
                                              + QString::number(color.green()) + ","
                                              + QString::number(color.blue()) + ","
                                              + QString::number(color.alpha()) + ");");
        updateStyle(0);
    });
    // 【标题】描边大小
    connect(ui->title_stroke_size,  static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [&]{ updateStyle(0); });

    // 【内容】字体颜色
    connect(ui->content_font_color, &QPushButton::clicked, this, [&]{
        QColor&& color = QColorDialog::getColor(ToCssQColor(ui->content_font_color->styleSheet()), this, QString::fromLocal8Bit("请选择颜色"));
        if (!color.isValid()) return;
        ui->content_font_color->setStyleSheet("background-color: rgb("
                                            + QString::number(color.red()) + ","
                                            + QString::number(color.green()) + ","
                                            + QString::number(color.blue()) + ");");
        updateStyle(1);
    });
    // 【内容】字体大小
    connect(ui->content_font_size,  static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [&]{ updateStyle(1); });
    // 【内容】字体加粗
    connect(ui->content_font_bold, &QCheckBox::stateChanged, this, [&]{ updateStyle(1); });
    // 【内容】描边颜色
    connect(ui->content_stroke_color, &QPushButton::clicked, this, [&]{
        QColor&& color = QColorDialog::getColor(ToCssQColor(ui->content_stroke_color->styleSheet()), this, QString::fromLocal8Bit("请选择颜色"), QColorDialog::ShowAlphaChannel);
        if (!color.isValid()) return;
        ui->content_stroke_color->setStyleSheet("background-color: rgba("
                                              + QString::number(color.red()) + ","
                                              + QString::number(color.green()) + ","
                                              + QString::number(color.blue()) + ","
                                              + QString::number(color.alpha()) + ");");
        updateStyle(1);
    });
    // 【内容】描边大小
    connect(ui->content_stroke_size,  static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, [&]{ updateStyle(1); });

    // 【保存样式】
    connect(ui->style_save, &QPushButton::clicked, this, [&]{ writeStyle(); m_pDisplay->updateAll(); });
    // 【还原样式】
    connect(ui->style_cancel, &QPushButton::clicked, this, [&]{ readStyle(); });
}

// 绑定事件【键盘钩子】
void MainWindow::bindEventOfKey()
{
    // 获取id
    connect(&g_key, &CKey::getid, this, [&]() -> int { return m_pDisplay->id(); });
    // 设置位置
    connect(&g_key, &CKey::sendRect, this, [&](int x, int y, int w, int h) {
        if (g_bLeft) m_pDisplay->move(x, y);
        else m_pDisplay->move(x + w - m_pDisplay->width(), y);
        m_pDisplay->resize(m_pDisplay->width(), h);
    });
    // 显示面板
    connect(&g_key, &CKey::show, m_pDisplay, &QWidget::show);
    // 隐藏面板
    connect(&g_key, &CKey::hide, m_pDisplay, &QWidget::hide);
    // 切换面板
    connect(&g_key, &CKey::switchPage, this, [&](int page) {
        if (page == -1) {
            m_pDisplay->lookupInit();
            m_pDisplay->switchPage(10);
        }
        else m_pDisplay->switchPage(page);
    });
    // 切换方案
    connect(&g_key, &CKey::switchId, this, [&](int newid) {
        m_pDisplay->updateId(newid);
        m_pDisplay->updateAll();
        m_pDisplay->switchPage(0);
        WriteProfileStr(L"配置", L"启用方案", QString::number(newid), m_strConfig.data());
        ui->label_selected_name->setText("#" + ToIdStr(newid) + " - " + g_scheme[newid][0][0]);
    });
    // 按id搜索方案
    connect(&g_key, &CKey::lookupId, m_pDisplay, static_cast<void (DialogDisplay::*)(const int&, const QList<const SSchemeData*>&)>(&DialogDisplay::lookupList));
    // 按拼写搜索方案
    connect(&g_key, &CKey::lookupName, m_pDisplay, static_cast<void (DialogDisplay::*)(const QString&, const QList<const SSchemeData*>&)>(&DialogDisplay::lookupList));
    // 调用剪贴板
    connect(&g_key, &CKey::clipboard, this, [&](const QString& str){ QApplication::clipboard()->setText(str); });
    // 快速设置连发模式
    connect(&g_key, &CKey::setContinuity, ui->senior_continuity, &QComboBox::setCurrentIndex);
}
