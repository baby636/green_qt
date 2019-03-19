import Blockstream.Green 0.1
import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5
import QtQuick.Controls.Material 2.3

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: 'Blockstream Green'


    Material.theme: Material.Dark

    Wallet {
        id: wallet
        Component.onCompleted: connect()
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: !wallet.connected
    }
}
