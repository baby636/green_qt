import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3

TextField {
    id: self
    property real radius: 4
    bottomPadding: 6
    topPadding: 6
    leftPadding: 8
    rightPadding: 8
    font.pixelSize: 12
    background: Rectangle {
        implicitWidth: 200
        radius: self.radius
        opacity: self.activeFocus ? 1 : (self.enabled ? 0.8 : 0.5)
        color: constants.c500
        border.color: Qt.lighter(color)
        border.width: self.activeFocus ? 1 : 0
    }
}
