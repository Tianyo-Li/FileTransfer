#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QDateTime>

#define DEFAULT_PORT 12345
#define CHUNK_SIZE (64 * 1024)  // 64KB分块

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , server(nullptr)
    , socket(nullptr)
    , currentFileIndex(0)
    , currentFile(nullptr)
    , currentFilePos(0)
    , currentFileSize(0)
    , receivingFile(nullptr)
    , receivingFileSize(0)
    , receivedBytes(0)
{
    ui->setupUi(this);

    // 设置默认端口
    ui->portLineEdit->setText(QString::number(DEFAULT_PORT));

    // 连接按钮信号
    connect(ui->startServerBtn, &QPushButton::clicked, this, &MainWindow::onStartServer);
    connect(ui->connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectToServer);
    connect(ui->selectFileBtn, &QPushButton::clicked, this, &MainWindow::onSelectFile);
    connect(ui->selectDirBtn, &QPushButton::clicked, this, &MainWindow::onSelectDirectory);
    connect(ui->sendBtn, &QPushButton::clicked, this, &MainWindow::onSend);

    // 设置保存目录为当前程序目录下的received文件夹
    saveDirectory = QDir::current().absoluteFilePath("received");
    QDir().mkpath(saveDirectory);

    updateLog("程序已启动，保存目录: " + saveDirectory);
}

MainWindow::~MainWindow()
{
    if (currentFile) delete currentFile;
    if (receivingFile) delete receivingFile;
    if (socket) socket->deleteLater();
    if (server) server->deleteLater();
    delete ui;
}

void MainWindow::updateLog(const QString &msg)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    ui->logTextEdit->append(QString("[%1] %2").arg(timestamp).arg(msg));
}

void MainWindow::onStartServer()
{
    if (server) {
        server->close();
        delete server;
        server = nullptr;
    }

    int port = ui->portLineEdit->text().toInt();
    if (port <= 0 || port > 65535) port = DEFAULT_PORT;

    server = new QTcpServer(this);
    if (!server->listen(QHostAddress::Any, port)) {
        updateLog("启动服务器失败: " + server->errorString());
        return;
    }

    connect(server, &QTcpServer::newConnection, this, &MainWindow::onNewConnection);
    updateLog(QString("服务器已启动，监听端口 %1").arg(port));
    ui->statusLabel->setText("服务器模式 - 等待连接");
}

void MainWindow::onNewConnection()
{
    if (socket) {
        socket->deleteLater();
    }

    socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, [this]() {
        updateLog("客户端已断开连接");
        ui->statusLabel->setText("未连接");
    });

    updateLog("客户端已连接: " + socket->peerAddress().toString());
    ui->statusLabel->setText("已连接 - 等待接收文件");
}

void MainWindow::onConnectToServer()
{
    QString ip = ui->ipLineEdit->text();
    if (ip.isEmpty()) {
        updateLog("请输入服务器IP地址");
        return;
    }

    int port = ui->portLineEdit->text().toInt();
    if (port <= 0 || port > 65535) port = DEFAULT_PORT;

    if (socket) {
        socket->deleteLater();
    }

    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, [this]() {
        updateLog("已连接到服务器");
        ui->statusLabel->setText("已连接 - 可以发送文件");
    });
    connect(socket, &QTcpSocket::errorOccurred, this, [this](QAbstractSocket::SocketError) {
        updateLog("连接错误: " + socket->errorString());
    });
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::onReadyRead);

    socket->connectToHost(ip, port);
    updateLog(QString("正在连接 %1:%2...").arg(ip).arg(port));
}

void MainWindow::onSelectFile()
{
    QStringList files = QFileDialog::getOpenFileNames(this, "选择要发送的文件");
    if (files.isEmpty()) return;

    for (const QString &file : files) {
        if (!fileList.contains(file)) {
            fileList.append(file);
            ui->fileListWidget->addItem(file);
        }
    }
    updateLog(QString("已添加 %1 个文件，共 %2 个待发送").arg(files.size()).arg(fileList.size()));
}

void MainWindow::onSelectDirectory()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, "选择要发送的目录");
    if (dirPath.isEmpty()) return;

    // 递归收集目录下所有文件
    QDir dir(dirPath);
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    int addedCount = 0;
    for (const QFileInfo &entry : entries) {
        if (entry.isFile()) {
            if (!fileList.contains(entry.absoluteFilePath())) {
                fileList.append(entry.absoluteFilePath());
                ui->fileListWidget->addItem(entry.absoluteFilePath());
                addedCount++;
            }
        } else if (entry.isDir()) {
            // 递归子目录（这里简化为只处理一层，实际项目可以用QDirIterator）
            QDir subDir(entry.absoluteFilePath());
            QFileInfoList subFiles = subDir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);
            for (const QFileInfo &subFile : subFiles) {
                if (!fileList.contains(subFile.absoluteFilePath())) {
                    fileList.append(subFile.absoluteFilePath());
                    ui->fileListWidget->addItem(subFile.absoluteFilePath());
                    addedCount++;
                }
            }
        }
    }

    updateLog(QString("从目录 %1 添加了 %2 个文件").arg(dirPath).arg(addedCount));
}

void MainWindow::onSend()
{
    if (!socket || socket->state() != QAbstractSocket::ConnectedState) {
        updateLog("未连接到服务器，无法发送");
        return;
    }

    if (fileList.isEmpty()) {
        updateLog("没有待发送的文件，请先选择文件或目录");
        return;
    }

    currentFileIndex = 0;
    sendNextFile();
}

void MainWindow::sendNextFile()
{
    if (currentFileIndex >= fileList.size()) {
        updateLog("所有文件发送完成！");
        ui->progressBar->setValue(100);
        ui->statusLabel->setText("发送完成");
        fileList.clear();
        ui->fileListWidget->clear();
        return;
    }

    QString filePath = fileList[currentFileIndex];
    if (currentFile) {
        delete currentFile;
        currentFile = nullptr;
    }

    currentFile = new QFile(filePath);
    if (!currentFile->open(QIODevice::ReadOnly)) {
        updateLog("无法打开文件: " + filePath);
        currentFileIndex++;
        sendNextFile();
        return;
    }

    currentFileSize = currentFile->size();
    currentFilePos = 0;

    // 提取文件名（不含路径）
    QString fileName = QFileInfo(filePath).fileName();
    sendFileHeader(fileName, currentFileSize);
}

void MainWindow::sendFileHeader(const QString &fileName, qint64 fileSize)
{
    // 格式: "FILE:文件名|文件大小\n"
    QString header = QString("FILE:%1|%2\n").arg(fileName).arg(fileSize);
    socket->write(header.toUtf8());
    updateLog(QString("开始发送文件: %1 (%2 bytes)").arg(fileName).arg(fileSize));

    // 开始发送数据
    sendFileData();
}

void MainWindow::sendFileData()
{
    if (!currentFile) return;

    while (currentFilePos < currentFileSize) {
        qint64 remaining = currentFileSize - currentFilePos;
        qint64 toRead = qMin(CHUNK_SIZE, remaining);
        QByteArray chunk = currentFile->read(toRead);

        if (chunk.isEmpty()) break;

        qint64 written = socket->write(chunk);
        if (written == -1) {
            updateLog("发送数据失败: " + socket->errorString());
            return;
        }

        currentFilePos += written;

        // 更新进度
        int percent = (int)(currentFilePos * 100 / currentFileSize);
        ui->progressBar->setValue(percent);

        // 避免事件循环阻塞
        QCoreApplication::processEvents();
    }

    if (currentFilePos >= currentFileSize) {
        // 文件发送完成
        currentFile->close();
        updateLog(QString("文件发送完成: %1").arg(QFileInfo(fileList[currentFileIndex]).fileName()));
        currentFileIndex++;
        sendNextFile();  // 发送下一个文件
    }
}

void MainWindow::onReadyRead()
{
    if (!socket) return;

    receiveBuffer.append(socket->readAll());

    // 处理接收缓冲区中的所有完整消息
    while (true) {
        // 查找消息头 "FILE:" 或数据块
        if (receivingFile == nullptr) {
            // 未在接收文件，尝试解析文件头
            int headerEnd = receiveBuffer.indexOf('\n');
            if (headerEnd == -1) break;  // 头不完整，等待更多数据

            QByteArray headerLine = receiveBuffer.left(headerEnd);
            receiveBuffer.remove(0, headerEnd + 1);

            if (headerLine.startsWith("FILE:")) {
                // 解析 "FILE:文件名|文件大小"
                QByteArray content = headerLine.mid(5);
                int pipePos = content.indexOf('|');
                if (pipePos != -1) {
                    QString fileName = QString::fromUtf8(content.left(pipePos));
                    qint64 fileSize = content.mid(pipePos + 1).toLongLong();
                    receiveFile(fileName, fileSize);
                }
            }
        } else {
            // 正在接收文件，写入数据
            if (receiveBuffer.isEmpty()) break;

            qint64 needBytes = receivingFileSize - receivedBytes;
            qint64 toWrite = qMin(needBytes, (qint64)receiveBuffer.size());

            receivingFile->write(receiveBuffer.left(toWrite));
            receivedBytes += toWrite;
            receiveBuffer.remove(0, toWrite);

            // 更新进度
            int percent = (int)(receivedBytes * 100 / receivingFileSize);
            ui->progressBar->setValue(percent);

            if (receivedBytes >= receivingFileSize) {
                // 文件接收完成
                receivingFile->close();
                delete receivingFile;
                receivingFile = nullptr;
                updateLog(QString("文件接收完成: %1").arg(receivingFileName));
                ui->statusLabel->setText("接收完成");
            }
        }
    }
}

void MainWindow::receiveFile(const QString &fileName, qint64 fileSize)
{
    receivingFileName = fileName;
    receivingFileSize = fileSize;
    receivedBytes = 0;

    QString savePath = QDir(saveDirectory).absoluteFilePath(fileName);
    receivingFile = new QFile(savePath);
    if (!receivingFile->open(QIODevice::WriteOnly)) {
        updateLog("无法创建文件: " + savePath);
        receivingFile = nullptr;
        return;
    }

    updateLog(QString("开始接收文件: %1 (%2 bytes)").arg(fileName).arg(fileSize));
    ui->statusLabel->setText(QString("正在接收: %1").arg(fileName));
}

// 以下函数在本简版中暂未使用，保留接口
void MainWindow::onBytesWritten(qint64 bytes) { Q_UNUSED(bytes); }
void MainWindow::parseReceivedData(const QByteArray &data) { Q_UNUSED(data); }
void MainWindow::appendFileData(const QByteArray &data) { Q_UNUSED(data); }