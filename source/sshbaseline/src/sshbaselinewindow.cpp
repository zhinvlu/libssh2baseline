/****************************************************************************
**
**
****************************************************************************/
#include "sshbaseline/inc/sshbaselinewindow.h"

QSshBaselinWindow::QSshBaselinWindow(QWidget *parent)
    : QDialog(parent)
{
    hostLabel = new QLabel(this);
    hostLabel->setText(tr("&主机:"));
    hostLineEdit = new QLineEdit(this);
    hostLabel->setBuddy(hostLineEdit);
    userLabel = new QLabel(this);
    userLabel->setText(tr("&用户:"));
    userLineEdit = new QLineEdit(this);
    userLabel->setBuddy(userLineEdit);
    passWordLabel = new QLabel();
    passWordLabel->setText(tr("&密码:"));
    passWordLineEdit = new QLineEdit(this);
    passWordLabel->setBuddy(passWordLineEdit);

    statusLabel = new QLabel(this);

    baseLineItemTabWdgt = new QTableWidget(this);
    baseLineItemTabWdgt->setColumnCount(4);
    baseLineItemTabWdgt->setHorizontalHeaderLabels(QStringList() << tr("序号") << tr("加固项") << tr("状态") << tr("描述") );
    baseLineItemTabWdgt->horizontalHeader()->setStretchLastSection(true);

    connectButton = new QPushButton(this);
    connectButton->setText(tr("连接"));
    connectButton->setCheckable(true);
    connect(connectButton, SIGNAL(toggled(bool)), this, SLOT(connectOrDisconnect(bool)));

    oneKeyScan = new QPushButton(this);
    oneKeyScan->setEnabled(false);
    oneKeyScan->setText(tr("一键扫描"));
    connect(oneKeyScan, SIGNAL(clicked()), this, SLOT(handleOneKeyScan()));

    topLayout = new QHBoxLayout();
    topLayout->addWidget(hostLabel);
    topLayout->addWidget(hostLineEdit);
    topLayout->addWidget(userLabel);
    topLayout->addWidget(userLineEdit);
    topLayout->addWidget(passWordLabel);
    topLayout->addWidget(passWordLineEdit);
    topLayout->addWidget(connectButton);
    topLayout->addWidget(oneKeyScan);

    mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(baseLineItemTabWdgt);
    mainLayout->addWidget(statusLabel);
    setLayout(mainLayout);

    setWindowTitle(tr("DEMO"));

    port = 22;
    session_ = NULL;
    socket_ = NULL;
    runShellProcess = new QProcess(this);
    connect(runShellProcess, SIGNAL(readyRead()), this, SLOT(readProcess()));
    connect(runShellProcess, SIGNAL(finished(int)), this, SLOT(finishedProcess()));
}

QSize QSshBaselinWindow::sizeHint() const
{
    return QSize(920, 580);
}

void QSshBaselinWindow::executeShellQProcess(QString shell){
    shellOutput="";
    runShellProcess->start(shell);
}

void QSshBaselinWindow::readProcess(){
    QString output=runShellProcess->readAll();
     shellOutput+=output;
}

void QSshBaselinWindow::finishedProcess(){
    qDebug()<<shellOutput;
}

void QSshBaselinWindow::handleOneKeyScan()
{
    QString host_ip=hostLineEdit->text().simplified();
    QString user_name=userLineEdit->text().simplified();
    QString pass_word=passWordLineEdit->text().simplified();

    QString command = QString("./sshpass -p %1 ssh -o StrictHostKeyChecking=no %2@%3 \'bash -s\' < test.sh").arg(pass_word,user_name,host_ip);
//    QString command = QString("ssh %1@%2 -C \'bash -s\' < test.sh").arg(user_name,host_ip);
    qDebug()<<"command:"<<command;
//        runCommand("\"bash -s\" < test.sh");
//        QProcess::execute(command);
        system(command.toUtf8());
//    executeShellQProcess(command);
}

void QSshBaselinWindow::handleConnected()
{
    int ret;

    connectButton->setEnabled(false);
    connectButton->setText(tr("断开"));
#ifndef QT_NO_CURSOR
    setCursor(Qt::WaitCursor);
#endif
    ret = initSession();
    if(ret != 1)
    {
        handleDisconnected();

        QMessageBox box(QMessageBox::Critical,"优炫软件提示","SSH连接错误，错误码：" + QString::number(ret,10));
//        QIcon icon(":/pic/uxcdps_std");box.setWindowIcon(icon);
        box.setStandardButtons (QMessageBox::Yes);
        box.setButtonText (QMessageBox::Yes,QString("确 定"));
        box.exec();

        return;
    }
    statusLabel->setText(tr("已连接"));
    oneKeyScan->setEnabled(true);
    setCursor(Qt::ArrowCursor);
    connectButton->setEnabled(true);
    handleOneKeyScan();
}

void QSshBaselinWindow::handleDisconnected()
{
    oneKeyScan->setEnabled(false);
    connectButton->setEnabled(true);
    connectButton->setText(tr("连接"));
#ifndef QT_NO_CURSOR
    setCursor(Qt::ArrowCursor);
#endif
    statusLabel->setText(tr("未连接"));
    closeSession();
}

void QSshBaselinWindow::connectOrDisconnect(bool checked)
{
    if (checked) {
        handleConnected();
    }else{
        handleDisconnected();
    }
}

int QSshBaselinWindow::initSession()
{
    QString host_ip=hostLineEdit->text().simplified();
    QString user_name=userLineEdit->text().simplified();
    QString pass_word=passWordLineEdit->text().simplified();

    //---------- connection --------------
    int libssh2_error = libssh2_init(0);
    if(libssh2_error)
    {
        qDebug("libssh2_init() error: %d", libssh2_error);
        return -2;
    }        
    socket_=new QTcpSocket(this);
    socket_->connectToHost(host_ip, port);
    if(!socket_->waitForConnected(5000))
    {
        qDebug("Error connecting to host %s", host_ip.toLocal8Bit().constData());
        return -1;
    }
    session_ = libssh2_session_init();
    if(!session_)
    {
        qDebug("libssh2_session_init() failed");
        return -2;
    }
    libssh2_session_set_timeout(session_,5000);
    libssh2_error = libssh2_session_handshake(session_, socket_->socketDescriptor());

    if(libssh2_error)
    {
        qDebug("libssh2_session_startup() error: %d", libssh2_error);
        return -3;
    }

    libssh2_userauth_list(session_, user_name.toLocal8Bit().constData(), user_name.toLocal8Bit().length());
    if(libssh2_userauth_password(
                session_,
                user_name.toLocal8Bit().constData(),
                pass_word.toLocal8Bit().constData()
                ))
    {
        qDebug("Password authentication failed");
        socket_->disconnectFromHost();
        libssh2_session_disconnect(session_, "Client disconnecting for error");
        libssh2_session_free(session_);
        libssh2_exit();
        return -4;
    }
    return 1;
}

int QSshBaselinWindow::closeSession()
{
    if(socket_ != NULL && socket_->isOpen())
    {
        socket_->disconnectFromHost();
        socket_->deleteLater();
        socket_=0;
    }
    if(session_ != NULL)
    {
        libssh2_session_disconnect(session_, "Client disconnecting normally");
        libssh2_session_free(session_);
        libssh2_exit();
        session_=NULL;
    }
    return 1;
}

int QSshBaselinWindow::runCommand(QString command)
{
    //------------setup channel ----------------------
    LIBSSH2_CHANNEL *channel = NULL;
    channel = libssh2_channel_open_session(session_);
    if ( channel == NULL )
    {
        qDebug()<<"Failed to open a new channel\n";
        socket_->disconnectFromHost();
        return -1;
    }

    libssh2_channel_handle_extended_data(channel,LIBSSH2_CHANNEL_EXTENDED_DATA_NORMAL);

    libssh2_channel_set_blocking(channel, 1);
    int rc;
    while ((rc=libssh2_channel_exec(channel, command.toLocal8Bit().constData()))==LIBSSH2_ERROR_EAGAIN );
    if (rc)
    {
        return -1;
    }

    //-------read channel-----------
    int read;
    QString stdout_str;
    QString stderr_str;
    while(true)
    {
        QByteArray byte_array;
        byte_array.resize(4096);
        char* buffer=byte_array.data();
        int buffer_size=byte_array.size();
        read = libssh2_channel_read(channel, buffer, buffer_size-10);
        if(read>0)
        {
            QByteArray debug = QByteArray(buffer, read);
            stdout_str.append(debug);
        }
        if(LIBSSH2_ERROR_EAGAIN == read)
        {
            qDebug("LIBSSH2_ERROR_EAGAIN");
            break;
        }
        else if(read  < 0)
        {
            qDebug(" error reading from std channel");
            closeChannel(channel);
            goto next_channel;
        }
        read = libssh2_channel_read_stderr(channel, buffer, buffer_size-10);
        if(read>0)
        {
            QByteArray debug = QByteArray(buffer, read);
            stderr_str.append(debug);
        }
        if(LIBSSH2_ERROR_EAGAIN == read)
        {
            qDebug("LIBSSH2_ERROR_EAGAIN");
            break;
        }
        else if(read  < 0)
        {
            qDebug(" error reading from stderr channel");
            closeChannel(channel);
            goto next_channel;
        }

        int i = libssh2_channel_eof(channel);
        if(i)
        {
            closeChannel(channel);
            goto next_channel;
        }
    }
    next_channel:
    if(!stdout_str.isEmpty())
    {
        qDebug()<<"STDOUT:\n"<<stdout_str;
    }
    if(!stderr_str.isEmpty())
    {
        qDebug()<<"STDERR:\n"<<stderr_str;
    }

    return 1;
}

void QSshBaselinWindow::closeChannel(LIBSSH2_CHANNEL *channel)
{
    if(channel)
    {
        libssh2_channel_close(channel);
        libssh2_channel_free(channel); // free will auto close the channel
    }
}

QSshBaselinWindow::~QSshBaselinWindow()
{
    if(topLayout)
    {
        topLayout->deleteLater();
        topLayout = NULL;
    }
    if(mainLayout)
    {
        mainLayout->deleteLater();
        mainLayout = NULL;
    }
    closeSession();
}
