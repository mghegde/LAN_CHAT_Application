#ifndef LAN_CHAT_H
#define LAN_CHAT_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QTimer>
#include <QMap>
#include <QListWidgetItem>


namespace Ui {
class Lan_Chat;
}

class Lan_Chat : public QMainWindow
{
    Q_OBJECT

public:
    explicit Lan_Chat(QWidget *parent = 0);
    ~Lan_Chat();

private:
    Ui::Lan_Chat *ui;
    QTimer timer;
    QUdpSocket udpSocket;
    QUdpSocket *server;
    QUdpSocket *msg_server;
    QMap<quint32,int> host_dict;
    quint32 peer_ip;

public slots:
    void read_heartbeat();
    void send_hello();
    void send_to_remote();
    void read_peer_message();
    void check_offline();
    void select_remote_Node(QListWidgetItem* item);
};

#endif // LAN_CHAT_H
