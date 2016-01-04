#include "lan_chat.h"
#include "ui_lan_chat.h"
#include <string.h>
#include <QDebug>

#define MAX_MSG_SIZE 1000
#define MAX_OFFLINE_TIME 3

Lan_Chat::Lan_Chat(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Lan_Chat)
{
    ui->setupUi(this);
    server=new QUdpSocket();
    server->bind(QHostAddress::AnyIPv4 , 2222);

    msg_server=new QUdpSocket();
    msg_server->bind(QHostAddress::AnyIPv4 , 2223);

    connect(ui->send,SIGNAL(clicked(bool)),this,SLOT(send_to_remote()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(send_hello()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(check_offline()));
    connect(server,SIGNAL(readyRead()),this,SLOT(read_heartbeat()));
    connect(msg_server,SIGNAL(readyRead()),this,SLOT(read_peer_message()));
    connect(ui->remoteNodes,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(select_remote_Node(QListWidgetItem*)));
    timer.start(2 * 1000);
}

void Lan_Chat::read_heartbeat()
{
    QByteArray datagram;
    QHostAddress sender;
    quint16 senderPort;

    server->readDatagram(datagram.data(), MAX_MSG_SIZE, &sender, &senderPort);

    //qDebug()<<"Data "<<datagram.data()<<endl;
    if ( (strcmp(datagram.data(),"Hello") == 0) && !host_dict.contains(sender.toIPv4Address()))
    {
         QString data=sender.toString();
         QListWidgetItem *item = new QListWidgetItem;
         item->setData(0,data);
         ui->remoteNodes->addItem(item);
         host_dict[sender.toIPv4Address()]=0;
    }
    else if ((strcmp(datagram.data(),"Hello") == 0) && host_dict.contains(sender.toIPv4Address()))
    {
        host_dict[sender.toIPv4Address()]=0;
    }
}

void Lan_Chat::check_offline()
{

    QMap<quint32,int>::iterator it;
     QList<quint32> dead_peers;
    for (it=host_dict.begin();it!=host_dict.end();it++)
    {
       // qDebug()<<"i.val Before "<<it.value();
        if (it.value() > MAX_OFFLINE_TIME)
        {
            /*remote Node is offline*/
            //qDebug()<<"i.val After "<<it.value();
            ui->remoteNodes->clear();

            dead_peers<<it.key();
           // ui->remoteNodes->removeItemWidget(ui->remoteNodes->findItems(QString(it.value()),Qt::MatchExactly));

            for (QMap<quint32,int>::iterator it1=host_dict.begin();it1!=host_dict.end();it1++)
            {
                if (it1.key() != it.key() && it1.value() <=MAX_OFFLINE_TIME)
                {
                   ui->remoteNodes->addItem(QString(QHostAddress(it1.key()).toString()));
                   //ui->remoteNodes->append(QHostAddress(it1.key()).toString());
                }
            }
        }
        else
        {
            host_dict[it.key()]=host_dict[it.key()]+1;
        }
    }

    /* Remove Dead Peers*/
    quint32 el;
    foreach ( el , dead_peers)
    {
        host_dict.remove(el);
    }
}

void Lan_Chat::select_remote_Node(QListWidgetItem* item)
{
    QString str=item->text();
    //qDebug()<<"Peer Selected IP "<<str;
    peer_ip=QHostAddress(str).toIPv4Address();

}

void Lan_Chat::read_peer_message()
{
    QByteArray datagram;
    QHostAddress sender;
    quint16 senderPort;

    datagram.resize(MAX_MSG_SIZE);
    msg_server->readDatagram(datagram.data(), MAX_MSG_SIZE, &sender, &senderPort);

    //qDebug()<<"Data peer "<<datagram.data();
    if ( !host_dict.contains(sender.toIPv4Address()))
    {
         QString data=sender.toString();
         QListWidgetItem *item = new QListWidgetItem;
         item->setData(0,data);
         ui->remoteNodes->addItem(item);
         host_dict[sender.toIPv4Address()]=1;
    }

    ui->textBrowser->append(QString("<Peer>")+ QString(datagram.data()));

}

void Lan_Chat::send_to_remote()
{
    udpSocket.writeDatagram(ui->lineEdit->text().toStdString().c_str(), strlen(ui->lineEdit->text().toStdString().c_str()), QHostAddress(peer_ip), 2223);
    ui->textBrowser->setFontItalic(true);

    ui->textBrowser->append(QString("<Me>") + ui->lineEdit->text());
    ui->lineEdit->clear();
}

void Lan_Chat::send_hello()
{
     QString data("Hello");
     udpSocket.writeDatagram(data.toStdString().c_str(), strlen(data.toStdString().c_str()), QHostAddress::Broadcast, 2222);
}

Lan_Chat::~Lan_Chat()
{
    delete ui;
}
