#ifndef OPTION_ARBITRAGEUR_H
#define OPTION_ARBITRAGEUR_H

#include "market_watcher_interface.h"
#include "trade_executer_interface.h"

#include "utility.h"

template <typename Key, typename T> class QMap;
class OptionPricing;

struct DepthMarket {
    uint time;
    double askPrice;
    int askVolume;
    double bidPrice;
    int bidVolume;
};

QDebug operator<<(QDebug dbg, const DepthMarket &depthMarket);

class OptionArbitrageur : public QObject
{
    Q_OBJECT
protected:
    com::lazzyquant::market_watcher *pWatcher;
    com::lazzyquant::trade_executer *pExecuter;

    void updateOptions();
    void timesUp(int index);
    void loadOptionArbitrageurSettings();
    void preparePricing(const QStringList &options);

    QSet<QString> underlyingIDs;
    QMap<QString, double> thresholdMap;
    QMap<QString, OptionPricing*> pricingMap;

    int updateRetryCounter;
    int allowTradeNumber;

    //   期货合约    两档盘口
    QMap<QString, DepthMarket> future_market_data;
    //  标的期货合约       看涨/看跌       行权价   两档盘口
    QMap<QString, QMap<OPTION_TYPE, QMap<int, DepthMarket>>> option_market_data;

    // Argitrage strategies
    void findInefficientPrices(const QString &futureID, OPTION_TYPE type = CALL_OPT, int exercisePrice = 0);
    void findCheapCallOptions(const QString &futureID, double threshold);
    void checkCheapCallOptions(const QString &futureID, int exercisePrice, double threshold);
    void findCheapPutOptions(const QString &futureID, double threshold);
    void checkCheapPutOptions(const QString &futureID, int exercisePrice, double threshold);
    void findReversedCallOptions(const QString &futureID, int exercisePriceToCheck);
    void checkReversedCallOptions(const QString &futureID, QMap<int, DepthMarket> &callOptionMap, int lowExercisePrice, int highExercisePrice);
    void findReversedPutOptions(const QString &futureID, int exercisePriceToCheck);
    void checkReversedPutOptions(const QString &futureID, QMap<int, DepthMarket> &putOptionMap, int lowExercisePrice, int highExercisePrice);

    void fishing(const QStringList &options, int vol, double price);
    void manageMoney(int vol = 1);

public:
    explicit OptionArbitrageur(int number = 0, const bool replayMode = false, const QString replayDate = QString(), QObject *parent = 0);
    ~OptionArbitrageur();

signals:

private slots:
    void onMarketData(const QString& instrumentID, uint time, double lastPrice, int volume,
                      double askPrice1, int askVolume1, double bidPrice1, int bidVolume1);
};

#endif // OPTION_ARBITRAGEUR_H
