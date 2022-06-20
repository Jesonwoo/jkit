#ifndef STREAMPARSER_H
#define STREAMPARSER_H

#include <QObject>
#include <QUrl>
#include <QJsonObject>
#include <libmedia/libmedia.h>

class StreamParser : public QObject
{
    Q_OBJECT
public:
    explicit StreamParser(QObject *parent = nullptr);

    Q_INVOKABLE void openStream(const QString &url);
    Q_INVOKABLE bool startParseStream(void);
    Q_INVOKABLE bool closeStream(void);
    Q_INVOKABLE QJsonObject streamInfo(void);
    Q_INVOKABLE qint32 naluCount(void);
    Q_INVOKABLE QJsonObject getNaluInfo(qint32 idx);
    Q_INVOKABLE QString getNaluSyntax(qint32 idx);
signals:
    void openFinished(const QJsonObject & info);
    void startFinished();
    void closeFinished();

private:
    QJsonObject streamInfoToJsonObj(const jkit::H264StreamInfo &info);
    QJsonObject naluToJsonObj(const jkit::Nalu &n);
    QString getFrameType(const jkit::Nalu &n);

private:
    bool m_isOpen = false;
    bool m_isStart = false;
    QScopedPointer<jkit::H264Parser> m_h264Parser;
};

#endif // STREAMPARSER_H
