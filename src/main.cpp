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

        connect(trayIcon, &QSystemTrayIcon::activated, this, [trayIcon](auto reason) {
            if (reason == QSystemTrayIcon::ActivationReason::Trigger) {
                trayIcon->contextMenu()->popup(QCursor::pos());
            }
        });

        auto updateTimer = new QTimer(this);
        updateTimer->start(15000);

        connect(networkAccessManager.get(), &QNetworkAccessManager::finished, this, [&](QNetworkReply* reply) {
            auto data = reply->readAll();

            if (reply->url() == URL_STATUS_LIGHTS || reply->url() == URL_TOGGLE_LIGHTS) {
                lightsAction->setChecked(!reply->error() && data.contains(R"("POWER":"ON")"));
            } else if (reply->url() == URL_STATUS_SPEAKERS || reply->url() == URL_TOGGLE_SPEAKERS) {
                speakersAction->setChecked(!reply->error() && data.contains(R"("POWER":"ON")"));
            }

            reply->deleteLater();
        });

        connect(updateTimer, &QTimer::timeout, this, [this] {
            networkAccessManager->get(QNetworkRequest(URL_STATUS_LIGHTS));
            networkAccessManager->get(QNetworkRequest(URL_STATUS_SPEAKERS));
        });

        // Initialize states
        networkAccessManager->get(QNetworkRequest(URL_STATUS_LIGHTS));
        networkAccessManager->get(QNetworkRequest(URL_STATUS_SPEAKERS));
    }

    std::unique_ptr<QMenu>& getMenu()
    {
        lightsAction = std::make_unique<QAction>("lampka ;3", this);
        lightsAction->setCheckable(true);

        speakersAction = std::make_unique<QAction>("glośniki ^.^", this);
        speakersAction->setCheckable(true);

        quitAction = std::make_unique<QAction>("Quit", this);

        connect(lightsAction.get(), &QAction::triggered, this,
            [this] { networkAccessManager->get(QNetworkRequest(URL_TOGGLE_LIGHTS)); });
        connect(speakersAction.get(), &QAction::triggered, this,
            [this] { networkAccessManager->get(QNetworkRequest(URL_TOGGLE_SPEAKERS)); });
        connect(quitAction.get(), &QAction::triggered, this, [] { qApp->quit(); });

        menu = std::make_unique<QMenu>(this);
        menu->addAction(lightsAction.get());
        menu->addAction(speakersAction.get());
        menu->addSeparator();
        menu->addAction(quitAction.get());

        return menu;
    }

private:
    const QUrl URL_STATUS_LIGHTS = QUrl("http://192.168.1.227/cm?cmnd=POWER");
    const QUrl URL_STATUS_SPEAKERS = QUrl("http://192.168.1.226/cm?cmnd=POWER");
    const QUrl URL_TOGGLE_LIGHTS = QUrl(URL_STATUS_LIGHTS.toString() + "%202");
    const QUrl URL_TOGGLE_SPEAKERS = QUrl(URL_STATUS_SPEAKERS.toString() + "%202");

    std::unique_ptr<QNetworkAccessManager> networkAccessManager;

    std::unique_ptr<QAction> lightsAction;
    std::unique_ptr<QAction> speakersAction;
    std::unique_ptr<QAction> quitAction;
    std::unique_ptr<QMenu> menu;
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    MainWindow mainWindow;

    return QApplication::exec();
}
