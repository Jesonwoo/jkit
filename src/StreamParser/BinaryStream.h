#ifndef BINARYSTREAM_H
#define BINARYSTREAM_H

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QScopedPointer>
#include <QMap>
#include <QUrl>
#include <QJsonObject>
#include <QJsonArray>

class BinaryStream : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 size READ size)

public:
    BinaryStream(QObject *parent = nullptr);
    ~BinaryStream();

    Q_INVOKABLE bool openStream(const QUrl & url);
    Q_INVOKABLE void closeStream(void);
    // 获取二进制结果
    Q_INVOKABLE QJsonArray getHexList(qint64 offset, qint64 size, const qint64 unitSize = 16);

    qint64 size();

private:
    QJsonArray binaryToHexList(const QByteArray & data);
    QJsonArray binaryToAsciiList(const QByteArray & data);

private:
    QFile m_file;
    const quint32 m_chunk = 16;
};

#endif // BINARYSTREAM_H
