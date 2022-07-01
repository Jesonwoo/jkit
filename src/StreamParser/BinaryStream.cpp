#include "BinaryStream.h"

BinaryStream::BinaryStream(QObject *parent)
    : QObject{parent}
{

}

BinaryStream::~BinaryStream()
{
    closeStream();
}

bool BinaryStream::openStream(const QUrl &url)
{
    if(m_file.isOpen() && m_file.fileName() == url.toLocalFile()) {
        return true;
    }

    if(m_file.isOpen()) {
        closeStream();
    }

    m_file.setFileName(url.toLocalFile());
    if(!m_file.open(QIODevice::ReadOnly)) {
        qWarning() << "failed to open" << url.toLocalFile();
        return false;
    }
    return true;
}

void BinaryStream::closeStream()
{
    m_file.close();
}

qint64 BinaryStream::size()
{
    m_file.size();
}

QJsonArray BinaryStream::getHexList(qint64 offset, qint64 size, const qint64 unitSize)
{
    QJsonArray ret;
    QByteArray data;
    if(m_file.isOpen()) {
        size = qMin(size, m_file.size()-offset);
        if(size > 0 && m_file.seek(offset)) {
            data = m_file.read(size);
        }
    }

    if(!data.isEmpty()) {
        qint64 pos = 0;
        for(qint64 i = 0; i < size; i += unitSize) {
            QJsonObject obj;
            qint64 s = qMin(unitSize, size-i);
            obj.insert("hex_list", binaryToHexList(data.mid(pos, s)));
            obj.insert("ascii_list", binaryToAsciiList(data.mid(pos, s)));
            obj.insert("offset", offset + pos);
            obj.insert("size", s);
            ret.append(obj);

            pos += s;
        }
    }

    return ret;
}

QJsonArray BinaryStream::binaryToHexList(const QByteArray &data)
{
    QJsonArray list;
    for(qint32 i = 0; i < data.size(); ++i) {
        list.append(QString("%1").arg((uchar)data.at(i), 2, 16, QChar('0')).toUpper());
    }
    return list;
}

QJsonArray BinaryStream::binaryToAsciiList(const QByteArray &data)
{
    QJsonArray list;
    for(qint32 i = 0; i < data.size(); ++i) {
        QChar c((uchar)data.at(i));
        if(c.isDigit() || (c.toLatin1()>32 && c.toLatin1()<127)) {
            list.append(QString(c));
        } else {
            list.append(".");
        }
    }
    return list;
}
