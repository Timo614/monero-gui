#include "WalletManager.h"
#include "UnsignedTransaction.h"
#include <QVector>
#include <QDebug>

UnsignedTransaction::Status UnsignedTransaction::status() const
{
    return static_cast<Status>(m_pimpl->status());
}

QString UnsignedTransaction::errorString() const
{
    return QString::fromStdString(m_pimpl->errorString());
}

quint64 UnsignedTransaction::fee(int index) const
{
    std::vector<uint64_t> arr = m_pimpl->fee();
    if(index > arr.size() - 1)
        return 0;
    return arr[index];
}

quint64 UnsignedTransaction::mixin(int index) const
{
    std::vector<uint64_t> arr = m_pimpl->mixin();
    if(index > arr.size() - 1)
        return 0;
    return arr[index];
}

quint64 UnsignedTransaction::txCount() const
{
    return m_pimpl->txCount();
}

quint64 UnsignedTransaction::minMixinCount() const
{
    return m_pimpl->minMixinCount();
}

QString UnsignedTransaction::confirmationMessage() const
{
    return QString::fromStdString(m_pimpl->confirmationMessage());
}

QStringList UnsignedTransaction::paymentId() const
{
    QList<QString> list;
    for (const auto &t: m_pimpl->paymentId())
        list.append(QString::fromStdString(t));
    return list;
}

bool UnsignedTransaction::sign(const QString &fileName) const
{
    if(!m_pimpl->sign(fileName.toStdString()))
        return false;
    // export key images
    return m_walletImpl->exportKeyImages(fileName.toStdString() + "_keyImages");
}

void UnsignedTransaction::setFilename(const QString &fileName)
{
    m_fileName = fileName;
}

QString UnsignedTransaction::destinations_formatted(int index) const
{
    QString destinations;
    for (auto const& t: transfers(index)) {
        if (!destinations.isEmpty())
          destinations += "<br>";
        destinations +=  WalletManager::instance()->displayAmount(t->amount()) + ": " + t->address();
    }
    return destinations;
}

QList<Transfer*> UnsignedTransaction::transfers(int index) const
{
    QList<Transfer*> transfers;
    auto &transaction_transfers = m_pimpl->transfers();
    if (index > transaction_transfers.size() - 1)
      return transfers;

    for(auto const& t: transaction_transfers[index]) {
        UnsignedTransaction * parent = const_cast<UnsignedTransaction*>(this);
        Transfer * transfer = new Transfer(t->amount(), QString::fromStdString(t->address()), parent);
        transfers.append(transfer);
    }
    return transfers;
}

UnsignedTransaction::UnsignedTransaction(Monero::UnsignedTransaction *pt, Monero::Wallet *walletImpl, QObject *parent)
    : QObject(parent), m_pimpl(pt), m_walletImpl(walletImpl)
{

}

UnsignedTransaction::~UnsignedTransaction()
{
    delete m_pimpl;
}
