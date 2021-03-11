#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QPushButton>
#include <QPlainTextEdit>

#include <map>
#include <string>
#include <iostream>

QT_BEGIN_NAMESPACE
class QFile;
class QLabel;
class QLineEdit;
class QPushButton;
class QNetworkReply;
class QCheckBox;
QT_END_NAMESPACE

class ProgressDialog : public QProgressDialog {
    Q_OBJECT

public:
    explicit ProgressDialog(const QUrl &url, QWidget *parent = nullptr);

public slots:
   void sendRequestsProgress(qint64 bytesRead, qint64 totalBytes);
};

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateRequest();

private:
    void setRequestRawHeaders();
    void startSendingRequests();
    void setUrl(QString newHost);
    void updateVisibleHeaders(QString newHost);
    void updateVisibleHostline(QString newHost);
    QString debugPlainTextEditorSetup(std::map<QString, QString> map);

    QLineEdit *hostLine_edit;
    QLineEdit *threadsline_edit;
    QCheckBox *priorityCheckBox;
    QPushButton *startButton;
    QPlainTextEdit *requestEditor;

    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkRequest request;
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> reply;
    bool testingAborted = false;
    std::map<QString, QString> headers;
};
#endif
