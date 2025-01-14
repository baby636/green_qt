#ifndef GREEN_WALLET_H
#define GREEN_WALLET_H

#include "activity.h"
#include "connectable.h"
#include "session.h"

#include <QtQml>
#include <QAtomicInteger>
#include <QList>
#include <QObject>
#include <QQmlListProperty>
#include <QThread>
#include <QJsonObject>

class Account;
class Asset;
class Device;
class Network;
class Session;
class WalletUpdateAccountsActivity;

struct GA_session;
struct GA_auth_handler;
struct GA_json;

#include "handler.h"
class SetPinHandler : public Handler
{
    const QByteArray m_pin;
    QByteArray m_pin_data;
    void call(GA_session* session, GA_auth_handler** auth_handler) override;
public:
    SetPinHandler(Wallet* wallet, const QByteArray& pin);
    QByteArray pinData() const;
};

class Wallet : public Entity
{
    Q_OBJECT
    QML_ELEMENT
public:
    enum AuthenticationStatus {
        Unauthenticated,
        Authenticating,
        Authenticated
    };
    Q_ENUM(AuthenticationStatus)

private:
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(bool watchOnly READ isWatchOnly CONSTANT)
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
    Q_PROPERTY(Session* session READ session NOTIFY sessionChanged)
    Q_PROPERTY(Network* network READ network CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(bool hasPinData READ hasPinData NOTIFY hasPinDataChanged)
    Q_PROPERTY(bool authenticated READ isAuthenticated NOTIFY authenticationChanged)
    Q_PROPERTY(AuthenticationStatus authentication READ authentication NOTIFY authenticationChanged)
    Q_PROPERTY(bool ready READ ready NOTIFY readyChanged)
    Q_PROPERTY(bool locked READ isLocked NOTIFY lockedChanged)
    Q_PROPERTY(QJsonObject settings READ settings NOTIFY settingsChanged)
    Q_PROPERTY(QJsonObject currencies READ currencies CONSTANT)
    Q_PROPERTY(QQmlListProperty<Account> accounts READ accounts NOTIFY accountsChanged)
    Q_PROPERTY(QJsonObject events READ events NOTIFY eventsChanged)
    Q_PROPERTY(QStringList mnemonic READ mnemonic CONSTANT)
    Q_PROPERTY(int loginAttemptsRemaining READ loginAttemptsRemaining NOTIFY loginAttemptsRemainingChanged)
    Q_PROPERTY(QJsonObject config READ config NOTIFY configChanged)
    Q_PROPERTY(Device* device READ device CONSTANT)
    Q_PROPERTY(bool empty READ isEmpty NOTIFY emptyChanged)
public:
    explicit Wallet(Network* network, QObject *parent = nullptr);
    virtual ~Wallet();
    QString id() const;
    Session* session() const { return m_session; }
    void setSession(Session *session);
    Network* network() const { return m_network; }
    QString name() const { return m_name; }
    void setName(const QString& name);

    bool isAuthenticated() const { return m_authentication == Authenticated; }
    AuthenticationStatus authentication() const { return m_authentication; }

    bool ready() const { return m_ready; }
    bool isEmpty() const { return m_empty; }
    bool isLocked() const { return m_locked; }
    void setLocked(bool locked);

    QJsonObject settings() const;
    QJsonObject currencies() const;

    QQmlListProperty<Account> accounts();

    void handleNotification(const QJsonObject& notification);

    QJsonObject events() const;

    QStringList mnemonic() const;

    int loginAttemptsRemaining() const { return m_login_attempts_remaining; }

    QJsonObject config() const { return m_config; }

    Q_INVOKABLE void changePin(const QByteArray& pin);
    Q_INVOKABLE QJsonObject convert(const QJsonObject& value) const;

    qint64 amountToSats(const QString& amount) const;
    Q_INVOKABLE qint64 parseAmount(const QString& amount, const QString& unit) const;

    QString formatAmount(qint64 amount, bool include_ticker) const;
    Q_INVOKABLE QString formatAmount(qint64 amount, bool include_ticker, const QString& unit) const;

    Q_INVOKABLE Asset* getOrCreateAsset(const QString& id);

    Account* getOrCreateAccount(const QJsonObject& data);

    void createSession();
    void setSession();

    Device* device() const { return m_device; }

    void updateHashId(const QString& hash_id);
public slots:
    void disconnect();
    void reload();

    void updateConfig();
    void updateSettings();

    void refreshAssets(bool refresh);

    void rename(QString name, bool active_focus);
    void setWatchOnly(const QString& username, const QString& password);
signals:
    void readyChanged(bool ready);
    void sessionChanged(Session* session);
    void hasPinDataChanged();
    void authenticationChanged();
    void lockedChanged(bool locked);
    void notification(const QString& type, const QJsonObject& data);
    void accountsChanged();
    void eventsChanged(QJsonObject events);
    void nameChanged(QString name);
    void loginAttemptsRemainingChanged(int loginAttemptsRemaining);
    void settingsChanged();
    void configChanged();
    void pinSet();
    void emptyChanged(bool empty);
    void usernameChanged(const QString& username);
protected:
    bool eventFilter(QObject* object, QEvent* event) override;
    void timerEvent(QTimerEvent* event) override;
private:
    void updateEmpty();
    void setEmpty(bool empty);
private:
    bool m_ready{false};
    bool m_empty{true};
public:
    void setAuthentication(AuthenticationStatus authentication);
    void setSettings(const QJsonObject& settings);
    void updateCurrencies();

    QString m_id;
    QString m_hash_id;
    bool m_restoring{false};
    WalletUpdateAccountsActivity* m_update_accounts_activity{nullptr};

    Connectable<Session> m_session;
    AuthenticationStatus m_authentication{Unauthenticated};
    bool m_locked{true};
    QJsonObject m_settings;
    QJsonObject m_config;
    QJsonObject m_currencies;
    QJsonObject m_events;
    QMap<QString, Asset*> m_assets;
    QList<Account*> m_accounts;
    QMap<int, Account*> m_accounts_by_pointer;

    QByteArray getPinData() const;
    QByteArray m_pin_data;
    QString m_name;
    Network* const m_network{nullptr};
    int m_login_attempts_remaining{3};
    int m_logout_timer{-1};
    bool m_busy{false};

    void save();
    Device* m_device{nullptr};
    bool hasPinData() const { return !m_pin_data.isEmpty(); }
    void clearPinData();

    bool m_watch_only{false};
    QString m_username;
    bool isWatchOnly() const { return m_watch_only; }
    QString username() const { return m_username; }
protected slots:
    void updateReady();
};

class WalletActivity : public Activity
{
    Q_OBJECT
    Q_PROPERTY(Wallet* wallet READ wallet CONSTANT)
    QML_ELEMENT
public:
    WalletActivity(Wallet* wallet, QObject* parent);
    Wallet* wallet() const { return m_wallet; }
private:
    Wallet* const m_wallet;
};

class WalletAuthenticateActivity : public WalletActivity
{
    Q_OBJECT
    QML_ELEMENT
public:
    WalletAuthenticateActivity(Wallet* wallet, QObject* parent);
    void exec() override;
};

class WalletSignupActivity : public WalletActivity
{
    Q_OBJECT
    QML_ELEMENT
public:
    WalletSignupActivity(Wallet* wallet, QObject* parent);
    void exec() override;
};

class WalletUpdateAccountsActivity : public WalletActivity
{
    Q_OBJECT
    QML_ELEMENT
public:
    WalletUpdateAccountsActivity(Wallet* wallet, QObject* parent);
    void exec() override;
};

class WalletRefreshAssets : public WalletActivity
{
    Q_OBJECT
    QML_ELEMENT
public:
    WalletRefreshAssets(Wallet* wallet, QObject* parent);
    void exec() override;
};

class LoginWithPinController : public Entity
{
    Q_OBJECT
    Q_PROPERTY(Wallet* wallet READ wallet WRITE setWallet NOTIFY walletChanged)
    Q_PROPERTY(QByteArray pin READ pin WRITE setPin NOTIFY pinChanged)
    QML_ELEMENT
public:
    LoginWithPinController(QObject* parent = nullptr);
    Wallet* wallet() const { return m_wallet; }
    void setWallet(Wallet* wallet);
    QByteArray pin() const { return m_pin; }
    void setPin(const QByteArray& pin);
signals:
    void walletChanged(Wallet* wallet);
    void pinChanged(const QByteArray& pin);
private slots:
    void update();
private:
    Connectable<Wallet> m_wallet;
    Connectable<Session> m_session;
    QByteArray m_pin;
};

class FeeEstimates : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Wallet* wallet READ wallet WRITE setWallet NOTIFY walletChanged)
    Q_PROPERTY(QJsonArray fees READ fees NOTIFY feesChanged)
    QML_ELEMENT
public:
    FeeEstimates(QObject* parent = nullptr);
    Wallet* wallet() const { return m_wallet; }
    void setWallet(Wallet* wallet);
    QJsonArray fees() const { return m_fees; }
signals:
    void walletChanged(Wallet* wallet);
    void feesChanged(const QJsonArray& fees);
private slots:
    void update();
private:
    Connectable<Wallet> m_wallet;
    QJsonArray m_fees;
    QTimer m_update_timer;
};

#endif // GREEN_WALLET_H
