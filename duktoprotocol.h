/* DUKTO - A simple, fast and multi-platform file transfer tool for LAN users
 * Copyright (C) 2011 Emanuele Colombo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef DUKTOPROTOCOL_H
#define DUKTOPROTOCOL_H

#include <QObject>

#include <QtNetwork/QUdpSocket>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QHash>
#include <QFile>
#include <QStringList>

#include "peer.h"

class DuktoProtocol : public QObject
{
    Q_OBJECT

public:
    DuktoProtocol(QObject *parent = nullptr);
    virtual ~DuktoProtocol();
    bool setupUdpServer(quint16 port);
    bool setupTcpServer(quint16 port);
    void closeServers();
    void sayHello(const QHostAddress &dest);
    void sayHello(const QHostAddress &dest, qint16 port);
    void sayGoodbye();
    inline QHash<QString, Peer>& getPeers() { return mPeers; }
    void sendFile(const QString &ipDest, qint16 port, const QStringList &files);
    void sendText(const QString &ipDest, qint16 port, const QString &text);
    void sendScreen(const QString &ipDest, qint16 port, const QString &path);
    inline bool isBusy() { return mIsSending || mIsReceiving; }
    void abortCurrentTransfer();
    void updateBuddyName();
    
public slots:
    void newUdpData();
    void newIncomingConnection();
    void readNewData();
    void closedConnection();
    void closedConnectionTmp();
    void sendMetaData();
    void sendData(qint64 b);
    void sendConnectError(QAbstractSocket::SocketError);

signals:
     void peerListAdded(Peer peer);
     void peerListRemoved(Peer peer);
     void sendFileComplete();
     void sendFileError(int code);
     void sendFileAborted();
     void receiveFileStart(QString senderIp);
     void receiveFileComplete(QStringList *files, qint64 totalSize);
     void receiveTextComplete(QString *text, qint64 totalSize);
     void receiveFileCancelled();
     void transferStatusUpdate(qint64 total, qint64 partial);

private:
    QByteArray getSystemSignature();
    QStringList expandTree(const QStringList& files);
    void addRecursive(QStringList& e, const QString &path);
    qint64 computeTotalSize(const QStringList& e);
    QByteArray nextElementHeader();
    void sendToAllBroadcast(const QByteArray& packet, const QList<qint16>& ports);
    void closeCurrentTransfer(bool aborted = false);

    void handleMessage(const QByteArray &data, const QHostAddress &sender);
    void updateStatus();

    QUdpSocket *mSocket = nullptr;              // Socket UDP segnalazione
    QTcpServer *mTcpServer = nullptr;           // Socket TCP attesa dati
    QTcpSocket *mCurrentSocket = nullptr;       // Socket TCP dell'attuale trasferimento file

    QHash<QString, Peer> mPeers;                // Elenco peer individuati

    // Send and receive members
    qint16 mLocalUdpPort;
    qint16 mLocalTcpPort;
    bool mIsSending = false;
    bool mIsReceiving = false;
    QFile *mCurrentFile = nullptr;              // Puntatore al file aperto corrente
    qint64 mTotalSize = 0;                      // Quantità totale di dati da inviare o ricevere
    int mFileCounter = 0;                       // Puntatore all'elemento correntemente da trasmettere o ricevere

    // Sending members
    QStringList mFilesToSend;                   // Elenco degli elementi da trasmettere
    qint64 mSentData = 0;                       // Quantità di dati totale trasmessi
    qint64 mSentBuffer = 0;                    // Quantità di dati rimanenti nel buffer di trasmissione
    QString mBasePath;                          // Percorso base per l'invio di file e cartelle
    QString mTextToSend;                        // Testo da inviare (in caso di invio testuale)
    bool mSendingScreen = false;                // Flag che indica se si sta inviando uno screenshot

    // Receive members
    qint64 mElementsToReceiveCount = 0;         // Numero di elementi da ricevere
    qint64 mTotalReceivedData = 0;              // Quantità di dati ricevuti totale
    qint64 mElementReceivedData = 0;            // Quantità di dati ricevuti per l'elemento corrente
    qint64 mElementSize = 0;                    // Dimensione dell'elemento corrente
    QString mRootFolderName;                    // Nome della cartella principale ricevuta
    QString mRootFolderRenamed;                 // Nome della cartella principale da utilizzare
    QStringList *mReceivedFiles = nullptr;      // Elenco degli elementi da trasmettere
    QByteArray mTextToReceive;                  // Testo ricevuto in caso di invio testo
    bool mReceivingText = false;                // Ricezione di testo in corso
    QByteArray mPartialName;                    // Nome prossimo file letto solo in parte
    enum RecvStatus {
        FILENAME,
        FILESIZE,
        DATA
    } mRecvStatus = FILENAME;

};

#endif // DUKTOPROTOCOL_H
