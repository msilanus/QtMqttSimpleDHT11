#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets/QMessageBox>
//#include <QtMqtt/QMqttClient>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_client = new QMqttClient(this);
    m_client->setHostname("eu.thethings.network");
    m_client->setPort(1883);
    m_client->setUsername("gestion-clim-lab-c12");
    m_client->setPassword("ttn-account-v2.GzYy6dxbgfWGUqkef_7JZ6YQjjlNLdCejB4bGS58BZM");

    connect(m_client, &QMqttClient::disconnected, this, &MainWindow::brokerDisconnected);
    connect(m_client, &QMqttClient::stateChanged, this, &MainWindow::stateChange);
    connect(m_client, &QMqttClient::messageReceived, this, &MainWindow::receivedMessage);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pbConnexion_clicked()
{
    if (m_client->state() == QMqttClient::Disconnected) {
        ui->pbConnexion->setText(tr("Deconnexion"));
        m_client->connectToHost();

    } else {
        ui->pbConnexion->setText(tr("Connexion"));
        m_client->disconnectFromHost();
    }
}

void MainWindow::brokerDisconnected()
{
    QMessageBox::critical(this, "Erreur", "Connexion avec le broker interrompue");
}

void MainWindow::stateChange()
{
    QString message;
    switch (m_client->state())
    {
        case 0 :message = "Déconnecté";break;
        case 1 :message = "En cours de connexion";break;
        case 2 :message = "Connecté";
            QString myTopic1 = "gestion-clim-lab-c12/devices/temp-hum-sensor-1/up/temperature_1";
            auto subscription = m_client->subscribe(myTopic1);
            if (!subscription) {
                QMessageBox::critical(this, "Erreur", "Impossible de souscrire au topic\n"+myTopic1);
                return;
            }
            QString myTopic2 = "gestion-clim-lab-c12/devices/temp-hum-sensor-1/up/relative_humidity_2";
            subscription = m_client->subscribe(myTopic2);
            if (!subscription) {
                QMessageBox::critical(this, "Erreur", "Impossible de souscrire au topic\n"+myTopic2);
                return;
            }
            break;
    }

    ui->statusBar->showMessage(message);
}

void MainWindow::receivedMessage(const QByteArray &message, const QMqttTopicName &topic)
{
    ui->statusBar->showMessage("Data",1000);
    QString unit;
    if(topic.name()=="gestion-clim-lab-c12/devices/temp-hum-sensor-1/up/relative_humidity_2")
    {
        unit = " %";
        ui->lbHum->setText(message+unit);
    }
    if(topic.name()=="gestion-clim-lab-c12/devices/temp-hum-sensor-1/up/temperature_1")
    {
        unit = " °C";
        ui->lbTemp->setText(message+unit);
    }

}

