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

#ifdef UPDATER
#include "updateschecker.h"

#include "platform.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QSysInfo>
#include <QLocale>

UpdatesChecker::UpdatesChecker(QObject *parent) :
    QThread(parent)
{
}

UpdatesChecker::~UpdatesChecker()
{
    delete mNetworkAccessManager;
}

void UpdatesChecker::run()
{
    // Get platform data
    QString osver;
    QString os = Platform::getPlatformName().toLower();
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    osver = QSysInfo::kernelVersion();
#else
#if defined(Q_OS_WIN)
    osver = QString::number(QSysInfo::WindowsVersion);
#elif defined(Q_OS_MAC)
    osver = QString::number(QSysInfo::MacintoshVersion);
#endif
#endif
    QString ver = "6.0";
    QString locale = QLocale::system().name();

    // Send check request
    QNetworkRequest request(QUrl("https://www.msec.it/blog/dukto/r5check.php?ver=" + ver + "&locale=" + locale + "&os=" + os + "&osver=" + osver));
    mNetworkAccessManager = new QNetworkAccessManager();
    connect(mNetworkAccessManager, &QNetworkAccessManager::finished, this, &UpdatesChecker::updatedDataReady);
    mNetworkAccessManager->get(request);

    exec();
}

// Read updates results
void UpdatesChecker::updatedDataReady(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) return;
    if (QString(reply->readAll()).isEmpty()) return;
    emit updatesAvailable();
}
#endif
