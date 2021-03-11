#include <QtWidgets>
#include <QtNetwork>
#include <QUrl>
#include <QDebug>

#include <vector>

#include "mainwindow.h"
#include "ui_mainwindow.h"

const char defaulHost[] = "http://localhost:1234";
const char defaultThreads[] = "1";
const char defaultRequest[] = "Host: localhost:1234\r\nConnection: keep-alive\r\nDNT: 1\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 11_2_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/88.0.4324.192 Safari/537.36\r\nAccept-Encoding: gzip, deflate, br\r\nAccept-Language: ru-RU,ru;q=0.9,en-GB;q=0.8,en;q=0.7,en-US;q=0.6\r\n\n";

ProgressDialog::ProgressDialog(const QUrl &url, QWidget *parent) : QProgressDialog(parent) {
    setWindowTitle(tr("Requests Progress"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setLabelText(tr("Downloading %1.").arg(url.toDisplayString()));
    setMinimum(0);
    setValue(0);
    setMinimumDuration(0);
    setMinimumSize(QSize(400, 75));
}

void ProgressDialog::sendRequestsProgress(qint64 bytesRead, qint64 totalBytes) {
    setMaximum(totalBytes);
    setValue(bytesRead);
}

MainWindow::MainWindow(QWidget *parent) : QDialog(parent), hostLine_edit(new QLineEdit(defaulHost)), threadsline_edit(new QLineEdit(defaultThreads)), startButton(new QPushButton(tr("Start"))), requestEditor(new QPlainTextEdit()), priorityCheckBox(new QCheckBox("Set priority for \"Host\" field.")){
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle(tr("Multithread Requests Sender"));

    requestEditor->setPlainText(defaultRequest);
    priorityCheckBox->setChecked(true);

    QFormLayout *formLayout = new QFormLayout;
    hostLine_edit->setClearButtonEnabled(true);

    formLayout->addRow(tr("&Host: "), hostLine_edit);
    formLayout->addRow(priorityCheckBox);
    formLayout->addRow(tr("&Threads: "), threadsline_edit);
    formLayout->addRow(tr("&Headers: "), requestEditor);

    // ----- debug -----
    //QPlainTextEdit *debugTextEditor = new QPlainTextEdit();
    //formLayout->addRow(tr("&Debug"), debugTextEditor);
    //setRequestRawHeaders();
    //debugTextEditor->setPlainText(debugPlainTextEditorSetup(headers));
    // -----------------

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    startButton->setAutoDefault(true);
    connect(startButton, &QAbstractButton::clicked, this, &MainWindow::updateRequest);

    QPushButton *quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);
    connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(startButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(quitButton, QDialogButtonBox::RejectRole);

    mainLayout->addWidget(buttonBox);
    hostLine_edit->setFocus();
}

MainWindow::~MainWindow() = default;

void MainWindow::updateRequest() {
    headers.clear();
    setRequestRawHeaders();

    const QString hostlineText = hostLine_edit->text();
    QString hostFromHeaders;
    QString newHost;

    for(auto elem : headers) {
        if(elem.first == "Host") {
            hostFromHeaders = elem.second;
            break;
        }
    }

    if(!(hostlineText == hostFromHeaders)) {
        if(priorityCheckBox->isChecked())
            {newHost = hostlineText; setUrl(newHost); updateVisibleHeaders(newHost);}
        else
            {newHost = hostFromHeaders; setUrl(newHost); updateVisibleHostline(newHost);}
    }

    startSendingRequests();
}

void MainWindow::setUrl(QString newHost) {
    if(!newHost.startsWith("http://"))
        newHost = "http://" + newHost;
    url = QUrl(newHost);
}

void MainWindow::updateVisibleHostline(QString newHost) {
    QString newVisibleHostline = "";

    if(newHost.startsWith("http://"))
        newVisibleHostline = newHost;
    else
        newVisibleHostline = "http://" + newHost;

    hostLine_edit->setText(newVisibleHostline);
}

void MainWindow::updateVisibleHeaders(QString newHost) {
    QString newVisibleText;

    for(auto elem : headers) {
        QString str = "";
        if(elem.first == "Host") {
            if(newHost.startsWith("http://"))
                elem.second = newHost.replace("http://", "");
            else
                elem.second = newHost;
        }
        str = elem.first + ": " + elem.second + "\n";
        newVisibleText.append(str);
    }
    newVisibleText.append("\n");
    requestEditor->clear();
    requestEditor->setPlainText(newVisibleText);
}

void MainWindow::startSendingRequests() {
    testingAborted = false;


    request.setUrl(url);
    for(auto elem : headers) {
        request.setRawHeader(elem.first.toUtf8().toBase64(), elem.second.toUtf8().toBase64());
    }
    qnam.get(request);
}

void MainWindow::setRequestRawHeaders() {
    const QString rawHeaders = requestEditor->toPlainText();
    std::vector<QString> lines;
    QString param = "", value = "";
    QString line;
    int switchFlag = 0;

    for(int i = 0; i < rawHeaders.length(); ++i) {
        if(rawHeaders[i] == '\n' && rawHeaders[i + 1] == '\n')
            break;
        if(rawHeaders[i] == '\n') {
            lines.push_back(line);
            line = "";
        } else {
            line += rawHeaders[i];
        }
    }

    for(int i = 0; i < lines.size(); ++i) {
        QString str = lines.at(i);
        switchFlag = 0;
        param = "";
        value = "";

        for(int j = 0; j < str.length(); ++j) {
          if(switchFlag == 0) {
              if(str.at(j) == ':') {
                 switchFlag = 2;
               } else {
                  param.append(str.at(j));
               }
           } else if(switchFlag == 2) {
               switchFlag = 1;
           } else {
               value.append(str.at(j));
           }
        }
        if(!(param == "" && value == "")) {
            headers.insert(std::pair<QString, QString>(param, value));
        }
    }
}

QString MainWindow::debugPlainTextEditorSetup(std::map<QString, QString> map) {
    QString result = "";
    for(auto elem : map) {
        result += ("|" + elem.first + "|" + elem.second + "|\n");
    }
    return result;
}
