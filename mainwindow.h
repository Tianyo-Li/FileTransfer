#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QFile>
#include <QList>
#include <QProgressBar>
#include <QTextEdit>
#include <QListWidget>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartServer();
    void onConnectToServer();
    void onSelectFile();
    void onSelectDirectory();
    void onSend();
    void onNewConnection();
    void onReadyRead();
    void onBytesWritten(qint64 bytes);
    void updateLog(const QString &msg);

private:
    Ui::MainWindow *ui;
    QTcpServer *server;
    QTcpSocket *socket;
    QList<QString> fileList;      // 待发送文件列表
    int currentFileIndex;         // 当前发送到第几个文件
    QFile *currentFile;            // 当前打开的文件
    qint64 currentFilePos;         // 当前文件已发送位置
    qint64 currentFileSize;        // 当前文件总大小
    QByteArray receiveBuffer;      // 接收缓冲区（处理粘包）

    void sendNextFile();           // 发送下一个文件
    void sendFileHeader(const QString &fileName, qint64 fileSize);
    void sendFileData();           // 发送文件数据块
    void parseReceivedData(const QByteArray &data);
    void receiveFile(const QString &fileName, qint64 fileSize);
    void appendFileData(const QByteArray &data);

    QString saveDirectory;          // 接收文件保存目录
    QFile *receivingFile;
    qint64 receivingFileSize;
    qint64 receivedBytes;
    QString receivingFileName;
};

#endif