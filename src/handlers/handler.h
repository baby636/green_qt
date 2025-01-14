#ifndef GREEN_HANDLER_H
#define GREEN_HANDLER_H

#include <QtQml>
#include <QObject>
#include <QJsonObject>

QT_FORWARD_DECLARE_CLASS(Resolver)
QT_FORWARD_DECLARE_CLASS(TwoFactorResolver)
QT_FORWARD_DECLARE_CLASS(Wallet)

QT_FORWARD_DECLARE_STRUCT(GA_session)
QT_FORWARD_DECLARE_STRUCT(GA_auth_handler)

#include <QFutureWatcher>

class Handler : public QFutureWatcher<void>
{
    Q_OBJECT
    Q_PROPERTY(Wallet* wallet READ wallet CONSTANT)
    Q_PROPERTY(QJsonObject result READ result NOTIFY resultChanged)
    QML_ELEMENT
    QML_UNCREATABLE("Handler is an abstract base class.")
public:
    Handler(Wallet* wallet);
    virtual ~Handler();
    Wallet* wallet() const;
    void exec();
    void fail();
    const QJsonObject& result() const;
public slots:
    void request(const QByteArray& method);
    void resolve(const QJsonObject& data);
    void resolve(const QByteArray& data);
signals:
    void resultChanged(const QJsonObject& result);
    void done();
    void error();
    void requestCode();
    void invalidCode();
    void resolver(Resolver* resolver);
private:
    virtual void call(GA_session* session, GA_auth_handler** auth_handler) = 0;
    void step();
    Resolver* createResolver(const QJsonObject& result);
    void setResult(const QJsonObject &result);
private:
    bool m_already_exec{false};
    Wallet* const m_wallet;
    GA_auth_handler* m_auth_handler{nullptr};
    TwoFactorResolver* m_two_factor_resolver{nullptr};
    QJsonObject m_result;
    QJsonObject m_error_details;
};

#endif // GREEN_HANDLER_H
