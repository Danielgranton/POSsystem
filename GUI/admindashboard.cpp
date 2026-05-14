#include "admindashboard.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPalette>
#include <QPushButton>
#include <QProxyStyle>
#include <QScrollArea>
#include <QSpacerItem>
#include <QSpinBox>
#include <QStyle>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

namespace {

QTableWidgetItem* makeItem(const QString& text)
{
    auto* item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

QFrame* makeStatCard(const QString& title, QLabel** valueLabel, const QString& accentColor)
{
    auto* card = new QFrame();
    card->setObjectName("statCard");

    auto* layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(6);

    auto* titleLabel = new QLabel(title, card);
    titleLabel->setObjectName("statTitle");

    auto* value = new QLabel("0", card);
    value->setObjectName("statValue");

    auto* accent = new QFrame(card);
    accent->setFixedHeight(3);
    accent->setStyleSheet(QString("background: %1; border-radius: 2px;").arg(accentColor));

    layout->addWidget(titleLabel);
    layout->addWidget(value);
    layout->addStretch();
    layout->addWidget(accent);

    *valueLabel = value;
    return card;
}

QTableWidget* makeTable(const QStringList& headers)
{
    auto* table = new QTableWidget();
    table->setObjectName("dataTable");
    table->setColumnCount(headers.size());
    table->setHorizontalHeaderLabels(headers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setAlternatingRowColors(true);
    table->setShowGrid(false);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setMinimumHeight(220);
    table->setFrameShape(QFrame::NoFrame);
    return table;
}

} // namespace

AdminDashboard::AdminDashboard(QWidget *parent)
    : QMainWindow(parent)
{
    // ── Force Fusion style so stylesheet takes full effect ──
    QApplication::setStyle("Fusion");

    QPalette pal;
    pal.setColor(QPalette::Window,          QColor("#f1f5f9"));
    pal.setColor(QPalette::WindowText,      QColor("#0f172a"));
    pal.setColor(QPalette::Base,            QColor("#ffffff"));
    pal.setColor(QPalette::AlternateBase,   QColor("#f8fafc"));
    pal.setColor(QPalette::Text,            QColor("#0f172a"));
    pal.setColor(QPalette::Button,          QColor("#ffffff"));
    pal.setColor(QPalette::ButtonText,      QColor("#374151"));
    pal.setColor(QPalette::Highlight,       QColor("#dbeafe"));
    pal.setColor(QPalette::HighlightedText, QColor("#1e3a5f"));
    pal.setColor(QPalette::Mid,             QColor("#e2e8f0"));
    pal.setColor(QPalette::Dark,            QColor("#cbd5e1"));
    QApplication::setPalette(pal);

    setWindowTitle("Admin Dashboard");
    resize(1280, 780);

    auto* root = new QWidget(this);
    root->setObjectName("root");
    setCentralWidget(root);

    auto* shell = new QHBoxLayout(root);
    shell->setContentsMargins(0, 0, 0, 0);
    shell->setSpacing(0);

    // ══════════════════════════════════════
    //  SIDEBAR
    // ══════════════════════════════════════
    auto* sidebar = new QFrame(root);
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(232);

    auto* sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(0, 0, 0, 0);
    sideLayout->setSpacing(0);

    // Logo row
    auto* logoWidget = new QWidget(sidebar);
    logoWidget->setObjectName("logoWidget");
    auto* logoLayout = new QHBoxLayout(logoWidget);
    logoLayout->setContentsMargins(18, 18, 18, 16);
    logoLayout->setSpacing(10);

    auto* logoMark = new QLabel("POS", logoWidget);
    logoMark->setObjectName("logoMark");
    logoMark->setFixedSize(34, 34);
    logoMark->setAlignment(Qt::AlignCenter);

    auto* logoName = new QLabel("POS System", logoWidget);
    logoName->setObjectName("logoName");

    logoLayout->addWidget(logoMark);
    logoLayout->addWidget(logoName);
    logoLayout->addStretch();

    // Section label helper
    auto makeSectionLabel = [&](const QString& text) -> QLabel* {
        auto* lbl = new QLabel(text, sidebar);
        lbl->setObjectName("sectionLabel");
        lbl->setContentsMargins(18, 14, 18, 4);
        return lbl;
    };

    // Nav button helper
    auto makeNavBtn = [&](const QString& text, bool active = false) -> QPushButton* {
        auto* btn = new QPushButton(text, sidebar);
        btn->setObjectName(active ? "navActive" : "navBtn");
        btn->setFlat(true);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(38);
        btn->setIconSize(QSize(16, 16));
        return btn;
    };

    auto* menuLabel     = makeSectionLabel("MENU");
    auto* dashBtn       = makeNavBtn("   Dashboard", true);
    auto* productsBtn   = makeNavBtn("   Products");
    auto* salesBtn      = makeNavBtn("   Sales");
    auto* usersBtn      = makeNavBtn("   Users");
    auto* actionsLabel  = makeSectionLabel("ACTIONS");
    auto* refreshBtn    = makeNavBtn("   Refresh data");
    auto* addProductBtn = makeNavBtn("   Add product");
    auto* sellProductBtn= makeNavBtn("   Sell product");
    auto* updateStockBtn= makeNavBtn("   Update stock");
    auto* addUserBtn    = makeNavBtn("   Add user");
    auto* removeUserBtn = makeNavBtn("   Remove user");

    // User info
    auto* userWidget = new QWidget(sidebar);
    userWidget->setObjectName("userWidget");
    auto* userRow = new QHBoxLayout(userWidget);
    userRow->setContentsMargins(18, 12, 18, 12);
    userRow->setSpacing(10);

    auto* avatar = new QLabel("A", userWidget);
    avatar->setObjectName("avatar");
    avatar->setFixedSize(32, 32);
    avatar->setAlignment(Qt::AlignCenter);

    auto* userInfoWidget = new QWidget(userWidget);
    auto* uil = new QVBoxLayout(userInfoWidget);
    uil->setContentsMargins(0, 0, 0, 0);
    uil->setSpacing(1);
    auto* userNameLbl = new QLabel("Administrator", userInfoWidget);
    userNameLbl->setObjectName("userName");
    auto* userRoleLbl = new QLabel("Admin session", userInfoWidget);
    userRoleLbl->setObjectName("userRole");
    uil->addWidget(userNameLbl);
    uil->addWidget(userRoleLbl);
    userRow->addWidget(avatar);
    userRow->addWidget(userInfoWidget);

    auto* logoutBtn = new QPushButton("   Logout", sidebar);
    logoutBtn->setObjectName("logoutBtn");
    logoutBtn->setFlat(true);
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setFixedHeight(40);

    sideLayout->addWidget(logoWidget);
    sideLayout->addWidget(menuLabel);
    sideLayout->addWidget(dashBtn);
    sideLayout->addWidget(productsBtn);
    sideLayout->addWidget(salesBtn);
    sideLayout->addWidget(usersBtn);
    sideLayout->addWidget(actionsLabel);
    sideLayout->addWidget(refreshBtn);
    sideLayout->addWidget(addProductBtn);
    sideLayout->addWidget(sellProductBtn);
    sideLayout->addWidget(updateStockBtn);
    sideLayout->addWidget(addUserBtn);
    sideLayout->addWidget(removeUserBtn);
    sideLayout->addStretch();
    sideLayout->addWidget(userWidget);
    sideLayout->addWidget(logoutBtn);

    // ══════════════════════════════════════
    //  MAIN AREA
    // ══════════════════════════════════════
    auto* mainArea = new QWidget(root);
    mainArea->setObjectName("mainArea");
    auto* mainLayout = new QVBoxLayout(mainArea);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Topbar
    auto* topbar = new QFrame(mainArea);
    topbar->setObjectName("topbar");
    topbar->setFixedHeight(68);
    auto* topbarRow = new QHBoxLayout(topbar);
    topbarRow->setContentsMargins(24, 0, 24, 0);
    topbarRow->setSpacing(8);

    auto* pageInfo = new QWidget(topbar);
    pageInfo->setObjectName("pageInfo");
    auto* pil = new QVBoxLayout(pageInfo);
    pil->setContentsMargins(0, 0, 0, 0);
    pil->setSpacing(2);
    auto* pageTitle = new QLabel("Admin Dashboard", pageInfo);
    pageTitle->setObjectName("pageTitle");
    auto* pageSub = new QLabel("Live data from PostgreSQL", pageInfo);
    pageSub->setObjectName("pageSub");
    pil->addWidget(pageTitle);
    pil->addWidget(pageSub);

    // Top action buttons
    auto makeTopBtn = [&](const QString& text, const QString& objName) -> QPushButton* {
        auto* btn = new QPushButton(text, topbar);
        btn->setObjectName(objName);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setFixedHeight(34);
        return btn;
    };

    auto* topRefreshBtn    = makeTopBtn("Refresh",      "topBtnDefault");
    auto* topAddProductBtn = makeTopBtn("+ Add Product","topBtnPrimary");
    auto* topSellBtn       = makeTopBtn("Sell Product", "topBtnDefault");
    auto* topUsersBtn      = makeTopBtn("Users",        "topBtnDefault");
    auto* topLogoutBtn     = makeTopBtn("Logout",       "topBtnDanger");

    topbarRow->addWidget(pageInfo);
    topbarRow->addStretch();
    topbarRow->addWidget(topRefreshBtn);
    topbarRow->addWidget(topAddProductBtn);
    topbarRow->addWidget(topSellBtn);
    topbarRow->addWidget(topUsersBtn);
    topbarRow->addWidget(topLogoutBtn);

    // Scroll content
    auto* scroll = new QScrollArea(mainArea);
    scroll->setObjectName("scrollArea");
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* contentWidget = new QWidget();
    contentWidget->setObjectName("contentWidget");
    auto* contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(24, 20, 24, 20);
    contentLayout->setSpacing(18);

    // Stat cards
    auto* statsRow = new QHBoxLayout();
    statsRow->setSpacing(12);
    statsRow->addWidget(makeStatCard("Total products", &totalProductsValue_, "#2563eb"));
    statsRow->addWidget(makeStatCard("Low stock items", &lowStockValue_,     "#f59e0b"));
    statsRow->addWidget(makeStatCard("Sales records",  &salesValue_,         "#16a34a"));
    statsRow->addWidget(makeStatCard("Users",          &usersValue_,         "#7c3aed"));
    statsRow->addWidget(makeStatCard("Revenue",        &revenueValue_,       "#0f766e"));

    // Products section
    auto* productsCard = new QFrame(contentWidget);
    productsCard->setObjectName("tableCard");
    auto* productsCardLayout = new QVBoxLayout(productsCard);
    productsCardLayout->setContentsMargins(0, 0, 0, 0);
    productsCardLayout->setSpacing(0);

    auto* productsHeader = new QWidget(productsCard);
    productsHeader->setObjectName("tableCardHeader");
    auto* productsHeaderLayout = new QHBoxLayout(productsHeader);
    productsHeaderLayout->setContentsMargins(16, 14, 16, 14);
    auto* productsTitle = new QLabel("Products", productsHeader);
    productsTitle->setObjectName("tableCardTitle");
    auto* productsHint = new QLabel("Current inventory", productsHeader);
    productsHint->setObjectName("tableCardHint");
    productsHeaderLayout->addWidget(productsTitle);
    productsHeaderLayout->addStretch();
    productsHeaderLayout->addWidget(productsHint);

    productsTable_ = makeTable({"ID", "Name", "Price", "Stock"});
    productsCardLayout->addWidget(productsHeader);
    productsCardLayout->addWidget(productsTable_);

    // Bottom row
    auto* bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(14);

    // Sales card
    auto* salesCard = new QFrame(contentWidget);
    salesCard->setObjectName("tableCard");
    auto* salesCardLayout = new QVBoxLayout(salesCard);
    salesCardLayout->setContentsMargins(0, 0, 0, 0);
    salesCardLayout->setSpacing(0);
    auto* salesHeader = new QWidget(salesCard);
    salesHeader->setObjectName("tableCardHeader");
    auto* salesHeaderLayout = new QHBoxLayout(salesHeader);
    salesHeaderLayout->setContentsMargins(16, 14, 16, 14);
    auto* salesTitle = new QLabel("Recent Sales", salesHeader);
    salesTitle->setObjectName("tableCardTitle");
    auto* salesHint = new QLabel("Latest 10 transactions", salesHeader);
    salesHint->setObjectName("tableCardHint");
    salesHeaderLayout->addWidget(salesTitle);
    salesHeaderLayout->addStretch();
    salesHeaderLayout->addWidget(salesHint);
    salesTable_ = makeTable({"Invoice", "Product", "Qty", "Total", "Date"});
    salesCardLayout->addWidget(salesHeader);
    salesCardLayout->addWidget(salesTable_);

    // Users card
    auto* usersCard = new QFrame(contentWidget);
    usersCard->setObjectName("tableCard");
    auto* usersCardLayout = new QVBoxLayout(usersCard);
    usersCardLayout->setContentsMargins(0, 0, 0, 0);
    usersCardLayout->setSpacing(0);
    auto* usersHeader = new QWidget(usersCard);
    usersHeader->setObjectName("tableCardHeader");
    auto* usersHeaderLayout = new QHBoxLayout(usersHeader);
    usersHeaderLayout->setContentsMargins(16, 14, 16, 14);
    auto* usersTitle = new QLabel("Users", usersHeader);
    usersTitle->setObjectName("tableCardTitle");
    auto* usersHint = new QLabel("Accounts & roles", usersHeader);
    usersHint->setObjectName("tableCardHint");
    usersHeaderLayout->addWidget(usersTitle);
    usersHeaderLayout->addStretch();
    usersHeaderLayout->addWidget(usersHint);
    usersTable_ = makeTable({"ID", "Username", "Role"});
    usersCardLayout->addWidget(usersHeader);
    usersCardLayout->addWidget(usersTable_);

    bottomRow->addWidget(salesCard, 3);
    bottomRow->addWidget(usersCard, 2);

    statusLabel_ = new QLabel("Ready.", contentWidget);
    statusLabel_->setObjectName("statusLabel");

    contentLayout->addLayout(statsRow);
    contentLayout->addWidget(productsCard);
    contentLayout->addLayout(bottomRow);
    contentLayout->addWidget(statusLabel_);
    contentLayout->addStretch();

    scroll->setWidget(contentWidget);
    mainLayout->addWidget(topbar);
    mainLayout->addWidget(scroll);

    shell->addWidget(sidebar);
    shell->addWidget(mainArea, 1);

    // ══════════════════════════════════════
    //  STYLESHEET — Fusion-safe, no inheritance issues
    // ══════════════════════════════════════
    setStyleSheet(R"(

        /* Root */
        QMainWindow { background: #f1f5f9; }
        QWidget#root { background: #f1f5f9; }
        QWidget#mainArea { background: #f1f5f9; }
        QWidget#contentWidget { background: #f1f5f9; }
        QWidget#pageInfo { background: transparent; }
        QScrollArea#scrollArea { background: #f1f5f9; border: none; }
        QScrollArea#scrollArea > QWidget > QWidget { background: #f1f5f9; }

        /* Scrollbar */
        QScrollBar:vertical {
            background: #f1f5f9;
            width: 7px;
            margin: 0;
            border-radius: 4px;
        }
        QScrollBar::handle:vertical {
            background: #cbd5e1;
            border-radius: 4px;
            min-height: 24px;
        }
        QScrollBar::add-line:vertical,
        QScrollBar::sub-line:vertical { height: 0; }

        /* ── Sidebar ── */
        QFrame#sidebar {
            background: #1e293b;
            border: none;
        }
        QWidget#logoWidget {
            background: #1e293b;
            border-bottom: 1px solid #334155;
        }
        QLabel#logoMark {
            background: #2563eb;
            color: #ffffff;
            border-radius: 8px;
            font-size: 11px;
            font-weight: bold;
        }
        QLabel#logoName {
            color: #f1f5f9;
            font-size: 14px;
            font-weight: bold;
        }
        QLabel#sectionLabel {
            background: transparent;
            color: #64748b;
            font-size: 10px;
            font-weight: bold;
        }
        QPushButton#navBtn {
            background: transparent;
            color: #94a3b8;
            border: none;
            text-align: left;
            padding-left: 18px;
            font-size: 13px;
        }
        QPushButton#navBtn:hover {
            background: #334155;
            color: #f1f5f9;
        }
        QPushButton#navActive {
            background: #2563eb;
            color: #ffffff;
            border: none;
            text-align: left;
            padding-left: 18px;
            font-size: 13px;
            font-weight: bold;
        }
        QWidget#userWidget {
            background: #1e293b;
            border-top: 1px solid #334155;
        }
        QLabel#avatar {
            background: #2563eb;
            color: #ffffff;
            border-radius: 16px;
            font-size: 13px;
            font-weight: bold;
        }
        QLabel#userName {
            background: transparent;
            color: #f1f5f9;
            font-size: 12px;
            font-weight: bold;
        }
        QLabel#userRole {
            background: transparent;
            color: #64748b;
            font-size: 11px;
        }
        QPushButton#logoutBtn {
            background: transparent;
            color: #f87171;
            border: none;
            border-top: 1px solid #334155;
            text-align: left;
            padding-left: 18px;
            font-size: 13px;
        }
        QPushButton#logoutBtn:hover {
            background: #7f1d1d;
            color: #fecaca;
        }

        /* ── Topbar ── */
        QFrame#topbar {
            background: #ffffff;
            border-bottom: 1px solid #e2e8f0;
        }
        QLabel#pageTitle {
            background: transparent;
            color: #0f172a;
            font-size: 16px;
            font-weight: bold;
        }
        QLabel#pageSub {
            background: transparent;
            color: #94a3b8;
            font-size: 11px;
        }
        QPushButton#topBtnDefault {
            background: #ffffff;
            color: #374151;
            border: 1px solid #d1d5db;
            border-radius: 7px;
            padding: 0 14px;
            font-size: 12px;
        }
        QPushButton#topBtnDefault:hover {
            background: #f8fafc;
            border-color: #9ca3af;
        }
        QPushButton#topBtnDefault:pressed {
            background: #f1f5f9;
        }
        QPushButton#topBtnPrimary {
            background: #2563eb;
            color: #ffffff;
            border: 1px solid #1d4ed8;
            border-radius: 7px;
            padding: 0 14px;
            font-size: 12px;
            font-weight: bold;
        }
        QPushButton#topBtnPrimary:hover  { background: #1d4ed8; }
        QPushButton#topBtnPrimary:pressed { background: #1e40af; }
        QPushButton#topBtnDanger {
            background: #ffffff;
            color: #ef4444;
            border: 1px solid #fca5a5;
            border-radius: 7px;
            padding: 0 14px;
            font-size: 12px;
        }
        QPushButton#topBtnDanger:hover {
            background: #fef2f2;
            border-color: #ef4444;
        }

        /* ── Stat cards ── */
        QFrame#statCard {
            background: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 10px;
            min-height: 100px;
        }
        QLabel#statTitle {
            background: transparent;
            color: #64748b;
            font-size: 11px;
            font-weight: bold;
        }
        QLabel#statValue {
            background: transparent;
            color: #0f172a;
            font-size: 24px;
            font-weight: bold;
        }

        /* ── Table cards ── */
        QFrame#tableCard {
            background: #ffffff;
            border: 1px solid #e2e8f0;
            border-radius: 10px;
        }
        QWidget#tableCardHeader {
            background: #ffffff;
            border-bottom: 1px solid #f1f5f9;
            border-top-left-radius: 10px;
            border-top-right-radius: 10px;
        }
        QLabel#tableCardTitle {
            background: transparent;
            color: #0f172a;
            font-size: 13px;
            font-weight: bold;
        }
        QLabel#tableCardHint {
            background: transparent;
            color: #94a3b8;
            font-size: 11px;
        }

        /* ── Tables ── */
        QTableWidget#dataTable {
            background: #ffffff;
            border: none;
            alternate-background-color: #f8fafc;
            selection-background-color: #dbeafe;
            selection-color: #1e3a5f;
            gridline-color: transparent;
            font-size: 12px;
            color: #374151;
            border-bottom-left-radius: 10px;
            border-bottom-right-radius: 10px;
        }
        QTableWidget#dataTable::item {
            padding: 8px 14px;
            border-bottom: 1px solid #f1f5f9;
            color: #374151;
        }
        QTableWidget#dataTable::item:selected {
            background: #dbeafe;
            color: #1e3a5f;
        }
        QHeaderView::section {
            background: #f8fafc;
            color: #64748b;
            padding: 9px 14px;
            border: none;
            border-bottom: 1px solid #e2e8f0;
            border-right: 1px solid #f1f5f9;
            font-size: 11px;
            font-weight: bold;
        }
        QTableCornerButton::section {
            background: #f8fafc;
            border: none;
            border-bottom: 1px solid #e2e8f0;
        }

        /* ── Status ── */
        QLabel#statusLabel {
            background: transparent;
            color: #94a3b8;
            font-size: 11px;
        }

        /* ── Dialogs ── */
        QDialog {
            background: #ffffff;
        }
        QDialog QLabel {
            background: transparent;
            color: #374151;
            font-size: 13px;
        }
        QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
            background: #ffffff;
            border: 1px solid #d1d5db;
            border-radius: 6px;
            padding: 6px 10px;
            color: #0f172a;
            font-size: 13px;
            min-height: 32px;
        }
        QLineEdit:focus, QSpinBox:focus,
        QDoubleSpinBox:focus, QComboBox:focus {
            border: 1px solid #2563eb;
        }
        QSpinBox::up-button, QSpinBox::down-button,
        QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
            background: #f8fafc;
            border: none;
            border-left: 1px solid #e2e8f0;
            width: 20px;
        }
        QSpinBox::up-button:hover, QDoubleSpinBox::up-button:hover,
        QSpinBox::down-button:hover, QDoubleSpinBox::down-button:hover {
            background: #e2e8f0;
        }
        QComboBox::drop-down {
            border: none;
            width: 24px;
        }
        QComboBox QAbstractItemView {
            background: #ffffff;
            border: 1px solid #e2e8f0;
            selection-background-color: #dbeafe;
            selection-color: #1e3a5f;
            font-size: 13px;
        }
        QDialogButtonBox QPushButton {
            background: #2563eb;
            color: #ffffff;
            border: none;
            border-radius: 6px;
            padding: 7px 20px;
            font-size: 13px;
            font-weight: bold;
            min-width: 80px;
        }
        QDialogButtonBox QPushButton:hover  { background: #1d4ed8; }
        QDialogButtonBox QPushButton:pressed { background: #1e40af; }
        QDialogButtonBox QPushButton[text="Cancel"] {
            background: #ffffff;
            color: #374151;
            border: 1px solid #d1d5db;
        }
        QDialogButtonBox QPushButton[text="Cancel"]:hover {
            background: #f8fafc;
        }

        /* ── Message boxes ── */
        QMessageBox {
            background: #ffffff;
        }
        QMessageBox QLabel {
            background: transparent;
            color: #0f172a;
            font-size: 13px;
        }
        QMessageBox QPushButton {
            background: #2563eb;
            color: #ffffff;
            border: none;
            border-radius: 6px;
            padding: 6px 18px;
            font-size: 13px;
            min-width: 70px;
        }
        QMessageBox QPushButton:hover { background: #1d4ed8; }

    )");

    // ── Connections ──────────────────────────────────
    connect(refreshBtn,     &QPushButton::clicked, this, &AdminDashboard::reloadData);
    connect(topRefreshBtn,  &QPushButton::clicked, this, &AdminDashboard::reloadData);
    connect(addProductBtn,  &QPushButton::clicked, this, &AdminDashboard::openAddProductDialog);
    connect(topAddProductBtn,&QPushButton::clicked,this, &AdminDashboard::openAddProductDialog);
    connect(sellProductBtn, &QPushButton::clicked, this, &AdminDashboard::openSellProductDialog);
    connect(topSellBtn,     &QPushButton::clicked, this, &AdminDashboard::openSellProductDialog);
    connect(updateStockBtn, &QPushButton::clicked, this, &AdminDashboard::openUpdateStockDialog);
    connect(addUserBtn,     &QPushButton::clicked, this, &AdminDashboard::openAddUserDialog);
    connect(removeUserBtn,  &QPushButton::clicked, this, &AdminDashboard::openRemoveUserDialog);
    connect(logoutBtn,      &QPushButton::clicked, this, &QWidget::close);
    connect(topLogoutBtn,   &QPushButton::clicked, this, &QWidget::close);
    connect(topUsersBtn,    &QPushButton::clicked, this, [this]() { usersTable_->setFocus(); });
    connect(productsBtn,    &QPushButton::clicked, this, [this]() { productsTable_->setFocus(); });
    connect(salesBtn,       &QPushButton::clicked, this, [this]() { salesTable_->setFocus(); });
    connect(usersBtn,       &QPushButton::clicked, this, [this]() { usersTable_->setFocus(); });
    connect(dashBtn,        &QPushButton::clicked, this, &AdminDashboard::reloadData);

    reloadData();
}

// ── The rest of your methods stay exactly the same ──

void AdminDashboard::showError(const QString& message)
{
    QMessageBox::critical(this, "Backend Error", message);
    if (statusLabel_ != nullptr)
        statusLabel_->setText(message);
}

void AdminDashboard::reloadData()
{
    DashboardMetrics metrics;
    QString error;

    if (!backend_.loadMetrics(&metrics, &error)) { showError(error); return; }
    auto products = backend_.loadProducts(&error);
    if (!error.isEmpty()) { showError(error); return; }
    auto sales = backend_.loadRecentSales(10, &error);
    if (!error.isEmpty()) { showError(error); return; }
    auto users = backend_.loadUsers(&error);
    if (!error.isEmpty()) { showError(error); return; }

    totalProductsValue_->setText(QString::number(metrics.totalProducts));
    lowStockValue_->setText(QString::number(metrics.lowStockProducts));
    salesValue_->setText(QString::number(metrics.totalSales));
    usersValue_->setText(QString::number(metrics.totalUsers));
    revenueValue_->setText(QStringLiteral("Ksh %1").arg(metrics.revenue, 0, 'f', 2));

    productsTable_->setRowCount(products.size());
    for (int row = 0; row < products.size(); ++row) {
        const auto& item = products[row];
        productsTable_->setItem(row, 0, makeItem(QString::number(item.id)));
        productsTable_->setItem(row, 1, makeItem(item.name));
        productsTable_->setItem(row, 2, makeItem(QStringLiteral("Ksh %1").arg(item.price, 0, 'f', 2)));
        productsTable_->setItem(row, 3, makeItem(QString::number(item.stock)));
    }

    salesTable_->setRowCount(sales.size());
    for (int row = 0; row < sales.size(); ++row) {
        const auto& item = sales[row];
        salesTable_->setItem(row, 0, makeItem(QString::number(item.invoiceNo)));
        salesTable_->setItem(row, 1, makeItem(item.productName));
        salesTable_->setItem(row, 2, makeItem(QString::number(item.quantity)));
        salesTable_->setItem(row, 3, makeItem(QStringLiteral("Ksh %1").arg(item.total, 0, 'f', 2)));
        salesTable_->setItem(row, 4, makeItem(item.createdAt));
    }

    usersTable_->setRowCount(users.size());
    for (int row = 0; row < users.size(); ++row) {
        const auto& item = users[row];
        usersTable_->setItem(row, 0, makeItem(QString::number(item.id)));
        usersTable_->setItem(row, 1, makeItem(item.username));
        usersTable_->setItem(row, 2, makeItem(item.role));
    }

    if (statusLabel_)
        statusLabel_->setText(QString("Loaded %1 products, %2 sales, %3 users.")
            .arg(products.size()).arg(sales.size()).arg(users.size()));
}

void AdminDashboard::openAddProductDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Add Product");
    dialog.setFixedWidth(360);
    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(14);
    auto* form = new QFormLayout();
    form->setSpacing(10);
    auto* idBox    = new QSpinBox(&dialog);    idBox->setRange(1, 1000000);
    auto* nameEdit = new QLineEdit(&dialog);
    auto* priceBox = new QDoubleSpinBox(&dialog); priceBox->setRange(0, 1000000); priceBox->setDecimals(2);
    auto* stockBox = new QSpinBox(&dialog);    stockBox->setRange(0, 1000000);
    form->addRow("Product ID", idBox);
    form->addRow("Name",       nameEdit);
    form->addRow("Price",      priceBox);
    form->addRow("Stock",      stockBox);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addLayout(form);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;
    QString error;
    if (!backend_.addProduct(idBox->value(), nameEdit->text(), priceBox->value(), stockBox->value(), &error))
        showError(error);
    else reloadData();
}

void AdminDashboard::openSellProductDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Sell Product");
    dialog.setFixedWidth(320);
    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(14);
    auto* form = new QFormLayout(); form->setSpacing(10);
    auto* idBox  = new QSpinBox(&dialog); idBox->setRange(1, 1000000);
    auto* qtyBox = new QSpinBox(&dialog); qtyBox->setRange(1, 1000000);
    form->addRow("Product ID", idBox);
    form->addRow("Quantity",   qtyBox);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addLayout(form);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;
    QString error;
    if (!backend_.sellProduct(idBox->value(), qtyBox->value(), &error))
        showError(error);
    else reloadData();
}

void AdminDashboard::openUpdateStockDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Update Stock");
    dialog.setFixedWidth(320);
    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(14);
    auto* form = new QFormLayout(); form->setSpacing(10);
    auto* idBox    = new QSpinBox(&dialog); idBox->setRange(1, 1000000);
    auto* stockBox = new QSpinBox(&dialog); stockBox->setRange(0, 1000000);
    form->addRow("Product ID", idBox);
    form->addRow("New Stock",  stockBox);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addLayout(form);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;
    QString error;
    if (!backend_.updateStock(idBox->value(), stockBox->value(), &error))
        showError(error);
    else reloadData();
}

void AdminDashboard::openAddUserDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Add User");
    dialog.setFixedWidth(340);
    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(14);
    auto* form = new QFormLayout(); form->setSpacing(10);
    auto* usernameEdit = new QLineEdit(&dialog);
    auto* passwordEdit = new QLineEdit(&dialog); passwordEdit->setEchoMode(QLineEdit::Password);
    auto* roleBox = new QComboBox(&dialog); roleBox->addItems({"admin", "cashier"});
    form->addRow("Username", usernameEdit);
    form->addRow("Password", passwordEdit);
    form->addRow("Role",     roleBox);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addLayout(form);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;
    QString error;
    if (!backend_.addUser(usernameEdit->text(), passwordEdit->text(), roleBox->currentText(), &error))
        showError(error);
    else reloadData();
}

void AdminDashboard::openRemoveUserDialog()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Remove User");
    dialog.setFixedWidth(320);
    auto* layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(14);
    auto* form = new QFormLayout(); form->setSpacing(10);
    auto* usernameEdit = new QLineEdit(&dialog);
    form->addRow("Username", usernameEdit);
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    layout->addLayout(form);
    layout->addWidget(buttons);
    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() != QDialog::Accepted) return;
    QString error;
    if (!backend_.removeUser(usernameEdit->text(), &error))
        showError(error);
    else reloadData();
}