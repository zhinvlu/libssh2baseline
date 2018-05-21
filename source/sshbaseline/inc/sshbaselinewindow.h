/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSshBaselinWindow_H
#define QSshBaselinWindow_H

#include <QDialog>
#include <QHash>
#include <QNetworkConfigurationManager>
#include <QtWidgets>
#include <QTcpSocket>
#include <QProcess>
#include "libssh2.h"

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QFile;
//class QFtp;
class QLabel;
class QLineEdit;
class QTableWidget;
class QTableWidgetItem;
//class QProgressDialog;
class QPushButton;
//class QUrlInfo;
//class QNetworkSession;
QT_END_NAMESPACE

class QSshBaselinWindow : public QDialog
{
    Q_OBJECT

public:
    explicit QSshBaselinWindow(QWidget *parent = 0);
    QSize sizeHint() const;
    ~QSshBaselinWindow();

private slots:
    void connectOrDisconnect(bool);
    void handleOneKeyScan();
    void readProcess();
    void finishedProcess();

private:
    QLabel *hostLabel;
    QLineEdit *hostLineEdit;
    QLabel *userLabel;
    QLineEdit *userLineEdit;
    QLabel *passWordLabel;
    QLineEdit *passWordLineEdit;
    QLabel *HostLabel;
    QLineEdit *HostLineEdit;
    QLabel *statusLabel;
    QTableWidget *baseLineItemTabWdgt;
    QPushButton *connectButton;
    QPushButton *oneKeyScan;
    QHBoxLayout *topLayout;
    QVBoxLayout *mainLayout;

    QProcess *runShellProcess;
    QString shellOutput;

    QTcpSocket *socket_;
    LIBSSH2_SESSION *session_;
    LIBSSH2_AGENT *agent_;

    quint16 port;

    int initSession();
    int closeSession();
    int runCommand(QString command);
    void closeChannel(LIBSSH2_CHANNEL *channel);
    void executeShellQProcess(QString);
    void handleConnected();
    void handleDisconnected();
};

#endif
