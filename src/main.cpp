#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget* parent = nullptr)
        : QMainWindow(parent)
        , networkAccessManager(std::make_unique<QNetworkAccessManager>(this))
    {
        auto trayIcon = new QSystemTrayIcon(QPixmap(":/icon.svg"));
        trayIcon->setContextMenu(getMenu().get());
        trayIcon->show();

        auto updateTimer = new QTimer(this);
        updateTimer->start(15000);

        connect(networkAccessManager.get(), &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
            auto data = reply->readAll();

            if (reply->url() == URL_STATUS_ALL) {
                lightsAction->setChecked(!reply->error() && data.contains(R"("POWER1":"ON")"));
                speakersAction->setChecked(!reply->error() && data.contains(R"("POWER2":"ON")"));
            } else if (reply->url() == URL_TOGGLE_LIGHTS) {
                lightsAction->setChecked(!reply->error() && data.contains(R"("POWER1":"ON")"));
            } else if (reply->url() == URL_TOGGLE_SPEAKERS) {
                speakersAction->setChecked(!reply->error() && data.contains(R"("POWER2":"ON")"));
            }
        });

        connect(updateTimer, &QTimer::timeout, this,
            [this] { networkAccessManager->get(QNetworkRequest(URL_STATUS_ALL)); });

        // Initialize states
        networkAccessManager->get(QNetworkRequest(URL_STATUS_ALL));
    }

    std::unique_ptr<QMenu>& getMenu()
    {
        lightsAction = std::make_unique<QAction>("lampka ;3", this);
        lightsAction->setCheckable(true);

        speakersAction = std::make_unique<QAction>("glośniki ^.^", this);
        speakersAction->setCheckable(true);

        connect(lightsAction.get(), &QAction::triggered, this,
            [this] { networkAccessManager->get(QNetworkRequest(URL_TOGGLE_LIGHTS)); });
        connect(speakersAction.get(), &QAction::triggered, this,
            [this] { networkAccessManager->get(QNetworkRequest(URL_TOGGLE_SPEAKERS)); });

        menu = std::make_unique<QMenu>(this);
        menu->addAction(lightsAction.get());
        menu->addAction(speakersAction.get());

        return menu;
    }

private:
    const QUrl URL_STATUS_ALL = QUrl("http://192.168.1.225/cm?cmnd=POWER0");
    const QUrl URL_TOGGLE_LIGHTS = QUrl("http://192.168.1.225/cm?cmnd=POWER1%202");
    const QUrl URL_TOGGLE_SPEAKERS = QUrl("http://192.168.1.225/cm?cmnd=POWER2%202");

    std::unique_ptr<QNetworkAccessManager> networkAccessManager;

    std::unique_ptr<QAction> lightsAction;
    std::unique_ptr<QAction> speakersAction;
    std::unique_ptr<QMenu> menu;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    MainWindow mainWindow;

    return QApplication::exec();
}
