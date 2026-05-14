#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "admindashboard.h"

#include <QColor>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("POS System");
    resize(1140, 700);

    QWidget *root = new QWidget(this);
    root->setObjectName("root");
    setCentralWidget(root);

    auto *shell = new QHBoxLayout(root);
    shell->setContentsMargins(0, 0, 0, 0);
    shell->setSpacing(0);


    auto *left = new QFrame(root);
    left->setObjectName("leftPanel");
    left->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto *leftLayout = new QVBoxLayout(left);
    leftLayout->setContentsMargins(44, 44, 44, 44);
    leftLayout->setSpacing(0);

    auto *logoRow = new QHBoxLayout();
    auto *logoMark = new QLabel("▣", left);
    logoMark->setObjectName("logoMark");
    auto *logoName = new QLabel("POS SYSTEM", left);
    logoName->setObjectName("logoName");
    logoRow->addWidget(logoMark);
    logoRow->addWidget(logoName);
    logoRow->addStretch();

    auto *eyebrow = new QLabel("// RETAIL CONTROL", left);
    eyebrow->setObjectName("heroEyebrow");

    auto *heroTitle = new QLabel("Built for\nspeed &\nprecision.", left);
    heroTitle->setObjectName("heroTitle");
    heroTitle->setWordWrap(true);

    auto *heroCopy = new QLabel(
        "Manage inventory, process sales, and track\n"
        "revenue — all from a single dashboard\n"
        "built for your team.",
        left
    );
    heroCopy->setObjectName("heroCopy");
    heroCopy->setWordWrap(true);

    auto *statsRow = new QHBoxLayout();
    statsRow->setSpacing(28);

    auto makeStatWidget = [&](const QString &num, const QString &label) -> QWidget* {
        auto *w = new QWidget(left);
        auto *l = new QVBoxLayout(w);
        l->setContentsMargins(0, 0, 0, 0);
        l->setSpacing(2);
        auto *n = new QLabel(num, w);
        n->setObjectName("statNum");
        auto *lb = new QLabel(label, w);
        lb->setObjectName("statLabel");
        l->addWidget(n);
        l->addWidget(lb);
        return w;
    };

    auto makeDivider = [&]() -> QFrame* {
        auto *d = new QFrame(left);
        d->setObjectName("statDivider");
        d->setFixedWidth(2);
        d->setFixedHeight(36);
        return d;
    };

    statsRow->addWidget(makeDivider());
    statsRow->addWidget(makeStatWidget("2", "User roles"));
    statsRow->addWidget(makeDivider());
    statsRow->addWidget(makeStatWidget("1 DB", "PostgreSQL"));
    statsRow->addWidget(makeDivider());
    statsRow->addWidget(makeStatWidget("∞", "Products"));
    statsRow->addStretch();

    auto *leftFooter = new QHBoxLayout();
    auto *version = new QLabel("v1.0.0", left);
    version->setObjectName("version");
    leftFooter->addWidget(version);
    leftFooter->addStretch();

    auto *spacer1 = new QSpacerItem(0, 32, QSizePolicy::Minimum, QSizePolicy::Fixed);
    auto *spacer2 = new QSpacerItem(0, 28, QSizePolicy::Minimum, QSizePolicy::Fixed);
    auto *spacer3 = new QSpacerItem(0, 24, QSizePolicy::Minimum, QSizePolicy::Fixed);
    auto *flexSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

    leftLayout->addLayout(logoRow);
    leftLayout->addItem(flexSpacer);
    leftLayout->addWidget(eyebrow);
    leftLayout->addItem(spacer1);
    leftLayout->addWidget(heroTitle);
    leftLayout->addItem(spacer2);
    leftLayout->addWidget(heroCopy);
    leftLayout->addItem(spacer3);
    leftLayout->addLayout(statsRow);
    leftLayout->addSpacing(44);
    leftLayout->addLayout(leftFooter);

    auto *right = new QFrame(root);
    right->setObjectName("rightPanel");
    right->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    auto *rightLayout = new QVBoxLayout(right);
    rightLayout->setContentsMargins(56, 0, 56, 0);
    rightLayout->setSpacing(0);
    rightLayout->setAlignment(Qt::AlignVCenter);

    auto *formEyebrow = new QLabel("SECURE ACCESS", right);
    formEyebrow->setObjectName("formEyebrow");

    auto *formTitle = new QLabel("Welcome back.", right);
    formTitle->setObjectName("formTitle");

    auto *formSub = new QLabel(
        "Sign in with your credentials to access the dashboard.",
        right
    );
    formSub->setObjectName("formSub");
    formSub->setWordWrap(true);

    auto *usernameLabel = new QLabel("USERNAME", right);
    usernameLabel->setObjectName("fieldLabel");

    auto *usernameBox = new QFrame(right);
    usernameBox->setObjectName("fieldBox");
    auto *usernameRow = new QHBoxLayout(usernameBox);
    usernameRow->setContentsMargins(16, 0, 16, 0);
    usernameRow->setSpacing(12);
    auto *uIcon = new QLabel("👤", usernameBox);
    uIcon->setObjectName("fieldIcon");
    auto *usernameInput = new QLineEdit(usernameBox);
    usernameInput->setObjectName("fieldInput");
    usernameInput->setPlaceholderText("e.g. admin");
    usernameInput->setClearButtonEnabled(true);
    usernameInput->setFrame(false);
    usernameRow->addWidget(uIcon);
    usernameRow->addWidget(usernameInput);


    auto *passwordLabel = new QLabel("PASSWORD", right);
    passwordLabel->setObjectName("fieldLabel");

    auto *passwordBox = new QFrame(right);
    passwordBox->setObjectName("fieldBox");
    auto *passwordRow = new QHBoxLayout(passwordBox);
    passwordRow->setContentsMargins(16, 0, 16, 0);
    passwordRow->setSpacing(12);
    auto *pIcon = new QLabel("🔒", passwordBox);
    pIcon->setObjectName("fieldIcon");
    auto *passwordInput = new QLineEdit(passwordBox);
    passwordInput->setObjectName("fieldInput");
    passwordInput->setPlaceholderText("••••••••");
    passwordInput->setEchoMode(QLineEdit::Password);
    passwordInput->setClearButtonEnabled(true);
    passwordInput->setFrame(false);
    passwordRow->addWidget(pIcon);
    passwordRow->addWidget(passwordInput);

    auto *feedback = new QLabel(right);
    feedback->setObjectName("feedback");
    feedback->setMinimumHeight(20);
    feedback->setAlignment(Qt::AlignCenter);

    auto *loginButton = new QPushButton("→  SIGN IN", right);
    loginButton->setObjectName("loginButton");
    loginButton->setCursor(Qt::PointingHandCursor);
    loginButton->setFixedHeight(52);

    auto *rolesGrid = new QHBoxLayout();
    rolesGrid->setSpacing(10);

    auto makeRoleCard = [&](const QString& icon, const QString& name,
                             const QString& desc, const QString& obj) -> QFrame* {
        auto *card = new QFrame(right);
        card->setObjectName(obj);
        auto *row = new QHBoxLayout(card);
        row->setContentsMargins(14, 12, 14, 12);
        row->setSpacing(12);
        auto *ic = new QLabel(icon, card);
        ic->setObjectName(obj + "Icon");
        ic->setFixedSize(36, 36);
        ic->setAlignment(Qt::AlignCenter);
        auto *info = new QWidget(card);
        auto *il = new QVBoxLayout(info);
        il->setContentsMargins(0, 0, 0, 0);
        il->setSpacing(2);
        auto *nm = new QLabel(name, card);
        nm->setObjectName("roleName");
        auto *ds = new QLabel(desc, card);
        ds->setObjectName("roleDesc");
        il->addWidget(nm);
        il->addWidget(ds);
        row->addWidget(ic);
        row->addWidget(info);
        return card;
    };

    rolesGrid->addWidget(makeRoleCard("🛡", "Admin", "Full access", "adminCard"));
    rolesGrid->addWidget(makeRoleCard("🖥", "Cashier", "Sales only", "cashierCard"));

    rightLayout->addSpacing(52);
    rightLayout->addWidget(formEyebrow);
    rightLayout->addSpacing(10);
    rightLayout->addWidget(formTitle);
    rightLayout->addSpacing(6);
    rightLayout->addWidget(formSub);
    rightLayout->addSpacing(32);
    rightLayout->addWidget(usernameLabel);
    rightLayout->addSpacing(8);
    rightLayout->addWidget(usernameBox);
    rightLayout->addSpacing(18);
    rightLayout->addWidget(passwordLabel);
    rightLayout->addSpacing(8);
    rightLayout->addWidget(passwordBox);
    rightLayout->addSpacing(10);
    rightLayout->addWidget(feedback);
    rightLayout->addSpacing(4);
    rightLayout->addWidget(loginButton);
    rightLayout->addSpacing(20);
    rightLayout->addLayout(rolesGrid);
    rightLayout->addSpacing(52);

    shell->addWidget(left, 45);
    shell->addWidget(right, 55);

    setStyleSheet(R"(
        QMainWindow { background: #0b0f1a; }
        QWidget#root { background: transparent; }

        /* Left panel */
        QFrame#leftPanel {
            background: #0b0f1a;
            border-right: 1px solid rgba(255,255,255,0.08);
        }
        QLabel#logoMark {
            color: #00e5a0;
            font-size: 22px;
        }
        QLabel#logoName {
            color: #ffffff;
            font-size: 12px;
            font-weight: 700;
            letter-spacing: 3px;
        }
        QLabel#heroEyebrow {
            color: #00e5a0;
            font-size: 11px;
            font-family: monospace;
            letter-spacing: 2px;
        }
        QLabel#heroTitle {
            color: #ffffff;
            font-size: 44px;
            font-weight: 800;
            line-height: 1.05;
        }
        QLabel#heroCopy {
            color: rgba(255,255,255,0.42);
            font-size: 13px;
            line-height: 1.7;
        }
        QFrame#statDivider {
            background: #00e5a0;
            border: none;
        }
        QLabel#statNum {
            color: #ffffff;
            font-size: 22px;
            font-weight: 700;
        }
        QLabel#statLabel {
            color: rgba(255,255,255,0.38);
            font-size: 11px;
        }
        QLabel#version {
            color: rgba(255,255,255,0.18);
            font-size: 10px;
            font-family: monospace;
            letter-spacing: 2px;
        }

        /* Right panel */
        QFrame#rightPanel {
            background: #f7f8fc;
        }
        QLabel#formEyebrow {
            color: green;
            font-size: 10px;
            font-family: monospace;
            letter-spacing: 3px;
        }
        QLabel#formTitle {
            color: #0b0f1a;
            font-size: 36px;
            font-weight: 800;
        }
        QLabel#formSub {
            color: #64748b;
            font-size: 13px;
            line-height: 1.6;
        }
        QLabel#fieldLabel {
            color: #64748b;
            font-size: 11px;
            font-weight: 600;
            letter-spacing: 1.5px;
        }
        QFrame#fieldBox {
            background: #ffffff;
            border: 1.5px solid #e2e8f0;
            border-radius: 12px;
            min-height: 50px;
            max-height: 50px;
        }
        QFrame#fieldBox:focus-within {
            border: 1.5px solid #0066ff;
        }
        QLabel#fieldIcon {
            color: #94a3b8;
            font-size: 16px;
        }
        QLineEdit#fieldInput {
            background: transparent;
            border: none;
            color: #0b0f1a;
            font-size: 14px;
            selection-background-color: #bfdbfe;
        }
        QLabel#feedback {
            color: #ef4444;
            font-size: 12px;
        }
        QPushButton#loginButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00e5a0, stop:1 #0066ff);
            color: #ffffff;
            border: none;
            border-radius: 12px;
            font-size: 14px;
            font-weight: 700;
            letter-spacing: 2px;
        }
        QPushButton#loginButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                stop:0 #00c98a, stop:1 #0052cc);
        }
        QPushButton#loginButton:pressed {
            background: #003fa3;
        }
        QFrame#adminCard, QFrame#cashierCard {
            background: #ffffff;
            border: 1.5px solid #e2e8f0;
            border-radius: 12px;
        }
        QLabel#adminCardIcon {
            background: #eff6ff;
            border-radius: 9px;
            font-size: 18px;
        }
        QLabel#cashierCardIcon {
            background: #f0fdf4;
            border-radius: 9px;
            font-size: 18px;
        }
        QLabel#roleName {
            color: #0f172a;
            font-size: 13px;
            font-weight: 600;
        }
        QLabel#roleDesc {
            color: #94a3b8;
            font-size: 11px;
        }
    )");


    connect(loginButton, &QPushButton::clicked, this,
        [this, usernameInput, passwordInput, feedback]() {
            const QString user = usernameInput->text().trimmed();
            const QString pass = passwordInput->text();

            if (user.isEmpty() || pass.isEmpty()) {
                feedback->setText("Both fields are required.");
                return;
            }

            QString err;
            const QString role = authDb_.login(user, pass, &err);

            if (!err.isEmpty()) {
                QMessageBox::critical(this, "Database Error", err);
                return;
            }

            if (!role.isEmpty()) {
                feedback->clear();
                QMessageBox::information(this, "Access granted",
                    "Welcome, " + user + "!\nRole: " + role);

                if (role == "admin") {
                    auto* dashboard = new AdminDashboard();
                    dashboard->setAttribute(Qt::WA_DeleteOnClose);
                    connect(dashboard, &QObject::destroyed, this, [this]() {
                        this->show();
                    });
                    dashboard->show();
                    this->hide();
                }
                return;
            }

            feedback->setText("Invalid credentials. Try again.");
        }
    );

    connect(passwordInput, &QLineEdit::returnPressed,
            loginButton, &QPushButton::click);
    connect(usernameInput, &QLineEdit::returnPressed,
            passwordInput, qOverload<>(&QLineEdit::setFocus));

    usernameInput->setFocus();
}

MainWindow::~MainWindow() { delete ui; }
