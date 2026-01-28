#include "nursery.h"
#include "./ui_nursery.h"
#include "../../src/Nursery/Nursery.h"
#include "../../src/Customer/Customer.h"
#include "../../src/Customer/CustomerCreator.h"
#include "../../src/Customer/Browse.h"
#include "../../src/Customer/Enquire.h"
#include "../../src/Customer/Purchasing.h"
#include "../../src/Greenhouse/Plant.h"
#include "../../src/Greenhouse/PlantDecorator.h"
#include "../../src/Greenhouse/GiftWrap.h"
#include "../../src/Greenhouse/Pot.h"
#include "../../src/Greenhouse/SpecialArrangement.h"
#include "../../src/Staff/Staff.h"
#include "../../src/Staff/InfoDesk.h"
#include "../../src/Staff/Cashiers.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QBrush>
#include <QDateTime>
#include <QPen>
#include <QDebug>
#include <QGraphicsTextItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneHoverEvent>
#include <QScrollArea>
#include <QGraphicsProxyWidget>
#include <QWidget>
#include <cmath>
#include <random>
#include <map>

// Custom thinking bubble class that expands on hover
class ThinkingBubble : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT
private:
    QGraphicsEllipseItem *smallBubble1;
    QGraphicsEllipseItem *smallBubble2;
    QGraphicsEllipseItem *mainBubble;
    QGraphicsRectItem *infoBox;
    QGraphicsTextItem *infoText;
    bool expanded;
    NurseryWindow *window;
    Customer *customer;

public:
    ThinkingBubble(NurseryWindow *win, Customer *cust, QGraphicsItem *parent = nullptr)
        : QGraphicsItemGroup(parent), expanded(false), window(win), customer(cust)
    {
        // Don't accept hover on bubble - we'll handle it on the customer dot
        setAcceptHoverEvents(false);

        // Create small thought bubbles (the "..." effect)
        smallBubble1 = new QGraphicsEllipseItem(0, 0, 4, 4);
        smallBubble1->setBrush(QBrush(Qt::white));
        smallBubble1->setPen(QPen(Qt::black, 1));
        addToGroup(smallBubble1);

        smallBubble2 = new QGraphicsEllipseItem(6, -4, 6, 6);
        smallBubble2->setBrush(QBrush(Qt::white));
        smallBubble2->setPen(QPen(Qt::black, 1));
        addToGroup(smallBubble2);

        // Main thinking bubble
        mainBubble = new QGraphicsEllipseItem(12, -12, 15, 15);
        mainBubble->setBrush(QBrush(Qt::white));
        mainBubble->setPen(QPen(Qt::black, 1.5));
        addToGroup(mainBubble);

        // Set initial color based on customer state
        updateColor();

        // Info box (initially hidden)
        infoBox = new QGraphicsRectItem(0, 0, 200, 100);
        infoBox->setBrush(QBrush(QColor(255, 255, 220, 230)));
        infoBox->setPen(QPen(Qt::black, 2));
        infoBox->setPos(30, -120);
        infoBox->setVisible(false);
        addToGroup(infoBox);

        // Info text (initially hidden)
        infoText = new QGraphicsTextItem();
        infoText->setDefaultTextColor(Qt::black);
        infoText->setPos(35, -115);
        infoText->setVisible(false);
        infoText->setTextWidth(190);
        addToGroup(infoText);
    }

    void updateInfo(const QString &info)
    {
        infoText->setPlainText(info);

        // Adjust box size based on text
        QRectF textRect = infoText->boundingRect();
        qreal width = qMax(200.0, textRect.width() + 10);
        qreal height = qMax(100.0, textRect.height() + 10);

        infoBox->setRect(0, 0, width, height);
        infoBox->setPos(30, -height - 20);
        infoText->setPos(35, -height - 15);
    }

public:
    void showInfo()
    {
        qDebug() << "ThinkingBubble::showInfo() called";
        qDebug() << "  window:" << window << "isSimulationRunning:" << (window ? window->isSimulationRunning() : -1);

        if (window && !window->isSimulationRunning())
        {
            expanded = true;
            // Always refresh info to show current state
            QString info = window->getCustomerInfo(customer);
            qDebug() << "  Got customer info, length:" << info.length();
            qDebug() << "  Info content:" << info;
            updateInfo(info);
            infoBox->setVisible(true);
            infoText->setVisible(true);
            qDebug() << "  Info box and text set to visible";
        }
        else
        {
            qDebug() << "  Cannot show info - simulation is running or window is null";
        }
    }

    void refreshInfo()
    {
        // Update info if bubble is currently expanded
        if (expanded && window && !window->isSimulationRunning())
        {
            QString info = window->getCustomerInfo(customer);
            updateInfo(info);
        }
    }

    void hideInfo()
    {
        expanded = false;
        infoBox->setVisible(false);
        infoText->setVisible(false);
    }

    void updateColor()
    {
        QColor bubbleColor = Qt::white; // Default color

        if (!customer)
        {
            bubbleColor = Qt::white;
        }
        else if (!customer->getAction()) // Leaving
        {
            bubbleColor = QColor(255, 165, 0); // Orange
        }
        else
        {
            Action *action = customer->getAction();
            std::string actionName = action->getActionName();

            if (actionName == "Browsing")
            {
                bubbleColor = QColor(144, 238, 144); // Light green
            }
            else if (actionName == "Enquiring")
            {
                bubbleColor = QColor(173, 216, 230); // Light blue
            }
            else if (actionName == "Purchasing")
            {
                bubbleColor = QColor(255, 192, 203); // Pink
            }
        }

        // Update all bubble colors
        smallBubble1->setBrush(QBrush(bubbleColor));
        smallBubble2->setBrush(QBrush(bubbleColor));
        mainBubble->setBrush(QBrush(bubbleColor));
    }
};

// Custom ellipse that shows bubble info on hover
class CustomerDot : public QGraphicsEllipseItem
{
private:
    ThinkingBubble *bubble;

public:
    CustomerDot(qreal x, qreal y, qreal w, qreal h, ThinkingBubble *b)
        : QGraphicsEllipseItem(x, y, w, h), bubble(b)
    {
        setAcceptHoverEvents(true);
    }

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override
    {
        if (bubble)
        {
            bubble->showInfo();
        }
        QGraphicsEllipseItem::hoverEnterEvent(event);
    }

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override
    {
        if (bubble)
        {
            bubble->hideInfo();
        }
        QGraphicsEllipseItem::hoverLeaveEvent(event);
    }
};

#include "nursery.moc"

NurseryWindow::NurseryWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::NurseryWindow),
      scene(nullptr),
      view(nullptr),
      simulationTimer(nullptr),
      customerSpawnTimer(nullptr),
      seasonTimer(nullptr),
      nurseryBackend(nullptr),
      isRunning(false),
      rng(std::random_device{}())
{
    qDebug() << "Initializing Nursery Window...";

    ui->setupUi(this);
    setWindowTitle("Chaotic Plants Nursery Simulation");
    resize(1000, 700);

    setupUI();
    setupScene();

    // Initialize backend
    qDebug() << "Creating backend nursery...";
    nurseryBackend = new Nursery(nullptr);
    qDebug() << "Backend nursery created successfully";

    // Now update displays with initial data
    qDebug() << "Updating inventory display...";
    updateInventoryDisplay();
    qDebug() << "Inventory display updated";

    qDebug() << "Updating staff display...";
    updateStaffDisplay();
    qDebug() << "Staff display updated";

    // Setup timers
    qDebug() << "Setting up timers...";
    simulationTimer = new QTimer(this);
    connect(simulationTimer, &QTimer::timeout, this, &NurseryWindow::updateSimulation);

    customerSpawnTimer = new QTimer(this);
    connect(customerSpawnTimer, &QTimer::timeout, this, &NurseryWindow::addCustomer);

    // Season timer - change season every 30 seconds
    seasonTimer = new QTimer(this);
    connect(seasonTimer, &QTimer::timeout, this, &NurseryWindow::changeSeason);

    qDebug() << "Nursery Window initialized successfully!";
}

NurseryWindow::~NurseryWindow()
{
    // Clean up customer visuals
    for (auto &cv : customerVisuals)
    {
        delete cv.dot;
        if (cv.bubble)
        {
            delete cv.bubble;
        }
    }
    customerVisuals.clear();

    delete nurseryBackend;
    delete scene;
    delete ui;
}

void NurseryWindow::setupUI()
{
    // Create central widget and main layout
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    // Create control panel
    QHBoxLayout *controlLayout = new QHBoxLayout();

    startBtn = new QPushButton("Start Simulation", this);
    pauseBtn = new QPushButton("Pause", this);
    resetBtn = new QPushButton("Reset", this);
    addCustomerBtn = new QPushButton("Add Customer", this);
    changeSeasonBtn = new QPushButton("Change Season", this);
    changeSeasonBtn->setStyleSheet("background-color: #FFA500; color: white; font-weight: bold;");

    connect(startBtn, &QPushButton::clicked, this, &NurseryWindow::startSimulation);
    connect(pauseBtn, &QPushButton::clicked, this, &NurseryWindow::pauseSimulation);
    connect(resetBtn, &QPushButton::clicked, this, &NurseryWindow::resetSimulation);
    connect(addCustomerBtn, &QPushButton::clicked, this, &NurseryWindow::addCustomer);
    connect(changeSeasonBtn, &QPushButton::clicked, this, &NurseryWindow::changeSeason);

    pauseBtn->setEnabled(false);

    controlLayout->addWidget(startBtn);
    controlLayout->addWidget(pauseBtn);
    controlLayout->addWidget(resetBtn);
    controlLayout->addWidget(addCustomerBtn);
    controlLayout->addWidget(changeSeasonBtn);
    controlLayout->addStretch();

    // Create status panel
    QHBoxLayout *statusLayout = new QHBoxLayout();

    seasonLabel = new QLabel("Season: Spring", this);
    customerCountLabel = new QLabel("Customers: 0", this);
    statusLabel = new QLabel("Status: Ready", this);

    seasonLabel->setStyleSheet("font-weight: bold; font-size: 14pt; color: #2E7D32;");
    customerCountLabel->setStyleSheet("font-weight: bold; font-size: 12pt;");
    statusLabel->setStyleSheet("font-weight: bold; font-size: 12pt;");

    statusLayout->addWidget(customerCountLabel);
    statusLayout->addWidget(statusLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(seasonLabel);

    // Add layouts to main layout
    mainLayout->addLayout(controlLayout);
    mainLayout->addLayout(statusLayout);

    setCentralWidget(centralWidget);
}

void NurseryWindow::setupScene()
{
    // Create graphics scene
    scene = new QGraphicsScene(this);
    scene->setSceneRect(0, 0, SCENE_WIDTH, SCENE_HEIGHT);
    scene->setBackgroundBrush(QBrush(QColor(240, 255, 240))); // Light green background

    // Create graphics view
    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setMinimumSize(SCENE_WIDTH + 20, SCENE_HEIGHT + 20);

    // Add view to central widget layout
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(centralWidget()->layout());
    if (layout)
    {
        layout->addWidget(view);
    }

    // Add some visual elements to represent the nursery layout
    // Draw entrance area (moved 50px left)
    QGraphicsRectItem *entrance = scene->addRect(-40, SCENE_HEIGHT - 60, 100, 50,
                                                 QPen(Qt::darkGreen, 2),
                                                 QBrush(QColor(144, 238, 144)));
    QGraphicsTextItem *entranceLabel = scene->addText("Entrance");
    entranceLabel->setPos(-25, SCENE_HEIGHT - 50);

    // Old queue block removed - no longer needed

    // Draw exit area (same size as entrance, moved 30px right)
    QGraphicsRectItem *checkout = scene->addRect(SCENE_WIDTH - 90, SCENE_HEIGHT - 60, 100, 50,
                                                 QPen(Qt::darkRed, 2),
                                                 QBrush(QColor(255, 218, 185)));
    QGraphicsTextItem *checkoutLabel = scene->addText("Exit");
    checkoutLabel->setPos(SCENE_WIDTH - 65, SCENE_HEIGHT - 50);

    // Create Info Desk visualization box (between staff and inventory, moved 60px more to right)
    int infoDeskBoxWidth = 150;
    int infoDeskBoxHeight = 100;
    int infoDeskBoxX = -40 + 200 + 90; // staffBoxX + staffBoxWidth + 90px spacing
    int infoDeskBoxY = 40;

    infoDeskBox = scene->addRect(infoDeskBoxX, infoDeskBoxY, infoDeskBoxWidth, infoDeskBoxHeight,
                                 QPen(Qt::darkBlue, 2), QBrush(QColor(173, 216, 230)));
    QGraphicsTextItem *infoDeskBoxLabel = scene->addText("Info Desk");
    infoDeskBoxLabel->setPos(infoDeskBoxX + 35, infoDeskBoxY + 40);
    infoDeskBoxLabel->setDefaultTextColor(Qt::darkBlue);
    QFont infoDeskFont = infoDeskBoxLabel->font();
    infoDeskFont.setPointSize(12);
    infoDeskFont.setBold(true);
    infoDeskBoxLabel->setFont(infoDeskFont);

    // Draw Inventory box under season label (right side, main white area)
    int invBoxWidth = 220;
    int invBoxHeight = 300;                       // Increased height for more content
    int invBoxX = SCENE_WIDTH - invBoxWidth + 70; // Moved 70px to the right (10px more)
    int invBoxY = 35;                             // Moved 25px up from 60 (5px more up)

    // Create background box
    inventoryBox = scene->addRect(invBoxX, invBoxY, invBoxWidth, invBoxHeight,
                                  QPen(Qt::darkGray, 2), QBrush(Qt::white));

    // Create scrollable content area
    inventoryScrollArea = new QScrollArea();
    inventoryScrollArea->setFixedSize(invBoxWidth - 4, invBoxHeight - 4);
    inventoryScrollArea->setStyleSheet("QScrollArea { background-color: white; border: none; }");

    // Create content label for inventory text
    inventoryContentLabel = new QLabel();
    inventoryContentLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    inventoryContentLabel->setMargin(5);
    inventoryContentLabel->setWordWrap(true);
    inventoryContentLabel->setStyleSheet("QLabel { background-color: white; color: black; font-size: 15px; }");

    // Set up scroll area with content
    inventoryScrollArea->setWidget(inventoryContentLabel);
    inventoryScrollArea->setWidgetResizable(true);
    inventoryScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    inventoryScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Add scroll area to scene via proxy widget
    inventoryProxy = scene->addWidget(inventoryScrollArea);
    inventoryProxy->setPos(invBoxX + 2, invBoxY + 2);

    // Add title
    QGraphicsTextItem *invTitle = scene->addText("Inventory");
    invTitle->setDefaultTextColor(Qt::darkBlue);
    invTitle->setFont(QFont("Arial", 12, QFont::Bold));
    invTitle->setPos(invBoxX + 10, invBoxY - 25);

    // Draw Staff Status box on left side
    int staffBoxWidth = 200;
    int staffBoxHeight = 200;
    int staffBoxX = 10;
    int staffBoxY = 40;

    // Create background box for staff
    staffBox = scene->addRect(staffBoxX, staffBoxY, staffBoxWidth, staffBoxHeight,
                              QPen(Qt::darkGray, 2), QBrush(QColor(255, 250, 205)));

    // Create scrollable content area for staff
    staffScrollArea = new QScrollArea();
    staffScrollArea->setFixedSize(staffBoxWidth - 4, staffBoxHeight - 4);
    staffScrollArea->setStyleSheet("QScrollArea { background-color: #FFFACD; border: none; }");

    // Create content label for staff text
    staffContentLabel = new QLabel();
    staffContentLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    staffContentLabel->setMargin(5);
    staffContentLabel->setWordWrap(true);
    staffContentLabel->setStyleSheet("QLabel { background-color: #FFFACD; color: black; font-size: 11px; }");

    // Set up scroll area with content
    staffScrollArea->setWidget(staffContentLabel);
    staffScrollArea->setWidgetResizable(true);
    staffScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    staffScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Add scroll area to scene via proxy widget
    staffProxy = scene->addWidget(staffScrollArea);
    staffProxy->setPos(staffBoxX + 2, staffBoxY + 2);

    // Add title
    QGraphicsTextItem *staffTitle = scene->addText("Staff Status");
    staffTitle->setDefaultTextColor(Qt::darkGreen);
    staffTitle->setFont(QFont("Arial", 12, QFont::Bold));
    staffTitle->setPos(staffBoxX + 10, staffBoxY - 25);

    // Create Cashier block (7px left of exit)
    int cashierBoxWidth = 100;
    int cashierBoxHeight = 50;
    int cashierBoxX = SCENE_WIDTH - 90 - cashierBoxWidth - 7; // 7px left of exit
    int cashierBoxY = SCENE_HEIGHT - 60;

    cashierBox = scene->addRect(cashierBoxX, cashierBoxY, cashierBoxWidth, cashierBoxHeight,
                                QPen(Qt::darkMagenta, 2), QBrush(QColor(255, 192, 203)));
    QGraphicsTextItem *cashierBoxLabel = scene->addText("Cashier");
    cashierBoxLabel->setPos(cashierBoxX + 25, cashierBoxY + 15);
    cashierBoxLabel->setDefaultTextColor(Qt::darkMagenta);
    QFont cashierFont = cashierBoxLabel->font();
    cashierFont.setPointSize(11);
    cashierFont.setBold(true);
    cashierBoxLabel->setFont(cashierFont);

    // Create Cashier Queue display area (left of cashier box)
    int queueBoxWidth = cashierBoxWidth;
    int queueBoxHeight = 100;
    int queueBoxX = cashierBoxX - queueBoxWidth - 5; // 5px left of cashier
    int queueBoxY = cashierBoxY;

    cashierQueueBox = scene->addRect(queueBoxX, queueBoxY, queueBoxWidth, queueBoxHeight,
                                     QPen(Qt::gray, 1), QBrush(QColor(255, 255, 255, 200)));

    // Create scrollable area for queue
    cashierQueueScrollArea = new QScrollArea();
    cashierQueueScrollArea->setFixedSize(queueBoxWidth - 4, queueBoxHeight - 4);
    cashierQueueScrollArea->setStyleSheet("QScrollArea { background-color: white; border: none; }");

    cashierQueueLabel = new QLabel();
    cashierQueueLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    cashierQueueLabel->setMargin(3);
    cashierQueueLabel->setWordWrap(true);
    cashierQueueLabel->setStyleSheet("QLabel { background-color: white; color: black; font-size: 10px; }");
    cashierQueueLabel->setText("Queue: Empty");

    cashierQueueScrollArea->setWidget(cashierQueueLabel);
    cashierQueueScrollArea->setWidgetResizable(true);

    cashierQueueProxy = scene->addWidget(cashierQueueScrollArea);
    cashierQueueProxy->setPos(queueBoxX + 2, queueBoxY + 2);

    QGraphicsTextItem *queueTitle = scene->addText("Cashier Queue");
    queueTitle->setDefaultTextColor(Qt::darkGray);
    QFont queueFont = queueTitle->font();
    queueFont.setPointSize(9);
    queueFont.setBold(true);
    queueTitle->setFont(queueFont);
    queueTitle->setPos(queueBoxX + 5, queueBoxY - 20);

    // Note: updateInventoryDisplay() and updateStaffDisplay() will be called after nurseryBackend is initialized
}

void NurseryWindow::updateInventoryDisplay()
{
    if (!nurseryBackend || !inventoryContentLabel)
        return;
    Inventory *inv = nurseryBackend->getInventory();
    if (!inv)
        return;
    auto &map = inv->getInventory();
    QString text;

    // Group plants by type for better organization
    QStringList flowers, herbs, trees, succulents;

    for (const auto &pair : map)
    {
        const std::string &name = pair.first;
        const auto &plantPtr = pair.second.first;
        int qty = pair.second.second;
        QString plantLine = QString::fromStdString(name) + ": " + QString::number(qty);

        // Categorize plants based on their actual type from the plant object
        if (plantPtr)
        {
            std::string plantType = plantPtr->getType();
            if (plantType == "Flower")
            {
                flowers << plantLine;
            }
            else if (plantType == "Herb")
            {
                herbs << plantLine;
            }
            else if (plantType == "Tree")
            {
                trees << plantLine;
            }
            else if (plantType == "Succulent")
            {
                succulents << plantLine;
            }
        }
    }

    // Build organized display text
    if (!flowers.isEmpty())
    {
        text += "<b>🌸 Flowers:</b><br>";
        text += flowers.join("<br>") + "<br><br>";
    }
    if (!herbs.isEmpty())
    {
        text += "<b>🌿 Herbs:</b><br>";
        text += herbs.join("<br>") + "<br><br>";
    }
    if (!trees.isEmpty())
    {
        text += "<b>🌳 Trees:</b><br>";
        text += trees.join("<br>") + "<br><br>";
    }
    if (!succulents.isEmpty())
    {
        text += "<b>🌵 Succulents:</b><br>";
        text += succulents.join("<br>");
    }

    inventoryContentLabel->setText(text.trimmed());
}

void NurseryWindow::updateStaffDisplay()
{
    if (!nurseryBackend || !staffContentLabel)
        return;

    InfoDesk *desk = nurseryBackend->getInfoDesk();
    if (!desk)
        return;

    try
    {
        std::vector<Staff *> allStaff = desk->getAllStaff();
        QString text;

        for (Staff *staff : allStaff)
        {
            if (!staff)
                continue;

            std::string nameStr = staff->getName();
            std::string typeStr = staff->getStaffType();
            // Prefer runtime availability for display to avoid transient state-toggle issues
            std::string stateStr = staff->getAvailability() ? std::string("Available") : std::string("Busy");

            QString name = QString::fromStdString(nameStr);
            QString type = QString::fromStdString(typeStr);
            QString state = QString::fromStdString(stateStr);
            bool available = staff->getAvailability();

            // Color code by availability
            QString color = available ? "green" : "red";
            QString statusIcon = available ? "✓" : "✗";

            text += QString("<b>%1</b> (%2)<br>")
                        .arg(name)
                        .arg(type);
            text += QString("<span style='color:%1;'>%2 %3</span><br>")
                        .arg(color)
                        .arg(statusIcon)
                        .arg(state);

            // Show current customer if any
            Customer *currentCust = staff->getCurrentCustomer();
            if (currentCust)
            {
                text += QString("<i>→ Customer #%1</i><br>")
                            .arg(currentCust->getId());
            }

            text += "<br>";
        }

        // Show cashier queue if exists
        Cashiers *cashier = nurseryBackend->getCashier();
        if (cashier && cashier->getQueueSize() > 0)
        {
            text += QString("<b style='color:darkred;'>Cashier Queue: %1</b><br>")
                        .arg(cashier->getQueueSize());
        }

        staffContentLabel->setText(text.trimmed());
    }
    catch (const std::exception &e)
    {
        qDebug() << "Error in updateStaffDisplay:" << e.what();
        staffContentLabel->setText("Error loading staff data");
    }
    catch (...)
    {
        qDebug() << "Unknown error in updateStaffDisplay";
        staffContentLabel->setText("Error loading staff data");
    }
}

void NurseryWindow::updateCashierQueue()
{
    if (!cashierQueueLabel)
        return;

    // Build list of customers who are purchasing (in queue or being served)
    std::vector<Customer *> purchasingCustomers;

    for (const auto &cv : customerVisuals)
    {
        if (cv.customer && cv.customer->getAction())
        {
            Action *action = cv.customer->getAction();
            if (action->getActionName() == "Purchasing")
            {
                const std::vector<Plant *> &basket = cv.customer->getBasket();
                if (!basket.empty())
                {
                    purchasingCustomers.push_back(cv.customer);
                }
            }
        }
    }

    // Build display text
    QString text;
    if (purchasingCustomers.empty())
    {
        text = "Cashier Queue: Empty";
    }
    else
    {
        text = QString("Cashier Queue (%1):\n\n").arg(purchasingCustomers.size());
        for (size_t i = 0; i < purchasingCustomers.size() && i < 10; ++i) // Show max 10
        {
            Customer *c = purchasingCustomers[i];
            const std::vector<Plant *> &basket = c->getBasket();
            text += QString("Customer #%1 (%2 items)\n").arg(c->getId()).arg(basket.size());
        }
        if (purchasingCustomers.size() > 10)
        {
            text += QString("... +%1 more").arg(purchasingCustomers.size() - 10);
        }
    }

    cashierQueueLabel->setText(text);
}

void NurseryWindow::startSimulation()
{
    isRunning = true;
    simulationTimer->start(50);      // Update every 50ms
    customerSpawnTimer->start(3000); // Add customer every 3 seconds
    seasonTimer->start(30000);       // Change season every 30 seconds

    startBtn->setEnabled(false);
    pauseBtn->setEnabled(true);
    statusLabel->setText("Status: Running");
}

void NurseryWindow::pauseSimulation()
{
    isRunning = false;
    simulationTimer->stop();
    customerSpawnTimer->stop();
    seasonTimer->stop();

    startBtn->setEnabled(true);
    pauseBtn->setEnabled(false);
    statusLabel->setText("Status: Paused");
}

void NurseryWindow::changeSeason()
{
    if (!nurseryBackend)
        return;

    Seasons *currentSeason = nurseryBackend->getCurrentSeason();
    if (!currentSeason)
        return;

    std::string currentSeasonName = currentSeason->getSeason();
    qDebug() << "Changing season from" << QString::fromStdString(currentSeasonName);
    std::cout << "=== Season Change ===" << std::endl;
    std::cout << "Previous season: " << currentSeasonName << std::endl;

    // Trigger season change in backend
    currentSeason->handleChange(nurseryBackend);

    // Get new season and update display
    Seasons *newSeason = nurseryBackend->getCurrentSeason();
    if (newSeason)
    {
        std::string newSeasonName = newSeason->getSeason();
        qDebug() << "Season changed to" << QString::fromStdString(newSeasonName);
        std::cout << "New season: " << newSeasonName << std::endl;
        seasonLabel->setText(QString("Season: %1").arg(QString::fromStdString(newSeasonName)));

        // Update season label color based on season
        if (newSeasonName == "Spring")
        {
            seasonLabel->setStyleSheet("font-weight: bold; font-size: 14pt; color: #4CAF50;"); // Green
        }
        else if (newSeasonName == "Summer")
        {
            seasonLabel->setStyleSheet("font-weight: bold; font-size: 14pt; color: #FFC107;"); // Yellow
        }
        else if (newSeasonName == "Autumn")
        {
            seasonLabel->setStyleSheet("font-weight: bold; font-size: 14pt; color: #FF5722;"); // Orange
        }
        else if (newSeasonName == "Winter")
        {
            seasonLabel->setStyleSheet("font-weight: bold; font-size: 14pt; color: #2196F3;"); // Blue
        }
    }

    // Update inventory display to reflect seasonal changes
    updateInventoryDisplay();
    std::cout << "Inventory updated for new season" << std::endl;
}

void NurseryWindow::resetSimulation()
{
    pauseSimulation();

    // Remove all customer visuals
    for (auto &cv : customerVisuals)
    {
        scene->removeItem(cv.dot);
        delete cv.dot;
        if (cv.bubble)
        {
            scene->removeItem(cv.bubble);
            delete cv.bubble;
        }
    }
    customerVisuals.clear();

    // Reset backend (recreate it)
    delete nurseryBackend;
    nurseryBackend = new Nursery(nullptr);

    customerCountLabel->setText("Customers: 0");
    statusLabel->setText("Status: Ready");
}

void NurseryWindow::addCustomer()
{
    if (!nurseryBackend)
    {
        qDebug() << "Warning: Backend nursery not initialized";
        return;
    }

    // Check customer limit (max 20 customers)
    const std::vector<Customer *> &activeCustomers = nurseryBackend->getActiveCustomers();
    if (activeCustomers.size() >= 20)
    {
        qDebug() << "Nursery at capacity (20 customers). Cannot add more customers.";
        return;
    }

    qDebug() << "Creating new customer...";

    // Create customer in backend
    CustomerCreator *creator = new CustomerCreator();
    Customer *newCustomer = nurseryBackend->getStock() ? creator->createNewCustomer(nurseryBackend, nurseryBackend->getStock()) : nullptr;
    delete creator;

    if (newCustomer)
    {
        nurseryBackend->addCustomer(newCustomer);
        qDebug() << "Customer" << newCustomer->getId() << "created";

        // Check customer's initial action
        Action *action = newCustomer->getAction();
        if (action)
        {
            qDebug() << "=== New Customer" << newCustomer->getId() << "===";
            qDebug() << "Action:" << QString::fromStdString(action->getActionName());

            Browse *browseAction = dynamic_cast<Browse *>(action);
            Enquire *enquireAction = dynamic_cast<Enquire *>(action);

            // Check if customer wants plants that are out of stock
            bool plantOutOfStock = false;
            if (browseAction && nurseryBackend->getInventory())
            {
                std::vector<Plant *> plants = browseAction->getPlantsToBuy();
                Inventory *inventory = nurseryBackend->getInventory();
                auto &invMap = inventory->getInventory();

                for (Plant *plant : plants)
                {
                    if (plant)
                    {
                        auto it = invMap.find(plant->getName());
                        if (it != invMap.end() && it->second.second == 0)
                        {
                            qDebug() << "Customer" << newCustomer->getId() << "wants" << QString::fromStdString(plant->getName()) << "but inventory is 0";
                            std::cout << "Customer " << newCustomer->getId() << " wants " << plant->getName() << " but inventory is 0. Sending to exit." << std::endl;
                            plantOutOfStock = true;
                            break;
                        }
                    }
                }
            }

            // If plant is out of stock, customer should leave immediately
            if (plantOutOfStock)
            {
                qDebug() << "Customer" << newCustomer->getId() << "cannot find desired plants, leaving immediately";
                // Set customer to leaving state
                newCustomer->setAction(nullptr);
            }

            if (!plantOutOfStock)
            {
                if (browseAction)
                {
                    std::vector<Plant *> plants = browseAction->getPlantsToBuy();
                    std::vector<int> quantities = browseAction->getQuantities();
                    qDebug() << "IMMEDIATELY after creation: Browse has" << plants.size() << "plants";
                    for (size_t i = 0; i < plants.size(); ++i)
                    {
                        if (plants[i])
                        {
                            qDebug() << "  -" << quantities[i] << "x" << QString::fromStdString(plants[i]->getName());
                        }
                    }

                    // Start the browsing timer
                    action->handle(newCustomer);
                    qDebug() << "Browse timer started";
                }
                else if (enquireAction)
                {
                    // Customer has enquiry - don't assign staff yet, wait until they reach info desk
                    qDebug() << "Customer enquiring:" << QString::fromStdString(enquireAction->getEnquiryQuestion());
                    // DON'T call action->handle() here - staff will be assigned when customer reaches info desk
                }
                else
                {
                    // For other actions, just handle normally
                    action->handle(newCustomer);
                }
            }
        }

        // Create visual representation
        CustomerVisual cv;
        cv.customer = newCustomer;
        cv.isLeaving = (newCustomer->getAction() == nullptr); // Set leaving flag if no action

        // Create thinking bubble first (so we can pass it to CustomerDot)
        cv.bubble = new ThinkingBubble(this, newCustomer);
        scene->addItem(cv.bubble);
        cv.bubble->setPos(60, SCENE_HEIGHT - 35 - 30); // Position above the dot
        cv.bubble->updateColor();                      // Set initial color based on state

        // Create blue dot for customer with hover capability
        cv.dot = new CustomerDot(0, 0, CUSTOMER_SIZE, CUSTOMER_SIZE, cv.bubble);
        cv.dot->setPen(QPen(Qt::darkBlue, 2));
        cv.dot->setBrush(QBrush(QColor(0, 120, 255)));
        cv.dot->setOpacity(0.9);
        scene->addItem(cv.dot);

        // Start at entrance (bottom left, adjusted for new position)
        cv.dot->setPos(10, SCENE_HEIGHT - 35);

        // Set random initial target
        std::uniform_real_distribution<qreal> distX(100, SCENE_WIDTH - 100);
        std::uniform_real_distribution<qreal> distY(150, SCENE_HEIGHT - 100);

        cv.targetX = distX(rng);
        cv.targetY = distY(rng);
        cv.velocityX = 0;
        cv.velocityY = 0;

        customerVisuals.append(cv);
        if (nurseryBackend)
        {
            const std::vector<Customer *> &activeCustomers = nurseryBackend->getActiveCustomers();
            customerCountLabel->setText(QString("Customers: %1/20").arg(activeCustomers.size()));
        }
        qDebug() << "Customer visual added. Total customers:" << customerVisuals.size();
    }
    else
    {
        qDebug() << "Failed to create customer";
    }
}

void NurseryWindow::updateSimulation()
{
    if (!isRunning)
        return;

    // Update season display
    if (nurseryBackend && nurseryBackend->getCurrentSeason())
    {
        QString seasonName = QString::fromStdString(nurseryBackend->getCurrentSeason()->getSeason());
        seasonLabel->setText(QString("Season: %1").arg(seasonName));
    }

    // Update each customer's movement
    for (int i = customerVisuals.size() - 1; i >= 0; --i)
    {
        updateCustomerMovement(customerVisuals[i]);

        // Remove customer visual if customer is null (departed)
        if (!customerVisuals[i].customer || !customerVisuals[i].dot)
        {
            customerVisuals.removeAt(i);
        }
    }

    // Update customer count display
    if (nurseryBackend)
    {
        const std::vector<Customer *> &activeCustomers = nurseryBackend->getActiveCustomers();
        customerCountLabel->setText(QString("Customers: %1/20").arg(activeCustomers.size()));
    }

    // Update thinking bubble information if any are displayed
    updateThinkingBubbles();

    // Update inventory, staff, and cashier queue displays every tick
    updateInventoryDisplay();
    updateStaffDisplay();
    updateCashierQueue();

    // Process staff duties (cashier checkout, etc.)
    if (nurseryBackend && nurseryBackend->getInfoDesk())
    {
        InfoDesk *desk = nurseryBackend->getInfoDesk();
        std::vector<Staff *> allStaff = desk->getAllStaff();
        for (Staff *staff : allStaff)
        {
            // Only perform duty if staff currently has a customer
            if (staff && staff->getCurrentCustomer())
            {
                staff->performDuty();
            }
        }

        // Process waiting customers at info desk
        desk->processWaitingCustomers();
    }
}
void NurseryWindow::updateCustomerMovement(CustomerVisual &cv)
{
    if (!cv.dot || !cv.customer)
        return;

    QPointF currentPos = cv.dot->pos();

    // Check customer's current action
    Action *action = cv.customer->getAction();
    bool isPurchasing = (action && action->getActionName() == "Purchasing");
    bool isEnquiring = (action && action->getActionName() == "Enquiring");

    // Debug: Log current action state
    if (action && cv.infoDeskArrivalTime > 0)
    {
        qDebug() << "Customer" << cv.customer->getId() << "current action:" << QString::fromStdString(action->getActionName());
    }

    // Handle Enquiring customers - move to Info Desk and wait
    // Skip if customer has already finished at info desk
    if (isEnquiring && !cv.finishedAtInfoDesk)
    {
        // Info Desk position (center of the info desk box) - updated with 60px offset
        qreal infoDeskX = -40 + 200 + 90 + 75; // staffBoxX + staffBoxWidth + spacing(90px) + half width
        qreal infoDeskY = 90;                  // Center Y of info desk box

        // Calculate distance to info desk
        qreal dx = infoDeskX - currentPos.x();
        qreal dy = infoDeskY - currentPos.y();
        qreal distance = std::sqrt(dx * dx + dy * dy);

        // If customer has arrived at desk (timer started), check if time is up FIRST
        if (cv.infoDeskArrivalTime > 0)
        {
            qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
            qint64 elapsedTime = currentTime - cv.infoDeskArrivalTime;

            if (elapsedTime >= 5000) // 5 seconds = 5000 milliseconds
            {
                qDebug() << "Customer" << cv.customer->getId() << "finished at info desk, moving away";

                // Reset timer FIRST before any other operations
                cv.infoDeskArrivalTime = 0;
                cv.finishedAtInfoDesk = true; // Mark as finished so they won't return to desk

                // Set a new target position away from the info desk so they move away
                std::uniform_real_distribution<qreal> distX(100, SCENE_WIDTH - 100);
                std::uniform_real_distribution<qreal> distY(200, SCENE_HEIGHT - 100);
                cv.targetX = distX(rng);
                cv.targetY = distY(rng);

                qDebug() << "Customer moving to new target:" << cv.targetX << "," << cv.targetY;

                // Immediately start moving away from info desk
                qreal dxMove = cv.targetX - currentPos.x();
                qreal dyMove = cv.targetY - currentPos.y();
                qreal dist = std::sqrt(dxMove * dxMove + dyMove * dyMove);

                if (dist > 0)
                {
                    cv.velocityX = (dxMove / dist) * MOVEMENT_SPEED;
                    cv.velocityY = (dyMove / dist) * MOVEMENT_SPEED;

                    qreal newX = currentPos.x() + cv.velocityX;
                    qreal newY = currentPos.y() + cv.velocityY;

                    cv.dot->setPos(newX, newY);
                    qDebug() << "  Customer moved to" << newX << "," << newY;

                    if (cv.bubble)
                    {
                        cv.bubble->setPos(newX, newY - 30);
                    }
                }

                // Properly release the staff through the backend
                Staff *assignedStaff = cv.customer->getAssignedStaff();
                if (assignedStaff && nurseryBackend && nurseryBackend->getInfoDesk())
                {
                    qDebug() << "  Releasing staff" << QString::fromStdString(assignedStaff->getName()) << "for customer" << cv.customer->getId();

                    try
                    {
                        // Clear staff's current customer first
                        assignedStaff->setCurrentCustomer(nullptr);
                        // Set staff availability to true
                        assignedStaff->setAvailability(true);
                        // Clear the customer's assignment
                        cv.customer->setAssignedStaff(nullptr);
                        // Notify InfoDesk that staff is available again
                        nurseryBackend->getInfoDesk()->notifyStaffAvailable(assignedStaff);
                        qDebug() << "  Staff released successfully";
                    }
                    catch (...)
                    {
                        qDebug() << "  ERROR: Exception while releasing staff";
                        // Clear customer assignment even if staff operations fail
                        cv.customer->setAssignedStaff(nullptr);
                    }
                }

                // Transition customer to next action after finishing at info desk
                if (cv.customer)
                {
                    qDebug() << "  Customer" << cv.customer->getId() << "transitioning to next action";
                    cv.customer->processNextAction();

                    // Check if customer is now leaving (action is null)
                    if (!cv.customer->getAction())
                    {
                        qDebug() << "  Customer" << cv.customer->getId() << "decided to leave after enquiring";
                        cv.isLeaving = true; // Mark customer as leaving so they walk to exit
                    }
                    else
                    {
                        qDebug() << "  Customer new action:" << cv.customer->getAction()->getActionName().c_str();
                    }

                    // Update bubble color after state change
                    if (cv.bubble)
                    {
                        cv.bubble->updateColor();
                    }
                }

                // Return immediately after transition to avoid accessing stale state
                return;
            }
            // If customer is still waiting at desk (timer running but not expired), stay at the info desk position
            else
            {
                // Customer is still waiting at info desk, don't move
                qDebug() << "Customer" << cv.customer->getId() << "waiting at desk, elapsed:" << elapsedTime << "ms";
                return;
            }
        }

        // If customer reached info desk while enquiring (and timer not started)
        if (distance < 30 && isEnquiring && cv.infoDeskArrivalTime == 0)
        {
            // Start the timer and assign staff (even if already assigned from elsewhere)
            cv.infoDeskArrivalTime = QDateTime::currentMSecsSinceEpoch();
            qDebug() << "Customer" << cv.customer->getId() << "arrived at info desk, starting 5s timer";

            // Assign staff if not already assigned
            if (nurseryBackend && nurseryBackend->getInfoDesk())
            {
                if (!cv.customer->getAssignedStaff())
                {
                    qDebug() << "  Assigning staff to customer" << cv.customer->getId();
                    nurseryBackend->getInfoDesk()->handleCustomer(cv.customer);
                }
                else
                {
                    qDebug() << "  Customer" << cv.customer->getId() << "already has staff assigned";
                }
            }

            // Staff is already set to busy by the backend's assistCustomer() method
            // No need to call changeState() here as it would toggle the state incorrectly
            return; // Stay at desk this frame
        }
        else if (distance >= 30 && cv.infoDeskArrivalTime == 0)
        {
            // Still moving toward info desk
            cv.velocityX = (dx / distance) * MOVEMENT_SPEED;
            cv.velocityY = (dy / distance) * MOVEMENT_SPEED;

            qreal newX = currentPos.x() + cv.velocityX;
            qreal newY = currentPos.y() + cv.velocityY;

            cv.dot->setPos(newX, newY);

            if (cv.bubble)
            {
                cv.bubble->setPos(newX, newY - 30);
            }
            return;
        }
    }

    if (isPurchasing)
    {
        // First, check if customer has items in basket
        const std::vector<Plant *> &basket = cv.customer->getBasket();
        if (basket.empty())
        {
            // Customer has nothing to purchase, send them to exit immediately
            qDebug() << "Customer" << cv.customer->getId() << "has empty basket, leaving immediately";

            if (nurseryBackend)
            {
                nurseryBackend->handleCustomerDeparture(cv.customer);
            }

            scene->removeItem(cv.dot);
            delete cv.dot;
            cv.dot = nullptr;

            if (cv.bubble)
            {
                scene->removeItem(cv.bubble);
                delete cv.bubble;
                cv.bubble = nullptr;
            }

            // Delete customer object
            delete cv.customer;
            cv.customer = nullptr;
            return;
        }

        // Cashier position (center of cashier box)
        qreal cashierX = SCENE_WIDTH - 90 - 100 - 7 + 50; // Center of cashier box
        qreal cashierY = SCENE_HEIGHT - 35;               // Center of cashier box

        // Calculate distance to cashier
        qreal dxCashier = cashierX - currentPos.x();
        qreal dyCashier = cashierY - currentPos.y();
        qreal distanceToCashier = std::sqrt(dxCashier * dxCashier + dyCashier * dyCashier);

        // If at cashier, wait for 3 seconds while basket is processed
        if (distanceToCashier < 30 && cv.cashierArrivalTime == 0)
        {
            cv.cashierArrivalTime = QDateTime::currentMSecsSinceEpoch();
            qDebug() << "Customer" << cv.customer->getId() << "arrived at cashier - ADDED TO QUEUE";
            std::cout << "Customer " << cv.customer->getId() << " entered cashier queue" << std::endl;

            // Update cashier staff status to Busy
            if (nurseryBackend && nurseryBackend->getCashier())
            {
                Cashiers *cashier = nurseryBackend->getCashier();
                cashier->setAvailability(false);
                cashier->changeState(); // Change to Busy
                std::cout << "Cashier " << cashier->getName() << " is now Busy with Customer " << cv.customer->getId() << std::endl;
            }

            updateCashierQueue();
            return;
        }

        if (cv.cashierArrivalTime > 0)
        {
            qint64 currentTime = QDateTime::currentMSecsSinceEpoch();
            qint64 elapsedTime = currentTime - cv.cashierArrivalTime;

            if (elapsedTime >= 3000) // 3 seconds
            {
                qDebug() << "Customer" << cv.customer->getId() << "finished at cashier - REMOVED FROM QUEUE";
                std::cout << "Customer " << cv.customer->getId() << " left cashier queue (basket cleared)" << std::endl;

                // Clear the basket
                cv.customer->clearBasket();
                cv.cashierArrivalTime = 0;

                // Update cashier staff status back to Available
                if (nurseryBackend && nurseryBackend->getCashier())
                {
                    Cashiers *cashier = nurseryBackend->getCashier();
                    cashier->setAvailability(true);
                    cashier->changeState(); // Change to Available
                    std::cout << "Cashier " << cashier->getName() << " is now Available" << std::endl;
                }

                updateCashierQueue();

                // Now move to exit
            }
            else
            {
                // Stay at cashier
                return;
            }
        }

        // If not yet at cashier, move to cashier first
        if (distanceToCashier >= 30)
        {
            cv.velocityX = (dxCashier / distanceToCashier) * MOVEMENT_SPEED * 1.5;
            cv.velocityY = (dyCashier / distanceToCashier) * MOVEMENT_SPEED * 1.5;

            qreal newX = currentPos.x() + cv.velocityX;
            qreal newY = currentPos.y() + cv.velocityY;

            cv.dot->setPos(newX, newY);

            if (cv.bubble)
            {
                cv.bubble->setPos(newX, newY - 30);
            }
            return;
        }

        // After cashier, move to exit
        qreal exitX = SCENE_WIDTH - 40;  // Center of 100px wide exit box
        qreal exitY = SCENE_HEIGHT - 35; // Center of 50px tall exit box

        // Calculate distance to exit
        qreal dx = exitX - currentPos.x();
        qreal dy = exitY - currentPos.y();
        qreal distance = std::sqrt(dx * dx + dy * dy);

        // If customer reached the exit, remove them
        if (distance < 40)
        {
            qDebug() << "Customer" << cv.customer->getId() << "reached exit. Removing from nursery.";
            std::cout << "Customer " << cv.customer->getId() << " left the nursery" << std::endl;

            // Handle customer departure in backend (this updates count and deletes customer)
            if (nurseryBackend)
            {
                nurseryBackend->handleCustomerDeparture(cv.customer);
            }

            // Remove visual representation
            scene->removeItem(cv.dot);
            delete cv.dot;
            cv.dot = nullptr;

            if (cv.bubble)
            {
                scene->removeItem(cv.bubble);
                delete cv.bubble;
                cv.bubble = nullptr;
            }

            cv.customer = nullptr; // Customer is already deleted by handleCustomerDeparture

            // Update cashier queue display
            updateCashierQueue();
            return;
        }

        // Move toward exit
        if (distance > 0)
        {
            cv.velocityX = (dx / distance) * MOVEMENT_SPEED * 1.5; // Move faster to exit
            cv.velocityY = (dy / distance) * MOVEMENT_SPEED * 1.5;

            qreal newX = currentPos.x() + cv.velocityX;
            qreal newY = currentPos.y() + cv.velocityY;

            cv.dot->setPos(newX, newY);

            if (cv.bubble)
            {
                cv.bubble->setPos(newX, newY - 30);
            }
        }
    }
    else
    {
        // Check if customer is leaving (decided to leave after enquiring)
        if (cv.isLeaving)
        {
            // Move to exit
            qreal exitX = SCENE_WIDTH - 40;
            qreal exitY = SCENE_HEIGHT - 35;

            qreal dx = exitX - currentPos.x();
            qreal dy = exitY - currentPos.y();
            qreal distance = std::sqrt(dx * dx + dy * dy);

            // If customer reached the exit, remove them
            if (distance < 40)
            {
                qDebug() << "Customer" << cv.customer->getId() << "reached exit after deciding to leave. Removing from nursery.";

                // Handle customer departure in backend (removes from active list)
                if (nurseryBackend)
                {
                    nurseryBackend->handleCustomerDeparture(cv.customer);
                }

                // Remove visual representation
                scene->removeItem(cv.dot);
                delete cv.dot;
                cv.dot = nullptr;

                if (cv.bubble)
                {
                    scene->removeItem(cv.bubble);
                    delete cv.bubble;
                    cv.bubble = nullptr;
                }

                // Delete customer object (safe now because we're not in customer's execution context)
                delete cv.customer;
                cv.customer = nullptr;
                return;
            }

            // Move toward exit
            if (distance > 0)
            {
                cv.velocityX = (dx / distance) * MOVEMENT_SPEED * 1.5;
                cv.velocityY = (dy / distance) * MOVEMENT_SPEED * 1.5;

                qreal newX = currentPos.x() + cv.velocityX;
                qreal newY = currentPos.y() + cv.velocityY;

                cv.dot->setPos(newX, newY);

                if (cv.bubble)
                {
                    cv.bubble->setPos(newX, newY - 30);
                }
            }
            return;
        }

        // Normal browsing/enquiring behavior - random movement
        qreal dx = cv.targetX - currentPos.x();
        qreal dy = cv.targetY - currentPos.y();
        qreal distance = std::sqrt(dx * dx + dy * dy);

        // If close to target, pick a new random target
        if (distance < 30)
        {
            std::uniform_real_distribution<qreal> distX(100, SCENE_WIDTH - 100);
            std::uniform_real_distribution<qreal> distY(150, SCENE_HEIGHT - 100);

            cv.targetX = distX(rng);
            cv.targetY = distY(rng);

            dx = cv.targetX - currentPos.x();
            dy = cv.targetY - currentPos.y();
            distance = std::sqrt(dx * dx + dy * dy);
        }

        // Move towards target
        if (distance > 0)
        {
            cv.velocityX = (dx / distance) * MOVEMENT_SPEED;
            cv.velocityY = (dy / distance) * MOVEMENT_SPEED;

            qreal newX = currentPos.x() + cv.velocityX;
            qreal newY = currentPos.y() + cv.velocityY;

            // Keep within bounds
            newX = qMax(0.0, qMin(newX, SCENE_WIDTH - CUSTOMER_SIZE));
            newY = qMax(0.0, qMin(newY, SCENE_HEIGHT - CUSTOMER_SIZE));

            cv.dot->setPos(newX, newY);

            // Move thinking bubble with customer
            if (cv.bubble)
            {
                cv.bubble->setPos(newX, newY - 30);
            }
        }
    }
}

void NurseryWindow::removeCustomerVisual(Customer *customer)
{
    for (int i = 0; i < customerVisuals.size(); ++i)
    {
        if (customerVisuals[i].customer == customer)
        {
            scene->removeItem(customerVisuals[i].dot);
            delete customerVisuals[i].dot;
            if (customerVisuals[i].bubble)
            {
                scene->removeItem(customerVisuals[i].bubble);
                delete customerVisuals[i].bubble;
            }
            customerVisuals.removeAt(i);
            if (nurseryBackend)
            {
                const std::vector<Customer *> &activeCustomers = nurseryBackend->getActiveCustomers();
                customerCountLabel->setText(QString("Customers: %1/20").arg(activeCustomers.size()));
            }
            break;
        }
    }
}

void NurseryWindow::updateThinkingBubbles()
{
    // Refresh info and colors in any thinking bubbles to reflect state changes
    for (auto &cv : customerVisuals)
    {
        if (cv.bubble)
        {
            cv.bubble->updateColor();
            cv.bubble->refreshInfo();
        }
    }
}

QString NurseryWindow::getCustomerInfo(Customer *customer)
{
    if (!customer)
        return "No customer data";

    QString info;
    info += QString("Customer #%1\n\n").arg(customer->getId());

    // Get current state/action
    Action *action = customer->getAction();
    if (action)
    {
        info += QString("State: %1\n\n").arg(QString::fromStdString(action->getActionName()));

        // Check if browsing and extract plant info
        Browse *browseAction = dynamic_cast<Browse *>(action);
        if (browseAction)
        {
            std::vector<Plant *> plants = browseAction->getPlantsToBuy();
            std::vector<int> quantities = browseAction->getQuantities();

            qDebug() << "=== getCustomerInfo: Browse customer detected ===";
            qDebug() << "Plants vector size:" << plants.size();
            qDebug() << "Quantities vector size:" << quantities.size();

            if (plants.size() > 0)
            {
                info += "Looking for:\n";
                for (size_t i = 0; i < plants.size() && i < quantities.size(); ++i)
                {
                    if (plants[i])
                    {
                        QString plantName = QString::fromStdString(plants[i]->getName());
                        info += QString("  • %1 x %2\n").arg(quantities[i]).arg(plantName);
                        qDebug() << "  Display:" << quantities[i] << "x" << plantName;

                        // Check if plant has decorations
                        PlantDecorator *decorator = dynamic_cast<PlantDecorator *>(plants[i]);
                        if (decorator)
                        {
                            // Check specific decoration types
                            if (dynamic_cast<GiftWrap *>(decorator))
                            {
                                info += "    → Wants: Gift Wrap\n";
                            }
                            else if (dynamic_cast<Pot *>(decorator))
                            {
                                info += "    → Wants: Pot\n";
                            }
                            else if (dynamic_cast<SpecialArrangement *>(decorator))
                            {
                                info += "    → Wants: Special Arrangement\n";
                            }
                            else
                            {
                                info += "    → Wants: Decoration\n";
                            }
                        }
                    }
                }
            }
            else
            {
                info += "Looking for: (Just started browsing...)\n";
                qDebug() << "  ERROR: Browse action has 0 plants!";
            }
        }
        else
        {
            qDebug() << "  Not a Browse action";
        }

        // Check if enquiring
        Enquire *enquireAction = dynamic_cast<Enquire *>(action);
        if (enquireAction)
        {
            std::vector<Plant *> plants = enquireAction->getPlantsOfInterest();

            if (enquireAction->getQuestionType() == 0)
            {
                info += "Type: Advice Question\n\n";
                QString question = QString::fromStdString(enquireAction->getAdviceQuestion());
                if (!question.isEmpty())
                {
                    info += QString("Question: %1\n").arg(question);
                }
            }
            else
            {
                info += "Type: Information Question\n\n";
                // For information questions, display the specific question about stock
                if (plants.size() > 0 && plants[0])
                {
                    QString plantName = QString::fromStdString(plants[0]->getName());
                    info += QString("Question: How many %1 do you have in stock?\n").arg(plantName);
                }
                else
                {
                    info += "Question: General stock inquiry\n";
                }
            }
        }

        // Check if purchasing
        Purchasing *purchaseAction = dynamic_cast<Purchasing *>(action);
        if (purchaseAction)
        {
            std::vector<Plant *> plants = purchaseAction->getPlantsToBuy();
            std::vector<int> quantities = purchaseAction->getQuantities();

            if (plants.size() > 0)
            {
                info += "Acquired:\n";
                for (size_t i = 0; i < plants.size() && i < quantities.size(); ++i)
                {
                    if (plants[i])
                    {
                        QString plantName = QString::fromStdString(plants[i]->getName());
                        info += QString("  • %1 x %2 (Acquired)\n").arg(quantities[i]).arg(plantName);

                        // Check if plant has decorations
                        PlantDecorator *decorator = dynamic_cast<PlantDecorator *>(plants[i]);
                        if (decorator)
                        {
                            if (dynamic_cast<GiftWrap *>(decorator))
                            {
                                info += "    → With: Gift Wrap\n";
                            }
                            else if (dynamic_cast<Pot *>(decorator))
                            {
                                info += "    → With: Pot\n";
                            }
                            else if (dynamic_cast<SpecialArrangement *>(decorator))
                            {
                                info += "    → With: Special Arrangement\n";
                            }
                            else
                            {
                                info += "    → With: Decoration\n";
                            }
                        }
                    }
                }
                info += "\n";
            }
        }
    }
    else
    {
        info += "State: Unknown\n";
    }

    // Show basket contents
    const std::vector<Plant *> &basket = customer->getBasket();
    if (!basket.empty())
    {
        info += QString("Basket: %1 items\n").arg(basket.size());

        // Count unique plants
        std::map<std::string, int> plantCounts;
        for (Plant *plant : basket)
        {
            if (plant)
            {
                plantCounts[plant->getName()]++;
            }
        }

        for (const auto &pair : plantCounts)
        {
            info += QString("  • %1 x %2\n").arg(pair.second).arg(QString::fromStdString(pair.first));
        }
    }

    return info;
}
